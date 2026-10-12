#include"protocol_messages.h"
#include"sys.h"
#include"routing.h"

extern nodeType hostNodeType;
ROUTING_TABLE routingTable = {0}; /*store current ids network in listening mode*/


void message_send_broadcast(const char* payload, uint8_t payloadLen, uint8_t msgType)
{
    linkaddr_copy((unsigned char*)&dataTxBroadCast.nodeInfo.id, &linkaddr_node_addr); 
    dataTxBroadCast.type = hostNodeType; /*set the type of the node*/
    dataTxBroadCast.nodeInfo.metric = node.metric; /*set the metric of the node*/
    dataTxBroadCast.nodeInfo.timerHeartBeat = node.timerHeartBeat; /*set the heartbeat timer of the node*/
    dataTxBroadCast.msgType = msgType; /*discovery message*/
    dataTxBroadCast.msgLen = payloadLen;
    memcpy(dataTxBroadCast.payload, payload, payloadLen);
    send_message_broadcast(&dataTxBroadCast);
}

void message_discovery_searching()
{
    message_send_broadcast(payload_msgType_BC_AUTODISCOVERY_START, 
                           sizeof(payload_msgType_BC_AUTODISCOVERY_START), 
                           msgType_BC_AUTODISCOVERY_START);
}

void message_clear_buffer_id(void)
{
    printf("[INFO]Clearing buffer of discovered nodes\n");
    memset(&routingTable, 0, sizeof(ROUTING_TABLE));
}


uint8_t message_return_index_nr_nodes(nodeType type)
{
    switch(type)
    {
        case PROCESSER:
            return routingTable.indexProcessers;
        break;
        
        case PRODUCER:
            return routingTable.indexProducers;
        break;

        case CONSUMER:
            return routingTable.indexConsumers;
        break;

        default:
            return 0;
        break;
    }
}

uint8_t message_search_for_type_BC(uint8_t type)
{
    /*Function that will search thourgh all the stack received data and will store the first
    node id in the NodeID buffer*/

    DATA tempData;

    if(pop_data_comBc_stack(type, &tempData))
    {
        /*case for new message*/
        switch (tempData.type)
        {
            case PROCESSER:
                if(routingTable.indexProcessers < MAX_NDR_NODES)
                {
                    routingTable.routingTableProcessers[routingTable.indexProcessers++] = tempData.nodeInfo;
                    printf("[INFO]New Processer discovered with id %u\n", tempData.nodeInfo.id);
                }
                break;

            case PRODUCER:
                if(routingTable.indexProducers < MAX_NDR_NODES)
                {
                    routingTable.routingTableProducers[routingTable.indexProducers++] = tempData.nodeInfo;
                    printf("[INFO]New Producer discovered with id %u\n", tempData.nodeInfo.id);
                }
                break;

            case CONSUMER:
                if(routingTable.indexConsumers < MAX_NDR_NODES)
                {
                    routingTable.routingTableConsumers[routingTable.indexConsumers++] = tempData.nodeInfo;
                    printf("[INFO]New Consumer discovered with id %u\n", tempData.nodeInfo.id);
                }
                break;

            default:
                break;
        }

        return 1; /*true*/
    }

    printf("[INFO]No message of type %u found in BC stack\n", type);
    return 0; /*false*/
}


uint8_t message_process_all_BC(uint8_t type)
{
    /*Funciton that will process all the current data in the radio stack
    by saving the nodeID's into a buffer*/

    volatile uint8_t flag = 0;

    while(message_search_for_type_BC(type))
    {
        flag = 1; /*at least one new element*/
    }
    
    return flag; /*return 1 if at least one element is new and 0 for none or just duplicates*/
}


void log_print_nods_id()
{
    uint8_t i = 0;
    printf("[INFO]<<NOD %i>> discovered nodes: ", node.id);
    printf("\n");

    printf("[INFO]Processers: ");
    for(i=0; i<routingTable.indexProcessers; i++)
    {
        printf("%u ", routingTable.routingTableProcessers[i].id);
    }
    printf(" Total: %i\n", routingTable.indexProcessers);

    printf("[INFO]Producers: ");
    for(i=0; i<routingTable.indexProducers; i++)
    {
        printf("%u ", routingTable.routingTableProducers[i].id);
    }
    printf(" Total: %i\n", routingTable.indexProducers);

    printf("[INFO]Consumers: ");
    for(i=0; i<routingTable.indexConsumers; i++)
    {
        printf("%u ", routingTable.routingTableConsumers[i].id);
    }
    printf(" Total: %i\n", routingTable.indexConsumers);
}

void log_print_routing_table(void)
{
    uint8_t i = 0;
    printf("[INFO]<<NOD %i>> routing table: ", node.id);
    printf("\n");

    printf("[INFO]Processers: ");
    for(i=0; i<routingTable.indexProcessers; i++)
    {
        printf("%u ", routingTable.routingTableProcessers[i].metric);
    }
    printf(" Total: %i\n", routingTable.indexProcessers);

    printf("[INFO]Producers: ");
    for(i=0; i<routingTable.indexProducers; i++)
    {
        printf("%u ", routingTable.routingTableProducers[i].metric);
    }
    printf(" Total: %i\n", routingTable.indexProducers);

    printf("[INFO]Consumers: ");
    for(i=0; i<routingTable.indexConsumers; i++)
    {
        printf("%u ", routingTable.routingTableConsumers[i].metric);
    }
    printf(" Total: %i\n", routingTable.indexConsumers);
}


uint8_t message_search_for_type_UC(uint8_t type)
{
    DATA tempData;

    if(pop_data_comUc_stack(type, &tempData))
    {
        printf("[INFO]Unicast message received from node with id %u\n", tempData.nodeInfo.id);
        return tempData.nodeInfo.id; /*return the id of the unicast node message*/
    }

    return 255;
}
