idf_component_register(
    SRCS 
        "IoAPI.cpp" 
        "SystemManager.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_gpio
)
