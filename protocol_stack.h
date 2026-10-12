#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "contiki.h"
#include "net/rime/rime.h" 
#include <stdint.h>
#include <stdio.h>

#define NrMaxBytes 16 /*Define max packet length*/
#define MAX_NDR_NODES 100

typedef enum 
{
    PROCESSER = 0,
    PRODUCER = 1,
    CONSUMER = 2
    
}nodeType;


typedef struct NODE 
{
    uint8_t id; /*unique id for each node*/
    uint8_t metric;
    uint16_t timerHeartBeat; /*timer for the heart beat*/

}NODE;

typedef struct DATA 
{
    //uint8_t id;      
    //uint8_t roleCode;
    nodeType type;
    NODE nodeInfo;   
    uint8_t msgType; 
    uint8_t msgLen;
    uint8_t payload[NrMaxBytes];   

}DATA;


extern NODE node;
DATA dataTxBroadCast;
DATA dataTxUniCast;


void init_com_channels(void);
void send_message_unicast(linkaddr_t *dest, DATA *data);
void send_message_broadcast(DATA *data);

uint8_t pop_data_comUc_stack(uint8_t msgType, DATA* dataFound);
uint8_t pop_data_comBc_stack(uint8_t msgType, DATA* dataFound);
void protocol_reset_stack_BC();
void protocol_reset_stack_UC();

void protocol_print_status_node(void);
linkaddr_t protocol_get_min_target_metric(nodeType target_type);



#endif 