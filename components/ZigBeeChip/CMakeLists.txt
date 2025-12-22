idf_component_register(
    SRCS 
        "CcZnpAPI.cpp" 
        "CcFrameAPI.cpp" 
        "CcChipController.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_uart
    REQUIRES esp_driver_gpio
)
