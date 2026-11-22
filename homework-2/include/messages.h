#pragma once


/// @brief  tipo di messaggio
enum MessageType {
	MESSAGE_ADVERTISING,  	// il master che si advertizza
	MESSAGE_HEARTBEAT,					 	// heatbeat di client e server
	MESSAGE_GO										// comando accendere i semafori
};

struct Message { 
	int type; // MessageType
	int data;	// il parametro di GO
};

