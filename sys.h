#ifndef SYS_H
#define SYS_H
#include"protocol_stack.h"
#include "lib/random.h"

/*MACROS for System State*/
#define state_AUTODISCOVERY        0x00 /*broadcast messaje for autodiscovery*/
#define state_LISTEN               0x01 /*broadcast messaje for autodiscovery*/
#define state_IDLE                 0x02 
#define state_START_HEART_BEAT     0x03 

struct ctimer timer01; /*100 ms timer isr*/

void sys_init_random_node_values(void);
void sys_heart_beat_handler_tx_start(void);

#endif