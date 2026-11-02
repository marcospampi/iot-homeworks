#include <iot_board.h>

#include "secrets.h"

#define DEBUG 1

#ifndef N_OF_DEVICES
#define N_OF_DEVICES 4
#endif

#define PING_ALIVE_THRESHOLD 1000

enum State {
  STATE_CONFIG,
  STATE_ACTIVE_GREEN,
  STATE_ACTIVE_RED
};

enum MsgType {
  MSGTYPE_UP_HORIZONTAL=0,
  MSGTYPE_UP_VERTICAL=1,
  MSGTYPE_PING,
  MSGTYPE_RESYNC
};

enum Direction {
  DIRECTION_HORIZONTAL=0,
  DIRECTION_VERTICAL=1,
  DIRECTION_COUNT
};

typedef struct __attribute__((packed)) {
  uint8_t msgType;
  uint8_t id;
} Msg;

struct Semaphore {
  int  id;
  bool isMaster;

  int direction;
  int enterGreenTime;
  int enterRedTime;
} semaphoreData;

struct Game {
  int upDirection;
  int time;
  int timestamp[N_OF_DEVICES];
} gameData;

int state;

void setState(State newState) {
  if(DEBUG)
    Serial.printf("setting state %d\n", newState);
  state = newState;
}

void writeDisplay(const char *txt="", int time=-1, int16_t x=0, int16_t y=0) {
  if(semaphoreData.isMaster) {
    display->setTextSize(1);
    display->setCursor(0, 0);
    display->print("Master");
  }

  display->setTextSize(2);
  display->setCursor(x, y);
  display->print(txt);

  if(time >= 0) {
    display->setCursor(50, 45);
    display->print(time);
    display->print("s");
  }
  display->display();
}

void setLEDs(int red, int yellow, int green) {
  digitalWrite(LED_RED,    red);
  digitalWrite(LED_YELLOW, yellow);
  digitalWrite(LED_GREEN,  green);
}

void config() {
  if(IoTBoard::runEvery(1000, 0))
    setLEDs(0, !digitalRead(LED_YELLOW), 0);
}

void activeRed() {
  const int now = millis();
  const int redTime = now - semaphoreData.enterRedTime;

  setLEDs(1, 0, 0);

  int countdown = (6000 - redTime + 999) / 1000;
  if(countdown < 0)
    countdown = 0;

  display->clearDisplay();
  writeDisplay("FERMATI", countdown, 22, 18);

}

void activeGreen() {
  const int now = millis();
  const int greenTime = now - semaphoreData.enterGreenTime;

  int countdown = (6000 - greenTime + 999) / 1000;
  if(countdown < 0)
    countdown = 0;

  display->clearDisplay();

  if(greenTime < 5000) {
    setLEDs(0, 0, 1);
    writeDisplay("SGASA BRO", countdown, 10, 18);
  } else if(greenTime < 6000) {
    setLEDs(0, 1, 0);
    writeDisplay("RALLENTA", countdown, 16, 18);
  } else {
    semaphoreData.enterRedTime = now + 1000;
    setState(STATE_ACTIVE_RED);
    activeRed();
  }
}

void receiveUpMsg(int msgType) {
  if(semaphoreData.direction == msgType) {
    setState(STATE_ACTIVE_GREEN);
    semaphoreData.enterGreenTime = millis();
  } else {
    setState(STATE_ACTIVE_RED);
    semaphoreData.enterRedTime = millis();
  }
}

void onMsg(const EspMeshEvent &event) {
  if(DEBUG)
    Serial.printf("MeshEvent: len=%d\n", event.len);

  if(event.len != sizeof(Msg))
    return;
  const Msg *msg = (const Msg *) event.data;

  if(semaphoreData.isMaster) {
    if(msg->msgType == MSGTYPE_PING) {
      int id = msg->id;
      if(id > 0 && id < N_OF_DEVICES)
        gameData.timestamp[id] = millis();

      if(DEBUG)
        Serial.printf("PING from id=%d\n", id);
    }
  } else {
    switch (msg->msgType) {
      case MSGTYPE_UP_VERTICAL:
        receiveUpMsg(MSGTYPE_UP_VERTICAL);
        if(DEBUG)
          Serial.printf("Received UP_VERTICAL\n");
        break;

      case MSGTYPE_UP_HORIZONTAL:
        receiveUpMsg(MSGTYPE_UP_HORIZONTAL);
        if(DEBUG)
          Serial.printf("Received UP_HORIZONTAL\n");
        break;

      case MSGTYPE_RESYNC:
        setState(STATE_CONFIG);
        if(DEBUG)
          Serial.printf("Received RESYNC\n");
        break;
    }
  }
}

void onMeshEvent(const EspMeshEvent &event) {
  switch(event.type) {
    case EspMeshEventType::Connected:
      if(DEBUG)
        Serial.printf("MeshEvent Connected\n");
      semaphoreData.isMaster = false;
      break;

    case EspMeshEventType::Disconnected:
      if(DEBUG)
        Serial.printf("MeshEvent Disconnected\n");
      setState(STATE_CONFIG);
      break;

    case EspMeshEventType::BecameRoot:
      if(DEBUG)
        Serial.printf("MeshEvent BecameRoot\n");
      semaphoreData.isMaster = true;
      break;

    case EspMeshEventType::Message:
      if(DEBUG)
        Serial.printf("MeshEvent Message\n");
      onMsg(event);
      break;
    case EspMeshEventType::ChildJoined:
    case EspMeshEventType::ChildLeft:
    default:
      break;
  }
}

void onBtn1Released(uint8_t pinBtn){
  Serial.println("BTN 1 pressed");

  semaphoreData.id++;
  if(semaphoreData.id >= N_OF_DEVICES)
    semaphoreData.id = 0;
  Serial.printf("Device ID changed to %d\n", semaphoreData.id);
}

void onBtn2Released(uint8_t pinBtn){
  Serial.println("BTN 2 pressed");

  semaphoreData.direction = (semaphoreData.direction + 1) % DIRECTION_COUNT;
  Serial.printf("Direction changed to %d\n", semaphoreData.direction);
}

void setup() {
  sleep(3);

  IoTBoard::init_serial();
  IoTBoard::init_buttons();
  IoTBoard::init_display();
  IoTBoard::init_leds();

  buttons->onBtn1Release(onBtn1Released);
  buttons->onBtn2Release(onBtn2Released);

  Serial.printf("Initializing mesh... ");
  IoTBoard::init_espmesh(SSID, PASSWORD, onMeshEvent);
  Serial.printf("Done\n");

  // set initial state
  setState(STATE_CONFIG);
  semaphoreData.id             = 0;
  semaphoreData.isMaster       = false;
  semaphoreData.direction      = DIRECTION_HORIZONTAL;
  semaphoreData.enterGreenTime = millis();
  semaphoreData.enterRedTime   = millis();
}

bool areSlavesAlive() {
  int now = millis();
  for(int i = 0; i < N_OF_DEVICES; i++) {
    // do not check master's own ID
    if(i == semaphoreData.id)
      continue;

    // if slave's timestamp is not fresh enough, consider it dead
    if(now - gameData.timestamp[i] > PING_ALIVE_THRESHOLD)
      return false;
  }
  return true;
}

void sendToSlaves(const Msg &msg) {

  int numofdevices = esp_mesh_get_routing_table_size();
  if ( IoTBoard::runEvery(1000, 5) && DEBUG) {
    Serial.printf("Number of devices is %d\n", numofdevices);
  }

  uint8_t masterMAC[6];
  espmesh->getMac(masterMAC);

  uint8_t nodes[numofdevices][6];
  espmesh->getNodes(nodes, numofdevices);

  for ( int i = 0; i < numofdevices; ++i ) {
    // non controllo il master
    if ( memcmp(masterMAC, nodes[i], sizeof(uint8_t[6])) == 0 ) continue;
    espmesh->sendTo(nodes[i], &msg, sizeof(Msg));
  }
}

void masterLoop() {
  if(state == STATE_CONFIG) {
    // send a RESYNC message every 250ms
    if(IoTBoard::runEvery(250, 1)) {
      Msg msg;
      msg.id = 0;
      msg.msgType = MSGTYPE_RESYNC;

      sendToSlaves(msg);

      if(DEBUG)
        Serial.println("Sending config broadcast message");
    }

    if(areSlavesAlive()) {
      setState(STATE_ACTIVE_RED);
      gameData.time = millis();
      semaphoreData.enterRedTime = millis();
    }
  }

  if(state != STATE_CONFIG) {
    int now = millis();

    if((now - gameData.time >= 6000)) {
      gameData.time = now;

      gameData.upDirection = !gameData.upDirection;
      int msgType = (gameData.upDirection ? MSGTYPE_UP_HORIZONTAL
                                          : MSGTYPE_UP_VERTICAL);

      Msg msg;
      msg.id = 0;
      msg.msgType = msgType;

      sendToSlaves(msg);
      receiveUpMsg(msgType); // update the master's semaphore too
    }

    if(!areSlavesAlive())
      setState(STATE_CONFIG);
  }
}

void slaveLoop() {
  // send a PING message to master
  if(IoTBoard::runEvery(250, 7)) {
    Msg msg;
    msg.id = semaphoreData.id;
    msg.msgType = MSGTYPE_PING;

    espmesh->sendToRoot(&msg,sizeof(msg));
  }
}

void loop() {
  buttons->update();
  espmesh->update();

  switch(state) {
    case STATE_CONFIG:
      config();
      display->clearDisplay();
      writeDisplay("CONFIG", -1, 28, 25);
      break;

    case STATE_ACTIVE_GREEN:
      activeGreen();
      break;

    case STATE_ACTIVE_RED:
      activeRed();
      break;
  }

  if(semaphoreData.isMaster)
    masterLoop();
  else
    slaveLoop();
}
