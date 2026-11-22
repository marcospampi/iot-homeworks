#include "heartbeat.h"
#include "messages.h"
#include "settings.h"
#include "device.h"
#include <iot_board.h>

#define HEARTBEAT_OK_TIME_MS 1000
#define HEARTBEAT_SEND_EVERY_MS 250
#define MAX_PEERS 64

struct Peer {
  uint16_t peer_id;
  uint32_t timestamp;
} peers[MAX_PEERS];

bool Heartbeat::ok() {
  int expected_count = settings.is_master ? settings.device_count : 1;
  int count = 0;
  int curr_time = millis();

  for (int i = 0; i < MAX_PEERS; ++i) {
    Peer *peer = &peers[i];
    if (peer->peer_id != 0 &&
        curr_time - peer->timestamp < HEARTBEAT_OK_TIME_MS) {
      count++;
    } else {
      peer->peer_id = 0;
      peer->timestamp = 0;
    }
  }
  return expected_count == count;
}

void Heartbeat::loop() {
	// non vogliamo mandare heartbeat in config!
	if ( DeviceState::get_state() == DEVICE_CONFIG ) return;
	
  // todo: fare header dove allochiamo i timer, runEvery()
  if (IoTBoard::runEvery(HEARTBEAT_SEND_EVERY_MS, 7)) {
    Message heartbeat;
    heartbeat.type = MESSAGE_ADVERTISING;

    for (int i = 0; i < MAX_PEERS; ++i) {
      Peer *peer = &peers[i];
      if (peer->peer_id != 0) {
        zigbee->startPacket();
        zigbee->writeBytes((uint8_t*)&heartbeat, sizeof(hearbeat));
        zigbee->sendPacket(peer->peer_id, true);

				if ( !settings.is_master ) return;
      }
    }
  }
}
  void Heartbeat::hearbeat(uint16_t device_id) {
    // cerco l'array
    for (int i = 0; i < MAX_PEERS; ++i) {
      // è già registrato?
      if (peers[i].peer_id == device_id) {
        // allora setto il suo timespamp
        peers[i].timestamp = millis();
        // torno prima
        return;
      }
    }
    // altrimenti non lo ho trovato, non è registrato
    for (int i = 0; i < MAX_PEERS; ++i) {
      // cerco uno slot vuoto
      if (peers[i].peer_id == 0) {
        // lo registro
        peers[i].peer_id = device_id;
        peers[i].timestamp = millis();
        return;
      }
    }
    // TODO: gestire errore che non ci sono slot vuoti
  }