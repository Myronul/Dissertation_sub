#ifndef ROUTING_H
#define ROUTING_H

#include "protocol_stack.h"

typedef struct ROUTING_TABLE
{
    NODE routingTableConsumers[MAX_NDR_NODES]; /*routing table for consumers*/
    NODE routingTableProducers[MAX_NDR_NODES]; /*routing table for producers*/
    NODE routingTableProcessers[MAX_NDR_NODES]; /*routing table for processers*/
    uint8_t indexConsumers; /*index for the routing table of consumers*/
    uint8_t indexProducers; /*index for the routing table of producers*/
    uint8_t indexProcessers; /*index for the routing table of processers*/
    
}ROUTING_TABLE;


void heapify_routing_table_all(void);
void heapify_routing_table_processers(void);
void heapify_routing_table_producers(void);
void heapify_routing_table_consumers(void);

#endif 