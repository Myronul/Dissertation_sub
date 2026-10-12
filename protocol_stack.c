#include "protocol_stack.h"
#include"protocol_messages.h"
#include"routing.h"
#include "sys.h"

#define UNICAST_CHANNEL 129 /*unic radio channel active*/
#define BROADCAST_CHANNEL 146 
#define NR_DATA_BUFFER 8 /*number of messages that can be stored in the buffer for each type of com*/

#define BROADCAST_PUSH 1
#define UNICAST_PUSH 0 

extern uint8_t SystemState;
extern ROUTING_TABLE routingTable;

NODE node; /*defined as extern in the header*/
DATA dataRxBroadCast;
DATA dataRxUniCast;

static DATA dataQeueBc[NR_DATA_BUFFER];
static DATA dataQeueUc[NR_DATA_BUFFER];

static uint8_t bitMapBc = 0x00;
static uint8_t bitMapUc = 0x00;

static uint8_t flagUniCastRx = 0;
static uint8_t flagBroadCastRx = 0;

static struct unicast_conn uc;    /*rime structure that handle the radio unicast com*/
static struct broadcast_conn bc;  /*rime structure that handle the radio broadcast com*/


static uint8_t filter_routing_table(DATA dataRx)
{
    /*Funciton that will return true if the node is already in the routing table*/
    uint8_t i = 0;

    switch (dataRx.type)
    {
        case PROCESSER:
            for(i=0; i < routingTable.indexProcessers; i++)
            {
                if(routingTable.routingTableProcessers[i].id == dataRx.nodeInfo.id)
                {
                    printf("[FILTER]Duplicate Processer already in routing table filtered at ISR for BR from nod: %i\n", dataRx.nodeInfo.id);
                    return 1;
                }
            }    
        break;

        case PRODUCER:
            for(i=0; i < routingTable.indexProducers; i++)
            {
                if(routingTable.routingTableProducers[i].id == dataRx.nodeInfo.id)
                {
                    printf("[FILTER]Duplicate Producer already in routing table filtered at ISR for BR from nod: %i\n", dataRx.nodeInfo.id);
                    return 1;
                }
            }
        break;

        case CONSUMER:
            for(i=0; i < routingTable.indexConsumers; i++)
            {
                if(routingTable.routingTableConsumers[i].id == dataRx.nodeInfo.id)
                {
                    printf("[FILTER]Duplicate Consumer already in routing table filtered at ISR for BR from nod: %i\n", dataRx.nodeInfo.id);
                    return 1;
                }
            }
        break;
        
        default:
            return 1;
            break;
        }
    
    return 0;

}


static void push_data_com_stack(DATA dataRx, DATA* buffer, uint8_t* map, uint8_t flagMap)
{
    
    /*filter by type*/
    switch(SystemState)
    {
        case state_AUTODISCOVERY:
        if((dataRx.msgType != msgType_BC_AUTODISCOVERY_START) && (dataRx.msgType != msgType_UC_JOIN_EXISTING_NETWORK))
        {
            printf("[FILTER]Message filtered in state AUTODISCOVERY\n");
            return; /*for this state we will accept only a certain types of msges to not fill fast the buffer*/
        }
        break;

        case state_LISTEN:
        if((dataRx.msgType != msgType_BC_AUTODISCOVERY_START) && (dataRx.msgType != msgType_UC_JOIN_EXISTING_NETWORK))
        {
            printf("[FILTER]Message filtered in state LISTEN\n");
            return; /*for this state we will accept only a certain types of msges to not fill fast the buffer*/
        }
        break;

        case state_IDLE:
            printf("[FILTER]Message filtered in state ELECT_LIDER\n");
            return;
        break;
        
        default:
        break;  
    }

    /*search if broadcast for duplicates*/

    uint8_t index = 0;

    if(flagMap == 1) /*case for broadcast storming*/
    {
        /*check if it is already in the stack buffer waiting*/
        while(index < NR_DATA_BUFFER)
        {
            if((buffer[index].nodeInfo.id == dataRx.nodeInfo.id && buffer[index].msgType == dataRx.msgType) && ((*map) & (1<<index)))
            {
                printf("[INFO]Duplicate already in buffer filtered at ISR for BR from nod: %i\n", dataRx.nodeInfo.id);
                return;
            }

            index++;
        }
        
        
        if(filter_routing_table(dataRx)) /*check if it is already in the routing table*/
        {
            return;
        }

    }

    /*add to the qeue bitMap implementation and message filter*/

    index = 0;

    while(index < NR_DATA_BUFFER)
    {
        if(!((*map) & (1<<index)))
        {
            buffer[index] = dataRx;
            *map = (*map) | (1<<index);
            printf("[INFO]Message stored in the buffer at index %u\n", index);
            return;
        }

        index++;
    }

    printf("[INFO]Qeue BC is full!\n");

}

void protocol_reset_stack_BC()
{
    bitMapBc = 0;
}

void protocol_reset_stack_UC()
{
    bitMapUc = 0;
}

uint8_t pop_data_comBc_stack(uint8_t msgType, DATA* dataFound)
{
    /*Function to search through all messages received in the buffer
    In base of the message ID it will pop the message fom the current index
    and will reset the bitMap. 
    flagMap = 1 -> broadcast
    flagMap = 0 -> unicast*/

    /*bitMap implementation*/
    uint8_t index = 0;

    while(index < NR_DATA_BUFFER)
    {
        if(((bitMapBc) & (1<<index)) && (dataQeueBc[index].msgType == msgType))
        {
            bitMapBc = (bitMapBc) & ~(1<<index); /*reset index*/
            *dataFound = dataQeueBc[index];
            return 1;
        }

        index++;
    }   

    return 0;

}


uint8_t pop_data_comUc_stack(uint8_t msgType, DATA* dataFound)
{
    /*Function to search through all messages received in the buffer
    In base of the message ID it will pop the message fom the current index
    and will reset the bitMap. 
    flagMap = 1 -> broadcast
    flagMap = 0 -> unicast*/

    /*bitMap implementation*/
    uint8_t index = 0;

    while(index < NR_DATA_BUFFER)
    {
        if(((bitMapUc) & (1<<index)) && (dataQeueUc[index].msgType == msgType))
        {
            bitMapUc = (bitMapUc) & ~(1<<index);
            *dataFound = dataQeueUc[index];
            return 1;
        }

        index++;
    }   

    return 0;

}


static void __unicast_ISR__(struct unicast_conn *c, const linkaddr_t *from) 
{
    /*ISR for the unicast receiving data*/
    flagUniCastRx = 1;
    memcpy(&dataRxUniCast, packetbuf_dataptr(), sizeof(DATA));
    printf("[RX-UC]Node %u received from Node %u\n", 
            node.id, from->u8[0]);
    push_data_com_stack(dataRxUniCast, dataQeueUc, &bitMapUc, UNICAST_PUSH);

}


static void __broadcast_ISR__(struct broadcast_conn *c, const linkaddr_t *from) 
{
    /*ISR for the broadcast receiving data*/
    if(packetbuf_datalen() != sizeof(DATA)) 
    {
        printf("[ERR]Received invalid size\n");
        return;
    }

    flagBroadCastRx = 1;
    memcpy(&dataRxBroadCast, packetbuf_dataptr(), sizeof(DATA));
    printf("[RX-BC]Node %u received from Node %u\n", 
            node.id, from->u8[0]);
    push_data_com_stack(dataRxBroadCast, dataQeueBc, &bitMapBc, BROADCAST_PUSH);
}

static const struct unicast_callbacks unicastCallback = {__unicast_ISR__};
static const struct broadcast_callbacks broadcastCallback = {__broadcast_ISR__};


void init_com_channels(void)
{
    node.id = linkaddr_node_addr.u8[0];
    printf("[INIT]Initializing communication channels\n");
    unicast_open(&uc, UNICAST_CHANNEL, &unicastCallback);
    broadcast_open(&bc, BROADCAST_CHANNEL, &broadcastCallback);
}


void send_message_unicast(linkaddr_t *dest, DATA *data) 
{
    packetbuf_copyfrom(data, sizeof(DATA)); /*copy data in the stack radio*/
    unicast_send(&uc, dest);
    printf("[TX] Sent packet to Node %u\n", dest->u8[0]);
}


void send_message_broadcast(DATA *data)
{
    packetbuf_copyfrom(data, sizeof(DATA));
    broadcast_send(&bc);
    printf("[TX] Sent packet Broadcast\n");
}




linkaddr_t protocol_get_min_target_metric(nodeType targetType) 
{
    linkaddr_t addr;
    addr.u8[0] = 0; /*default values*/
    addr.u8[1] = 0;
    
    return addr;
}