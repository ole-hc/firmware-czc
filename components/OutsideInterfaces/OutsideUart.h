#pragma once
#include "driver/uart.h"

#include "esp_log.h"

class OutsideUart
{
private:
    static const char* TAG; 

    uart_port_t outsideUartNum;
public:
    OutsideUart();
    ~OutsideUart();

    void initOutsideUart();
    void closeOutsideUart();
    int readOutsideUart(uint8_t *buffer, uint8_t bufferLength);
    int writeOutsideUart(uint8_t* buffer, uint8_t bufferLength);
};
