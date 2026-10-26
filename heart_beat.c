#include "heart_beat.h"
#include "routing.h"
#include "protocol_messages.h"

extern ROUTING_TABLE routingTable;
uint8_t flagStartHeartBeatTimer = 0;

static uint32_t last_hb_time = 0;

static void debug_timer_time()
{
    uint32_t now = clock_time();
    uint32_t delta = (last_hb_time == 0) ? 0 : ((now - last_hb_time) * 1000UL) / CLOCK_SECOND;
    printf("DEBUG: Heartbeat time: %lu ms\n", (unsigned long)delta);
    last_hb_time = now;
}

void sys_start_heartbeat(void)
{
    /*
     * Function that will be called at the beggining of the 
     * state state_START_HEART_BEAT  
    */

    flagStartHeartBeatTimer = 1;
    printf("START_HEART_BEAT: next heartbeat window=%u ms\n", node.timerHeartBeat*100);
}

void handle_heart_beat_send(void)
{
    /*send heartbeat message Broadcast*/

    if(flagStartHeartBeatTimer)
    {
        /* timer arrived, send heartbeat */
        char myPayload[1] = {payload_msgType_HB_HEARTBEAT};
        
        message_send_broadcast(myPayload, 
                              sizeof(myPayload), 
                              msgType_HB_HEARTBEAT);

        printf("HEARTBEAT SENT...test...passed %u ms :)\n", node.timerHeartBeat);
        debug_timer_time();
    }
}


void inc_timer_heart_beat_tick_routing_table(void)
{
    /*function that will handle all the current registered node heartbeat in the
    routing table by incrementic their ticks. It is important that the timer cb is kept short,
    so in an separate process, it will periodically check the ticks*/

    uint8_t i = 0;

    for(i=0; i< routingTable.indexConsumers; i++)
    {
        routingTable.routingTableConsumers[i].timerTick++;
    }

    for(i=0; i< routingTable.indexProducers; i++)
    {
        routingTable.routingTableProducers[i].timerTick++;
    }

    for(i=0; i< routingTable.indexProcessers; i++)
    {
        routingTable.routingTableProcessers[i].timerTick++;
    }

}


void check_heart_beat_tick_routing_table(void)
{
    /*function that will handle all the current registered node heartbeat in the
    routing table*/
    int16_t i = 0;

    for(i=routingTable.indexConsumers-1; i>=0; i--)
    {
        if(routingTable.routingTableConsumers[i].timerTick >= 4*routingTable.routingTableConsumers[i].timerHeartBeat)
        {
            /*EXPIRED TIMER! delete it from the list*/
            printf("Deleted node from routing table consumer:%i\n", routingTable.routingTableConsumers[i].id);
            delete_node_from_table(CONSUMER, i);
            i--; 
        }
    }

    for(i=routingTable.indexProducers-1; i>=0; i--)
    {
        if(routingTable.routingTableProducers[i].timerTick >= 4*routingTable.routingTableProducers[i].timerHeartBeat)
        {
            printf("Deleted node from routing table producer:%i\n", routingTable.routingTableProducers[i].id);
            delete_node_from_table(PRODUCER, i);
            i--;
        }
    }

    for(i=routingTable.indexProcessers-1; i>=0; i--)
    {
        if(routingTable.routingTableProcessers[i].timerTick >= 4*routingTable.routingTableProcessers[i].timerHeartBeat)
        {
            printf("Deleted node from routing table processer:%i\n", routingTable.routingTableProcessers[i].id);
            delete_node_from_table(PROCESSER, i);
            i--;
        }
    }

}

inline bool check_heart_beat_tick_all_routing_table(void)
{
    /*function that will handle all the current registered node heartbeat in the
    routing table*/

    if(routingTable.indexConsumers == 0 && 
       routingTable.indexProducers == 0 && 
       routingTable.indexProcessers == 0)
    {
        printf("[INFO]All routing tables are empty! Dead NODE!\n");
        return true;
    }

    return false;
}

void reset_heart_beat_tick_routing_table_node(DATA nodeData)
{
    /*this funciton will go directly in the current recieved heartbeat message from another node
    and it will reset the counter */
    uint8_t i = 0;
    printf("[HEARTBEAT RESET]Tha heartbeat tick is about to be RESETTED!\n");
    
    switch(nodeData.type)
    {
        case PROCESSER:
            printf("[HEARTBEAT RESET] PROCESSER %i cleared.\n", nodeData.nodeInfo.id);

            for(i=0; i<routingTable.indexProcessers; i++)
            {
                if(nodeData.nodeInfo.id == routingTable.routingTableProcessers[i].id)
                {
                    routingTable.routingTableProcessers[i].timerTick = 0;
                    break;
                }
            }
            
            break;

        case PRODUCER:
            printf("[HEARTBEAT RESET] PRODUCER %i cleared.\n", nodeData.nodeInfo.id);
            
            for(i=0; i<routingTable.indexProducers; i++)
            {
                if(nodeData.nodeInfo.id == routingTable.routingTableProducers[i].id)
                {
                    routingTable.routingTableProducers[i].timerTick = 0;
                    break;
                }
            }
            break;

        case CONSUMER:
            printf("[HEARTBEAT RESET] CONSUMER %i cleared.\n", nodeData.nodeInfo.id);
            for(i=0; i<routingTable.indexConsumers; i++)
            {
                if(nodeData.nodeInfo.id == routingTable.routingTableConsumers[i].id)
                {
                    routingTable.routingTableConsumers[i].timerTick = 0;
                    break;
                }
            }
            break;
        
        default:
        break;
    }
    
}