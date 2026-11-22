#pragma once
#include <cstdint>
#include "messages.h"

class Protocol {
	public:
		/// @brief  initializes zigbee
		static void init();
		/// @brief verifica che tutto è apposto
		static bool ok();
		
		/// @brief loop
		static void loop();

		/// @brief  sends advertise message in broadscast mode
		static void advertise();

		/// @brief sends gi nessage for group
		/// @param group 
		static void send_go_message(int group);
	private:
		/// @brief accetta un heatbeat
		static void accept_heartbeat(uint16_t device_id);
		static void accept_go(int);
		static void accept_advertising(uint16_t device_id);
};


/**
 * vericare che abbiamo i timestamp freschi e non facciano puzza di pesce
 * loop che manda gli heartbeat
 * accettare gli heartbeat
 */

/**
 *
 * is_master:
 * 	heatbeat a tutti
 * not(is_master):
 * 	heatbeat al master
 * 
 * hearbeat->loop(); <-- 
 * hearbeat->ok();   <--
 * 
 * 
 * 
 * device_lists = [] is_master <-- heatbeat n dispotivi, slave <-- heartbeat 1 dispotivo
 * send_hearbeat() <-- mando io
 * recv_hearbeat() <-- ricevo e traccio
 * 
 * loop():
 * 	se un dispositivo è disconnesso ( non abbiamo hearbeat da TOT tempo )
 * 	allora lo eliminiamo
 */


 