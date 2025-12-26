idf_component_register(
    SRCS 
        
        "CcFrameAPI.cpp" 
        "CcChipController.cpp"
    INCLUDE_DIRS "."
    REQUIRES OutsideInterfaces
    REQUIRES esp_driver_uart
    REQUIRES esp_driver_gpio
)
