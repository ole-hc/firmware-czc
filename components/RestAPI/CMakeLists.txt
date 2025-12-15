idf_component_register(
    SRCS "RestAPI.cpp"
    INCLUDE_DIRS "."
    REQUIRES 
        HttpServer
        Storage
        System
        Network
)
