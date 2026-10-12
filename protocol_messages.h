#ifndef PROTOCOL_MESSAGES_H
#define PROTOCOL_MESSAGES_H

#include "contiki.h"
#include "protocol_stack.h"
#include "net/rime/rime.h" 
#include <stdint.h>
#include <stdio.h>


/*MACROS for MSG_TYPE broadcast*/
#define msgType_BC_AUTODISCOVERY_START           0x01 /*broadcast messaje for autodiscovery*/
#define payload_msgType_BC_AUTODISCOVERY_START   0x00 /*payload for the broadcast message for autodiscovery*/
/*MACROS for MSG_TYPE unicast*/
#define msgType_UC_JOIN_EXISTING_NETWORK         0x02 /*message unicast from a network to join*/
/*MACROS for HEARTBEAT*/
#define msgType_HB_HEARTBEAT                     0x03 /*heartbeat message*/
#define payload_msgType_HB_HEARTBEAT             0x00 /*payload for the heartbeat message*/


void message_discovery_searching(void);
void message_send_broadcast(const char* payload, uint8_t payloadLen, uint8_t msgType);
uint8_t message_search_for_type_BC(uint8_t type);
uint8_t message_search_for_type_UC(uint8_t type);
uint8_t message_process_all_BC(uint8_t type);
void log_print_nods_id(void);
void log_print_routing_table(void);
void message_clear_buffer_id(void);
uint8_t message_return_index_nr_nodes();

#endif