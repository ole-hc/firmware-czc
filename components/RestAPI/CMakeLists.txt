idf_component_register(
    SRCS "EventQueue.cpp" "RestAPI.cpp"
    INCLUDE_DIRS "."
    REQUIRES 
        HttpServer
        Storage
        System
        Network
)
