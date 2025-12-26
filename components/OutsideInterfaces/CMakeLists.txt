idf_component_register(
    SRCS 
        "OutsideInterface.cpp"
        "OutsideSocket.cpp"
        "OutsideUart.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_uart
)
