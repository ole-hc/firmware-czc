#pragma once
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
#define COMMAND_RET_SUCCESS 0x40
#define CMD_FRAME_START 0xFE

struct ccInfo {
    uint32_t fwRevision;
    uint32_t flashSize;
    uint32_t ramSize;
    uint8_t maintrel;
    uint8_t minorrel;
    uint8_t majorrel;
    uint8_t product;
    uint8_t transportrev;
};

class CcFrameAPI
{
private:
    static const char* TAG;
    const uint16_t ccUartBufferSize;
    QueueHandle_t ccUartQueue;
    uart_port_t ccUartNum;
    bool inBootloaderMode;
        
    std::vector<uint8_t> receivePacket();
    char* getStatusString(uint8_t statusCode);
    bool waitForAck(uint16_t timeoutMs);
    void encodeAddress(uint32_t address, uint8_t encodedAddress[4]);
    uint8_t calculateChecksum(uint8_t cmd, uint32_t address, uint32_t size);
    bool checkLastCmd();
    void setupCcChipBootloaderMode();
    bool sendSynch();
    void sendACK();
    void sendNACK();

public:
    CcFrameAPI();
    ~CcFrameAPI();

    void initCcUart();
    void initBslAndRst();

    bool setCcBootloaderMode();
    bool detectChipInfo();

    void restartCc();
    void routerRejoin();

    bool cmdEraseFlash();
    bool cmdDownload(uint32_t address, uint32_t size);
    bool cmdSendData(std::vector<uint8_t> data);
    bool cmdSetLedState(bool ledState);
    uint32_t cmdGetChipId();
    std::vector<uint8_t> cmdMemRead(uint32_t address);
    std::vector<uint8_t> cmdGetStatus();
    bool cmdCheckFwVersion(ccInfo& chip);
};