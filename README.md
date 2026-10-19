## Building

This project must be built within the Contiki environment. It is intended to be used as a subrepo inside the `examples/` folder of the official [Contiki OS repository](https://github.com/contiki-os/contiki). Alternatively, it can be built standalone by including the Contiki kernel and its dependency files.

Two targets are supported:

| Target | Command | Output | Typical use |
|--------|---------|--------|-------------|
| Native | `make TARGET=native` | x86 executable (`research_protocol.native`) | Debugging protocol logic and studying the memory layout of data structures and messages |
| Sky | `make TARGET=sky` | MSP430 firmware image | Emulation in COOJA: log analysis, packet tracing between nodes, power consumption measurements, and protocol evolution studies |

## Debugging

Besides emulating the MCU firmware in COOJA with the Sky target, the native build can be debugged with GDB. This is especially useful for profiling and for catching logic bugs and memory inconsistencies.

```bash
gdb ./research_protocol.native
```

Alternatively, use the provided VS Code debug configuration.

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