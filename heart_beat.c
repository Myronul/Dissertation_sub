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
    printf("START_HEART_BEAT: next heartbeat window=%u ms\n", node.timerHeartBeat);
}

void handle_heart_beat_send(void)
{
    /*send heartbeat message Broadcast*/

    if(flagStartHeartBeatTimer)
    {
        /* timer arrived, send heartbeat */
        message_send_broadcast(payload_msgType_HB_HEARTBEAT, 
                              sizeof(payload_msgType_HB_HEARTBEAT), 
                              msgType_HB_HEARTBEAT);

        printf("HEARTBEAT SENT...test...passed %u ms :)\n", node.timerHeartBeat);
        debug_timer_time();
    }
}