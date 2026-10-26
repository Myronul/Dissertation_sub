#include "sys.h"
#include "heart_beat.h"

nodeType hostNodeType;
struct ctimer timer01; /*100 ms timer isr*/
extern struct process process_heartbeat;
extern uint8_t timerHeartBeatRNG_counter;
extern uint8_t flagStartHeartBeatTimer;
extern uint16_t timerHeartBeatRNG_max;

static void sys_init_random_node_type(void)
{
    hostNodeType = (nodeType)(random_rand()%3); /*randomly assign a node type*/
    printf("[INFO]Node %u assigned type: %u\n", node.id, hostNodeType);
}

void sys_init_random_node_values(void)
{
    sys_init_random_node_type();
    node.metric = random_rand()%100; /*randomly assign a metric value*/
    node.timerHeartBeat = 500 + (100 * (random_rand() % 10)); /* *100ms */
    node.timerTick = 0;
    printf("[INFO]Node %u assigned metric: %u, heartbeat timer: %u ms\n", node.id, node.metric, node.timerHeartBeat);
}

static void sys_timer01_cb_(void* ptr)
{
    /* universal timer of 100ms interrupt cb*/
    if(flagStartHeartBeatTimer)
    {
        node.timerTick++;

        if(node.timerTick >= node.timerHeartBeat)
        {
            /*send heartbeat in the network*/
            process_post(&process_heartbeat, PROCESS_EVENT_CONTINUE, NULL);
            node.timerTick = 0;
        }

        inc_timer_heart_beat_tick_routing_table(); /*inc for all nodes*/
    }

    ctimer_reset(&timer01);
}

void sys_heart_beat_handler_tx_start(void)
{
    /* node.timerHeartBeat is in milliseconds; ctimer uses clock ticks */
    clock_time_t delay_ticks = (clock_time_t)((CLOCK_SECOND * 100) / 1000); /*500ms*/
    ctimer_set(&timer01, delay_ticks, sys_timer01_cb_, NULL);
}