#include <iot_board.h>

#define SSID "iotProjectZGroup"
#define PASSWORD "marcotiamo"

#define DEVICE_NUMBER 4

enum State {
  STATE_CONFIG,
  STATE_ACTIVE_GREEN,
  STATE_ACTIVE_RED
};

enum MsgType {
  MSGTYPE_ACK,
  MSGTYPE_UP_ORIZZONTAL,
  MSGTYPE_UP_VERTICAL,
  MSGTYPE_ERROR
};

__attribute__((packed)) struct Msg {
  uint8_t msgType;
  uint8_t id;
};

int state;
bool isMaster;
bool isVertical;

int enterGreenTime;
int timestamp[DEVICE_NUMBER] = { 0 };
int masterTime;

int direction;
uint8_t id;

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
    state = STATE_ACTIVE_RED;
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

void receiveUpMsg(bool vertical) {
  if(isVertical == vertical) {
    enterGreenTime = millis();
    state = STATE_ACTIVE_GREEN;
  } else {
    state = STATE_ACTIVE_RED;
  }
}

void onMsg(const EspMeshEvent &event){
  if(event.len != sizeof(Msg))
    return;
  const Msg *msg = (const Msg *) event.data;

  if(isMaster) {
    if(msg->msgType == MSGTYPE_ACK) {
      uint8_t id = msg->id;
      if(id < DEVICE_NUMBER)
        timestamp[id] = millis();
    }
  } else {
    timestamp[0] = millis();

    switch (msg->msgType) {
      case MSGTYPE_UP_VERTICAL:
        receiveUpMsg(true);
        break;

      case MSGTYPE_UP_ORIZZONTAL:
        receiveUpMsg(false);
        break;

      case MSGTYPE_ERROR:
        state = STATE_CONFIG;
        break;
    }
    sendAck();
  }
}

void onMeshEvent(const EspMeshEvent &event) {
  switch(event.type) {
    case EspMeshEventType::Connected:
      isMaster = false;
      break;

    case EspMeshEventType::Disconnected:
      state = STATE_CONFIG;
      break;

    case EspMeshEventType::BecameRoot:
      isMaster = true;
      break;

    case EspMeshEventType::Message:
      onMsg(event);
      break;

    default:
      break;
  }
}

void setup() {
  sleep(3);

  IoTBoard::init_serial();
  IoTBoard::init_buttons();
  IoTBoard::init_display();
  IoTBoard::init_leds();

  IoTBoard::init_espmesh(SSID, PASSWORD, onMeshEvent);

  state = STATE_CONFIG;
  isMaster = false;
  isVertical = false;

  enterGreenTime = millis();
}

bool checkTimestamp(int epsilon){
  int now = millis();
  for(int i = 0; i < 4; i++) {
    // ignore master's own ID
    if(i == id)
      continue;

    if(now - timestamp[i] > epsilon) {
      return false;
    }
  }
  return true;
}

void masterLoop() {
  if(state == STATE_CONFIG) {
    Msg msg;
    msg.id = id;
    msg.msgType = MSGTYPE_ERROR;

    if(IoTBoard::runEvery(250, 1))
      espmesh->broadcast(&msg, sizeof(msg));

    if(checkTimestamp(500)) {
      state = STATE_ACTIVE_RED;
      masterTime = millis();
    }
  }

  if(state != STATE_CONFIG) {
    int now = millis();

    if((now - masterTime >= 6000)){
      masterTime = now;

      direction = !direction;
      int upDirection = (direction ? MSGTYPE_UP_ORIZZONTAL
                                   : MSGTYPE_UP_VERTICAL);

      Msg msg;
      msg.id = 0;
      msg.msgType = upDirection;

      espmesh->broadcast(&msg, sizeof(Msg));

      receiveUpMsg(upDirection);
    }
  }
}

void slaveLoop() {
  // TODO
}

void loop() {
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
