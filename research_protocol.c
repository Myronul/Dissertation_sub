#include "contiki.h"
#include "protocol_stack.h"
#include "protocol_messages.h"
#include "net/rime/rime.h"
#include "lib/random.h"
#include "sys.h"
#include "heart_beat.h"
#include <stdio.h>
#include <string.h>

uint8_t SystemState = 0; /*global state for the entire system*/

static clock_time_t generate_random_time(void)
{
    int jitter = 1 + random_rand()%1000;
    clock_time_t t = ((CLOCK_SECOND * jitter) / 1000)+100; /*add 100 ticks to avoid 0 time*/
    printf("wait...%lu\n", t);
    if(t == 0) t = 1;
    
    return t;
}

static void Log_print_state()
{
    printf("Current state: %i\n", SystemState);
}

PROCESS(process_init_node, "Protocol Init Node");
PROCESS(process_heartbeat, "Protocol Process HeartBeat");
AUTOSTART_PROCESSES(&process_init_node, &process_heartbeat);


PROCESS_THREAD(process_init_node, ev, data)
{
    static struct etimer timer;
    static uint8_t counter = 0;
    static uint8_t counter_t = 0;
 
    PROCESS_BEGIN();

    sys_init_random_node_values();
    init_com_channels();
    random_init(linkaddr_node_addr.u8[0] ^ (clock_time() & 0xFF));

    while(1)
    {
        if(SystemState == state_AUTODISCOVERY)
        {
            Log_print_state();
            clock_time_t t = generate_random_time();
            etimer_set(&timer, t);
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
            message_discovery_searching();

            if(message_search_for_type_UC(msgType_UC_JOIN_EXISTING_NETWORK)!=255)
            {
                /*invite from an existing network have been sent*/
                /*TO DO*/

            }

            if(message_search_for_type_BC(msgType_BC_AUTODISCOVERY_START) == 1)
            {
                /*try if another nodes are searching too for a network*/
                counter_t = 3;
                counter = 0;
                protocol_reset_stack_BC();
                SystemState = state_LISTEN;
            }

        }

        /************************************************************************************************************ */

        if(SystemState == state_LISTEN)
        {   

            Log_print_state();

            if(message_search_for_type_UC(msgType_UC_JOIN_EXISTING_NETWORK)!=255)
            {
                /*If in the meantime an existing network sent an inviation, accept it and
                abort the new netowrk thing*/
            }

            if(message_process_all_BC(msgType_BC_AUTODISCOVERY_START) == 1)
            {
                printf("All recevied Message processed in LISTEN state, add counter\n");
                counter_t++;
            }

            uint8_t n = message_return_index_nr_nodes();
            clock_time_t t = 20 + (clock_time_t)20*n + generate_random_time();
            etimer_set(&timer, t);
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));               
            
            message_discovery_searching();
            counter++;

            if(counter == counter_t)
            {
                counter = 0;
                counter_t = 0;
                heapify_routing_table_all();
                //log_print_nods_id();
                log_print_routing_table();
                SystemState = state_START_HEART_BEAT;
                continue;
                //PROCESS_YIELD();
            }

        }

        if(SystemState == state_START_HEART_BEAT)
        {
            Log_print_state();
            sys_start_heartbeat();
            sys_heart_beat_handler_tx_start();
            PROCESS_YIELD();
        }

        PROCESS_PAUSE();

    }

    PROCESS_END();
}

PROCESS_THREAD(process_heartbeat, ev, data)
{
    PROCESS_BEGIN();

    while(1)
    {
        /*we dont do it here busy waiting, nah nah*/

        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);

        if(SystemState >= state_START_HEART_BEAT)
        {
            handle_heart_beat_send();
        }        
    }

    PROCESS_END();
}

