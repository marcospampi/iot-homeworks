#pragma once



class Heartbeat {
	public:
		/// @brief verifica che tutto è apposto
		static bool ok();
		
		/// @brief loop
		static void loop();
		
		/// @brief accetta un heatbeat
		static void hearbeat(uint16_t device_id);
};




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


 