#include "driver/uart.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <vector>

#include "esp_log.h"

#define TX_PIN 4
#define RX_PIN 36
#define RST_PIN 16
#define BSL_PIN 32

#define ACK_BYTE 0xCC
#define NACK_BYTE 0x33
#define COMMAND_RET_SUCCESS = 0x40

// NOTING TESTED ...

class CcFrameAPI
{
private:
    static const char* TAG;
    const uint16_t ccUartBufferSize;
    QueueHandle_t ccUartQueue;
    uart_port_t ccUartNum;
    bool inBootloaderMode;
    
    void initBslAndRst();
    bool setCcBootloaderMode();
    bool detectChipInfo();
    
    void setupCcChipBootloaderMode();
    void restartCc();
    void routerRejoin();
    bool sendSynch();
    void sendACK();
    void sendNACK();
    uint32_t cmdGetChipId();
    bool checkLastCmd();
    std::vector<uint8_t> cmdGetStatus();
    std::vector<uint8_t> receivePacket();
    char* getStatusString(uint8_t statusCode);
    bool waitForAck(uint16_t timeoutMs);

    //bool eraseFlash
    //bool cmdDownload
    //bool cmdSendData
    //bool ping
    //std::vector<uint8_t> cmdMemRead
    //void encodeAddress
    //unsigned long decodeAddress
    //uint8_t calcChecks
    //bool ledToggle
    //...

public:
    CcFrameAPI();
    ~CcFrameAPI();
    void initCcUart();
    void closeCcUart();
};