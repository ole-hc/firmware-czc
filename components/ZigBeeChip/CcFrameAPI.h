#include "driver/uart.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "esp_log.h"

#define TX_PIN 4
#define RX_PIN 36
#define RST_PIN 16
#define BSL_PIN 32

#define ACK_BYTE 0xCC
#define NACK_BYTE 0x33

class CcFrameAPI
{
private:
    static const char* TAG;
    const uint16_t ccUartBufferSize;
    QueueHandle_t ccUartQueue;
    uart_port_t ccUartNum;
    
    void initBslAndRst();
    void setupCcChip();
    bool sendSynch();
    bool waitForAck(uint16_t timeoutMs);
public:
    CcFrameAPI();
    ~CcFrameAPI();
    void initCcUart();
    void closeCcUart();
};