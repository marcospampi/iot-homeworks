#include <iot_board.h>

#include "secrets.h"

#define DEBUG 1

#ifndef N_OF_DEVICES
#define N_OF_DEVICES 4
#endif

enum State {
  STATE_CONFIG,
  STATE_ACTIVE_GREEN,
  STATE_ACTIVE_RED
};

enum MsgType {
  MSGTYPE_ACK,
  MSGTYPE_UP_HORIZONTAL,
  MSGTYPE_UP_VERTICAL,
  MSGTYPE_ERROR
};

typedef struct __attribute__((packed))  {
  uint8_t msgType;
  uint8_t id;
} Msg;

int state;
bool isMaster;
bool isVertical;

int enterGreenTime;
int timestamp[N_OF_DEVICES] = { 0 };
int masterTime;

int direction;
int id;

void setState(State newState) {
  if(DEBUG)
    Serial.printf("setting state %d\n", newState);
  state = newState;
}

void write_display(const char *txt) {
  display->clearDisplay();
  display->setCursor(0,0);
  display->print(txt);
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

void activeGreen() {
  uint64_t currentTime = millis();

  if(currentTime-enterGreenTime < 4000)
    setLEDs(0, 0, 1);
  else if(currentTime-enterGreenTime < 5000)
    setLEDs(0, 1, 0);
  else
    setState(STATE_ACTIVE_RED);
}

void activeRed() {
  setLEDs(1, 0, 0);
}

void sendAck() {
  Msg msg;
  msg.id = id;
  msg.msgType = MSGTYPE_ACK;

  espmesh->sendToRoot(&msg,sizeof(msg));
}

void receiveUpMsg(int msgType) {
  bool shouldTurnGreen = (
    (isVertical  && msgType == MSGTYPE_UP_VERTICAL) ||
    (!isVertical && msgType == MSGTYPE_UP_HORIZONTAL)
  );

  if(shouldTurnGreen) {
    setState(STATE_ACTIVE_GREEN);
    enterGreenTime = millis();
  } else {
    setState(STATE_ACTIVE_RED);
  }
}

void onMsg(const EspMeshEvent &event) {
  if(DEBUG)
    Serial.printf("MeshEvent: len=%d\n", event.len);

  if(event.len != sizeof(Msg))
    return;
  const Msg *msg = (const Msg *) event.data;

  if(isMaster) {
    if(msg->msgType == MSGTYPE_ACK) {
      uint8_t id = msg->id;
      if(id < N_OF_DEVICES)
        timestamp[id] = millis();

      if(DEBUG)
        Serial.printf("ACK from id=%d\n", id);
    }
  } else {
    timestamp[0] = millis();

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

      case MSGTYPE_ERROR:
        setState(STATE_CONFIG);
        if(DEBUG)
          Serial.printf("Received ERROR\n");
        break;
    }
    sendAck();
  }
}

void onMeshEvent(const EspMeshEvent &event) {
  switch(event.type) {
    case EspMeshEventType::Connected:
      if(DEBUG)
        Serial.printf("MeshEvent Connected\n");
      isMaster = false;
      break;

    case EspMeshEventType::Disconnected:
      if(DEBUG)
        Serial.printf("MeshEvent Disconnected\n");
      setState(STATE_CONFIG);
      break;

    case EspMeshEventType::BecameRoot:
      if(DEBUG)
        Serial.printf("MeshEvent BecameRoot\n");
      write_display("Sono master");
      isMaster = true;
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

  id++;
  if(id >= N_OF_DEVICES)
    id = 0;
  Serial.printf("Device ID changed to %d\n", id);
}

void onBtn2Released(uint8_t pinBtn){
  Serial.println("BTN 2 pressed");

  id--;
  if(id < 0)
    id = N_OF_DEVICES - 1;
  Serial.printf("Device ID changed to %d\n", id);
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

  setState(STATE_CONFIG);
  isMaster = false;
  isVertical = false;

  enterGreenTime = millis();
}

bool checkTimestamp(int epsilon) {
  int now = millis();
  for(int i = 0; i < N_OF_DEVICES; i++) {
    // ignore master's own ID
    if(i == id)
      continue;

    if(now - timestamp[i] > epsilon) {
      return false;
    }
  }
  return true;
}

void masterBroadcast(const Msg &msg) {
  // TODO: puliziari
  // Msg msg;
  // msg.id = id;
  // msg.msgType = eventTy;

  int numofdevices = esp_mesh_get_routing_table_size();
  if ( IoTBoard::runEvery(1000, 5)) {
    Serial.printf("Number of devices is %d\n", numofdevices);
  }
  uint8_t me[6];
  espmesh->getMac(me);

  uint8_t nodes[numofdevices][6];
  espmesh->getNodes(nodes, numofdevices);

  for ( int i = 0; i < numofdevices; ++i ) {
    // non controllo a me stesso
    if ( memcmp(me, nodes[i], sizeof(uint8_t[6])) == 0 ) continue;
    espmesh->sendTo(nodes[i], &msg, sizeof(Msg));
  }
}

void masterLoop() {
  if(state == STATE_CONFIG) {
    if(IoTBoard::runEvery(250, 1)) {
      Msg msg;
      msg.id = id;
      msg.msgType = MSGTYPE_ERROR;
      //espmesh->broadcast(&msg, sizeof(msg));
      masterBroadcast(msg);
      if(DEBUG)
        Serial.println("Sending config broadcast message");
    }

    if(checkTimestamp(500)) {
      setState(STATE_ACTIVE_RED);
      masterTime = millis();
    }
  }

  if(state != STATE_CONFIG) {
    int now = millis();

    if((now - masterTime >= 6000)) {
      masterTime = now;

      direction = !direction;
      int msgType = (direction ? MSGTYPE_UP_HORIZONTAL
                               : MSGTYPE_UP_VERTICAL);

      Msg msg;
      msg.id = 0;
      msg.msgType = msgType;

     // espmesh->broadcast(&msg, sizeof(Msg));
      masterBroadcast(msg);

      receiveUpMsg(msgType);
    }

    // TODO checkTimestamp
  }
}

void slaveLoop() {
  // TODO
}

void loop() {
  buttons->update();
  espmesh->update();

  switch(state) {
    case STATE_CONFIG:
      config();
      break;

    case STATE_ACTIVE_GREEN:
      activeGreen();
      break;

    case STATE_ACTIVE_RED:
      activeRed();
      break;
  }

  if(isMaster)
    masterLoop();
  else
    slaveLoop();

}
