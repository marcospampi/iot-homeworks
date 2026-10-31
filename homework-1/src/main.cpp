#include <iot_board.h>

#define SSID "iotProjectZGroup"
#define PASSWORD "marcotiamo"

int state;
bool isMaster;
bool isVertical;

int enterGreenTime;
int timestamp[4];
int masterTime;

int direction;

uint8_t id;

enum COLOR{
  RED,
  YELLOW,
  GREEN
};

enum STATE{
  CONFIG,
  ACTIVE_GREEN,
  ACTIVE_RED
};

enum MsgType{
  ACK,
  UP_ORIZZONTAL,
  UP_VERTICAL,
  ERROR
};

__attribute__((packed)) struct Msg{
  uint8_t msgType;
  uint8_t id;
};

void config(){
  if(IoTBoard::runEvery(1000,0)){
    digitalWrite(LED_YELLOW,!digitalRead(LED_YELLOW));
  }

}

void activeGreen(){
  uint64_t currentTime=millis();

  if(currentTime-enterGreenTime<4000){
    digitalWrite(LED_RED,LOW);
    digitalWrite(LED_GREEN,HIGH);
    digitalWrite(LED_YELLOW,LOW);
  }
  else if(currentTime-enterGreenTime<5000){
    digitalWrite(LED_RED,LOW);
    digitalWrite(LED_GREEN,LOW);
    digitalWrite(LED_YELLOW,YELLOW);
  }
  else{
    state=ACTIVE_RED;
  }
}

void activeRed(){
  digitalWrite(LED_RED,HIGH);
  digitalWrite(LED_GREEN,LOW);
  digitalWrite(LED_YELLOW,LOW);
}

void sendAck(){

  Msg msg;

  msg.id=id;
  msg.msgType=ACK;

  espmesh->sendToRoot(&msg,sizeof(msg));

}

void setState(int nextState){

  switch (nextState)
        {

        case UP_VERTICAL:
          if(isVertical==true){
            enterGreenTime=millis();
            state=ACTIVE_GREEN;
          }else{
            state=ACTIVE_RED;
          }
          break;

        case UP_ORIZZONTAL:
          if(isVertical==false){
            enterGreenTime=millis();
            state=ACTIVE_GREEN;
          }else{
            state=ACTIVE_RED;
          }
          break;

        case ERROR:
          state=CONFIG;
          break;
        }

}

void onMsg(const EspMeshEvent &event){
  if(event.len==sizeof(Msg)){

    const Msg *msg = (const Msg *) event.data;

    if(isMaster){

      if(msg->msgType==ACK){

        uint8_t id=msg->id;

        if(id<4)
          timestamp[id]=millis();

      }

    }

    else{

      timestamp[0]=millis();

        switch (msg->msgType)
        {

        case UP_VERTICAL:
          if(isVertical==true){
            enterGreenTime=millis();
            state=ACTIVE_GREEN;
          }else{
            state=ACTIVE_RED;
          }
          break;

        case UP_ORIZZONTAL:
          if(isVertical==false){
            enterGreenTime=millis();
            state=ACTIVE_GREEN;
          }else{
            state=ACTIVE_RED;
          }
          break;

        case ERROR:
          state=CONFIG;
          break;
        }

        sendAck();

      }
  }
}

void onMeshEvent(const EspMeshEvent &event){

  switch(event.type){

    case EspMeshEventType::Connected:
      isMaster=false;
      break;

    case EspMeshEventType::Disconnected:
      state=CONFIG;
      break;

    case EspMeshEventType::BecameRoot:
      isMaster=true;
      break;

    case EspMeshEventType::Message:
      onMsg(event);
      break;
  }
};

void setup()
{
  sleep(3);

  IoTBoard::init_serial();
  IoTBoard::init_buttons();
  IoTBoard::init_display();
  IoTBoard::init_leds();

  IoTBoard::init_espmesh(SSID,PASSWORD,onMeshEvent);

  state=ACTIVE_GREEN;
  isMaster=false;
  isVertical=false;

  enterGreenTime=millis();

  memset(timestamp,0,sizeof(timestamp));

}

bool checkTimestamp(int epsilon){

  int now=millis();

  for(int i=0;i<4;i++){

    if(i==id)
      continue;

    if(now-timestamp[i]>epsilon){
      return false;
    }
  }
  return true;
}

void master(){

  if(state==CONFIG){

    Msg msg;
    msg.id=id;
    msg.msgType=ERROR;

    if(IoTBoard::runEvery(250,1))
      espmesh->broadcast(&msg,sizeof(msg));

    if(checkTimestamp(500)){
      state=ACTIVE_RED;
      masterTime=millis();
    }

  }
  if(state!=CONFIG){

    int now=millis();

    if((now-masterTime>=6000)){
      masterTime=now;
      direction=!direction;

      Msg msg;
      msg.msgType=(direction ? UP_ORIZZONTAL : UP_VERTICAL);
      msg.id=0;

      espmesh->broadcast(&msg,sizeof(Msg));

      setState(msg.msgType);

    }

  }

}

void loop() {

  switch(state){

    case CONFIG:
      config();
      break;

    case ACTIVE_GREEN:
      activeGreen();
      break;

    case ACTIVE_RED:
      activeRed();
      break;
  }

  if(isMaster){
    master();
  }
  //todo: slave();

}
