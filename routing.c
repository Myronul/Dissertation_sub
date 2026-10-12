#include "routing.h"
#include "protocol_stack.h"

extern ROUTING_TABLE routingTable;
extern uint8_t SystemState;

static void heapify_min_array_nodes(NODE *nodesArray, uint8_t index, uint8_t size)
{
    uint16_t leftChild = 2*index + 1;
    uint16_t rightChild = 2*index + 2;
    uint16_t smallest = index;

    if((leftChild < size) && (nodesArray[leftChild].metric < nodesArray[smallest].metric))
    {
        smallest = leftChild;
    }

    if((rightChild < size) && (nodesArray[rightChild].metric < nodesArray[smallest].metric))
    {
        smallest = rightChild;
    }

    if(smallest != index)
    {
        NODE temp = nodesArray[index];
        nodesArray[index] = nodesArray[smallest];
        nodesArray[smallest] = temp;
        heapify_min_array_nodes(nodesArray, smallest, size);
    }
}

void heapify_routing_table_all(void)
{
    heapify_routing_table_processers();
    heapify_routing_table_producers();
    heapify_routing_table_consumers();
}

inline void heapify_routing_table_processers(void)
{
    int16_t i;
    for(i = ((int16_t)routingTable.indexProcessers/2)-1; i >= 0; i--)
    {
        heapify_min_array_nodes(
            routingTable.routingTableProcessers,
            i,
            routingTable.indexProcessers);
    }   
}

inline void heapify_routing_table_producers(void)
{
    int16_t i;
    for(i = ((int16_t)routingTable.indexProducers/2)-1; i >= 0; i--)
    {
        heapify_min_array_nodes(
            routingTable.routingTableProducers,
            i,
            routingTable.indexProducers);
    }   
}

inline void heapify_routing_table_consumers(void)
{
    int16_t i;
    for(i = ((int16_t)routingTable.indexConsumers/2)-1; i >= 0; i--)
    {
        heapify_min_array_nodes(
            routingTable.routingTableConsumers,
            i,
            routingTable.indexConsumers);
    }

}