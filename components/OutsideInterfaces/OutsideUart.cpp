#include "OutsideUart.h"

const char* OutsideUart::TAG = "OutsideUart";

OutsideUart::OutsideUart()
    : outsideUartNum(UART_NUM_0)
{
}

OutsideUart::~OutsideUart()
{
}

// Uninit ESP_LOG and init uart for proxy
void OutsideUart::initOutsideUart()
{
}

void OutsideUart::closeOutsideUart()
{
}

int OutsideUart::readOutsideUart(uint8_t *buffer, uint8_t bufferLength)
{
    int readLength = uart_read_bytes(outsideUartNum, buffer, bufferLength, 10);
    return readLength;
}

int OutsideUart::writeOutsideUart(uint8_t *buffer, uint8_t bufferLength)
{
    int writtenLength = uart_write_bytes(outsideUartNum, buffer, bufferLength);
    return writtenLength;
}
