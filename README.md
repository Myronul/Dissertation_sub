Research Protocol Implementation:

*Development of the Internal Software Architecture*
1. Data structure of the nodes
2. Adding a new node to the network described through role-based priority queues (broadcast discovery message)
3. (Tx) Packet routing policy based on priority queues and roles
4. (Rx) Efficient data packet handling via ISR (thread polling semaphore & radio software FIFO queue)
5. Process design (critical + non-critical + role-specific)
6. Development of an efficient metric for routing packets between producer–processor–consumer roles (Python/MATLAB simulation for optimal weighting)

*Protocol Structure Development*
1. Application-level message data structure (header + payload)
2. Periodic heartbeat
3. Handling node removal from the network (no heartbeat)
4. Handling node reintegration after heartbeat loss
5. Migration of tasks and associated data of a node through the implementation of a global cache memory per role
6. Implementation of a global token for access to shared resources

*BONUS*

Development of a network sniffer to capture data packets exchanged between nodes and display the topology evolution in real time.
For this simulation, Cooja will not be used. Instead, each node will be compiled as an independent process, and nodes will communicate at the software radio level using Linux sockets/pipes.
A Contiki kernel image can be created using a native build (make TARGET=native), allowing kernel emulation in an x86 Linux environment. This enables direct interaction with each node through a common process.



1. Data structure of the nodes
Each node will use an efficient data structure for transmitting data. Essentially, any transmitted message will be composed of a specific structure: a header (containing identification elements and essential message characteristics) and a payload.

* 8 bits for the node ID
* 2 bits for the type code associated with the Tx MCU role
* 2 bits for the type code associated with the Rx MCU role
* 8 bits for the message type
* 8 bits for the message length (in bytes)
* 256 Bytes[] payload

typedef struct MSG
{
    uint8_t id;      
    uint8_t roles;   
    uint8_t msgType; 
    uint8_t msgLen;
    uint8_t payload[N];
}MSG;