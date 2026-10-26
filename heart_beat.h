#ifndef HEART_BEAT
#define HEART_BEAT
#include "util.h"
#include "sys.h"

void sys_start_heartbeat(void);
void handle_heart_beat_send(void);
void inc_timer_heart_beat_tick_routing_table(void);
void check_heart_beat_tick_routing_table(void);
void reset_heart_beat_tick_routing_table_node(DATA nodeData);
bool check_heart_beat_tick_all_routing_table(void);
#endif