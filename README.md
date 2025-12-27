# Table of Contents

- [Project documentation](#project-documentation)
  - [Folder structure](#folder-structure)
  - [UML Conceptual Class Diagram](#uml-conceptual-class-diagram)
  - [Components](#components)
    - [Network](#network)
    - [Storage](#storage)
    - [HttpServer](#httpserver)
    - [RestAPI](#restapi)
      - [API functions](#api-functions)
      - [SSE events / EventQueue](#sse-events--eventqueue)
    - [ZigBeeProxy](#zigbeeproxy)
    - [OutsideInterfaces](#outsideinterfaces)
    - [System](#system)

# Project documentation

## Folder structure

```text
FIRMWARE-CZC
├── build                   # Build output (bootloader, app binaries, partition table)
│   ├── bootloader          # Bootloader build artifacts
│   ├── firmware-czc.elf    # Compiled application ELF
│   ├── partition_table     # Partition table binary
│   └── esp-idf             # Compiled ESP-IDF components
├── components              
│   ├── HttpServer          # Http server for WebInterface
│   ├── MqttTelemetry       # Mqtt server for publishing runtime data
│   ├── Network             # Ethernet, Wifi and AccessPoint state machine 
│   ├── OutsideInterfaces   # Proxy outside communication 
│   ├── RestAPI             # Http server backend functions 
│   ├── Storage             # Filesystem and Nvs
│   ├── System              # Runtime data and IO
│   ├── ZigBeeProxy         # Proxy functionality and CC communication
├── documentation           # images and source files for the documentation 
├── main                    # Application source code
├── websource               # html, js and css files for the webinterface  
├── sdkconfig               # Project configuration
├── partitions_4mb.csv      # custom partition table 
└── CMakeLists.txt          # Build configuration
```

## UML Conceptual Class Diagram
This diagram depicts the basic class structure of the software project with a brief description of each class.
![UML Conceptual Class Diagram](/documentation/UML%20Conceptual%20Class%20Diagram.png)
All objects and tasks are created in the main.cpp file.

## Components

### Network
The network module consists of the network state machine which is a special concept in this project. It provides the complete functionality of the network components during startup and runtime.
Behavior: Wi‑Fi is started when a configuration exists in NVS, Ethernet is always started.
After 5 seconds, if neither Ethernet nor Wi‑Fi is connected, the Access Point is started.
If Wi‑Fi loses connection, a retry timer is triggered. If the timer finishes without a re‑established connection, the system switches to Ethernet.
This mechanism is implemented for both directions: Ethernet → Wi‑Fi and Wi‑Fi → Ethernet.
When the Wi‑Fi configuration is changed, the Access Point, Wi‑Fi, and Ethernet are shut down, the new configuration is saved, and Wi‑Fi is started again.
Note: If you configure Wi‑Fi credentials via the Access Point and they are wrong, the system cannot connect.

Wireless and Ethernet each have their own small event handlers: eth_event_handler (Ethernet events), wifi_event_handler (Wi‑Fi events). In the state machine, a combined event handler called network_event_handler is defined. It controls the state machine and reacts to IP events as well as custom network events (defined in network_event.h). These custom events are used for special cases such as updated Wi‑Fi configuration, initialization timer timeout, and other project‑specific events.

### Storage
The storage system is divided into non‑volatile storage (NVS) and a volatile filesystem.
The filesystem is used for temporary or large data, such as storing the CC firmware file before an update or other transient files.
NVS, on the other hand, serves a completely different purpose. It's main task is to save Configuration data between restarts of the ESP. This config data stored in NVS is organized into component‑specific configuration structures, which are defined in NvsAPI.h. Currently, only entire configuration structures can be saved or loaded; partial updates are not supported.

### HttpServer
This component only implements the basic HttpServer functionality and its configuration. All URIs and similar are implemented in the RestAPI component.

### RestAPI
The web interface must communicate bidirectionally with the ESP32. For requests from the web interface to the ESP32, a REST API is provided. For communication in the opposite direction (ESP32 → web interface), Server‑Sent Events (SSE) are used. In this model, the browser’s JavaScript opens an HTTP connection to a REST endpoint, and the connection remains open throughout runtime. The implemented API and Http server support multiple Clients at the same time.

Events in the stream follow the SSE format:

    event: 'event' \n
    data: 'data' \n\n

This mechanism is implemented using a global event queue. Any component can publish data to this queue, while a dedicated task continuously processes queued events and pushes them to the frontend. Specific REST API functions are described in the API section, and the available SSE events are documented separately in the SSE events section.

#### API functions
This section documents all REST API endpoints that are exposed by the ESP32. Note: not every internal method in RestAPI.cpp is listed here — only the URIs that are accessible from the web interface.

| URI              | Method | Description                                                                 |
|------------------|--------|-----------------------------------------------------------------------------|
| `/`              | GET    | Serves the main index page (HTML dashboard).                                |
| `/scripts.js`    | GET    | Provides JavaScript resources required by the web interface.                |
| `/style.css`     | GET    | Provides CSS styles for the web interface.                                  |
| `/events`        | GET    | Opens a Server‑Sent Events (SSE) stream. Used for ESP32 → Web communication.|
| `/api/led/on`    | POST   | Turns on mode LED on the circuit board.                                     |
| `/api/led/off`   | POST   | Turns off mode LED on the circuit board.                                    |

Notes
> The index, scripts, and style URIs are static file endpoints served from the filesystem.
<br> The /events URI is special: it keeps the HTTP connection open and streams events in SSE format.
<br>SSE events are documented in the dedicated SSE Events section.

#### SSE events / EventQueue
Server‑sent events are handled through the EventQueue class. This class provides a thread‑safe queue that collects events generated during the updateFrontendTask. Each event is stored internally as an SseEvent structure.
Other components can also push events into the queue, as long as they have access to the already‑initialized queue instance.
During each cycle of updateFrontendTask, the system first gathers all relevant data from the ESP. After polling is complete, the task sends all queued events to every connected frontend client.
The following section documents all event types currently supported by the frontend.

### ZigBeeProxy
The CcChipController class manages high‑level communication with the CC chip and implements the proxy using a state machine with four states: COORDINATOR, ROUTER, THREAD, and CONTROL.
The proxy tasks run continuously, but their behavior is determined by the active mode. In Coordinator and Thread mode, the proxy operates normally. In Router mode, the proxy is disabled, but the CC remains outside of bootloader mode. In Control mode, the system can enter bootloader mode whenever required—for example, during firmware updates.

Two dedicated tasks handle the standard proxy operation:

>outsideToCcTask forwards data from the external interface to the CC.

> ccToOutsideTask forwards data from the CC back to the external interface.

When the system enters Control mode, both tasks remain idle, and all control operations are executed directly by other components such as the web interface or the IO interface. This approach avoids the need for an additional task while ensuring safe and predictable transitions between operational modes.

The CcFrameAPI class implements the low‑level CC communication commands, including UART initialization and flash‑related functionality.


### OutsideInterfaces
This component manages the different proxy output modes (UART and Network). It handles switching between these modes and ensures that the proxy always reads from and writes to the correct external interface. Because the proxy uses the same UART port as the system’s debug output (UART_NUM_0), debug logging is automatically disabled whenever the proxy operates in USB mode. The external interfaces are implemented in the two classes: 'OutsideUart' and 'OutsideSocket'. Default setting for the Proxy is NETWORK mode. All mode transitions are handled safely with the 'setOutsideInterface' ensuring that UART, logging, and proxy behavior remain consistent.

### System
The System component is responsible for monitoring runtime data and controlling direct peripherals such as the Mode LED, Power LED, and the onboard button. The Mode and Power LEDs indicate the current status of both the ESP and the CC. The button is intended to support multiple functions depending on how long it is pressed, although these features have not yet been implemented.