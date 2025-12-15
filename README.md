# Project documentation

### Folder structure

```text
FIRMWARE-CZC
├── build                   # Build output (bootloader, app binaries, partition table)
│   ├── bootloader          # Bootloader build artifacts
│   ├── firmware-czc.elf    # Compiled application ELF
│   ├── partition_table     # Partition table binary
│   └── esp-idf             # Compiled ESP-IDF components
├── documentation           # images and source files for the documentation 
├── main                    # Application source code
│   ├── communication       # REST API, HTTP server handlers
│   ├── network             # Network state machine, Wi-Fi/Ethernet logic
│   ├── esp                 # ESP-specific utilities
│   └── zigBeeChip          # ZigBee integration
├── websource               # html, js and css files for the webinterface  
├── components              # Custom reusable modules (if present)
├── sdkconfig               # Project configuration
├── partitions_4mb.csv      # custom partition table 
└── CMakeLists.txt          # Build configuration
```

### UML Component Diagram
Soon :)

### UML Conceptual Class Diagram
This diagram depicts the basic class structure of the software project with a brief description of each class.
![UML Conceptual Class Diagram](/documentation/UML%20Conceptual%20Class%20Diagram.png)
All objects and tasks are created in the main.cpp file. The proxy functionality is implemented in the CcChipController.

### Network state machine
The network state machine is a special concept in this project. It provides the complete functionality of the network components during startup and runtime.
Behavior: Wi‑Fi is started when a configuration exists in NVS, Ethernet is always started.
After 5 seconds, if neither Ethernet nor Wi‑Fi is connected, the Access Point is started.
If Wi‑Fi loses connection, a retry timer is triggered. If the timer finishes without a re‑established connection, the system switches to Ethernet.
This mechanism is implemented for both directions: Ethernet → Wi‑Fi and Wi‑Fi → Ethernet.
When the Wi‑Fi configuration is changed, the Access Point, Wi‑Fi, and Ethernet are shut down, the new configuration is saved, and Wi‑Fi is started again.
Note: If you configure Wi‑Fi credentials via the Access Point and they are wrong, the system cannot connect.

Wireless and Ethernet each have their own small event handlers: eth_event_handler (Ethernet events), wifi_event_handler (Wi‑Fi events). In the state machine, a combined event handler called network_event_handler is defined. It controls the state machine and reacts to IP events as well as custom network events (defined in network_event.h). These custom events are used for special cases such as updated Wi‑Fi configuration, initialization timer timeout, and other project‑specific events.

### Non volatile storage
Configuration data in non‑volatile storage is organized into component‑specific configuration structs, which are defined in NvsAPI.h. At present, only complete configuration structures can be saved or loaded; partial updates are not supported.

### RestAPI
The web interface must communicate bidirectionally with the ESP32. For requests from the web interface to the ESP32, a REST API is provided. For communication in the opposite direction (ESP32 → web interface), Server‑Sent Events (SSE) are used. In this model, the browser’s JavaScript opens an HTTP connection to a REST endpoint, and the connection remains open throughout runtime.

Events in the stream follow the SSE format:

    event: 'event' \n
    data: 'data' \n\n

This mechanism is implemented using a global event queue. Any component can publish data to this queue, while a dedicated task continuously processes queued events and pushes them to the frontend. Specific REST API functions are described in the API section, and the available SSE events are documented separately in the SSE events section.

#### API functions
This section documents all REST API endpoints that are exposed by the ESP32. Note: not every internal method in RestAPI.cpp is listed here — only the URIs that are accessible from the web interface.

| URI          | Method | Description                                                                 |
|--------------|--------|-----------------------------------------------------------------------------|
| `/`          | GET    | Serves the main index page (HTML dashboard).                                |
| `/scripts.js`| GET    | Provides JavaScript resources required by the web interface.                |
| `/style.css` | GET    | Provides CSS styles for the web interface.                                  |
| `/events`    | GET    | Opens a Server‑Sent Events (SSE) stream. Used for ESP32 → Web communication.|

Notes

    The index, scripts, and style URIs are static file endpoints served from the filesystem.

    The /events URI is special: it keeps the HTTP connection open and streams events in SSE format.

    SSE events are documented in the dedicated SSE Events section.

#### SSE events
Currently there are no implemented events.

### Cc chip controller
This class provides the high level API of the Texas instruments CC2652P4 used for the ZigBee / Thread communication. 
More detailed documentation coming soon. 
