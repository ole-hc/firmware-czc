#include "CcFrameAPI.h"

const char* CcFrameAPI::TAG = "CC-lowlevel-API";

CcFrameAPI::CcFrameAPI()
    :ccUartBufferSize(1024 * 2), ccUartQueue(NULL), ccUartNum(UART_NUM_1), inBootloaderMode(false)
{
}

CcFrameAPI::~CcFrameAPI()
{
}

// --- init --- 
void CcFrameAPI::initCcUart()
{
    ESP_ERROR_CHECK(uart_driver_install(ccUartNum, ccUartBufferSize, ccUartBufferSize, 10, &ccUartQueue, 0));
    uart_config_t ccUartConfig = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_param_config(ccUartNum, &ccUartConfig));
    ESP_ERROR_CHECK(uart_set_pin(ccUartNum, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    initBslAndRst();

    if(setCcBootloaderMode() == false) {
        ESP_LOGW(TAG, "No begin answer from the CC chip!");
        return;
    }
    restartCc();

    ESP_LOGI(TAG, "CC Uart communication setup successfully, rebooting CC");
}

void CcFrameAPI::closeCcUart()
{
}

void CcFrameAPI::initBslAndRst()
{
    gpio_config_t bslConfig = {
        .pin_bit_mask = (1ULL << BSL_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&bslConfig);

    gpio_config_t rstConfig = {
        .pin_bit_mask = (1ULL << RST_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&rstConfig);

    // Both are pulled up and low active 
    gpio_set_level((gpio_num_t)BSL_PIN, 1);
    gpio_set_level((gpio_num_t)RST_PIN, 1);
}

// --- Highlevel ---
bool CcFrameAPI::setCcBootloaderMode()
{
    if(inBootloaderMode == false) {
        setupCcChipBootloaderMode();
        if(sendSynch() == false) {
            ESP_LOGD(TAG, "Error setting up Bootloader mode");
            return false;
        }
    }
    return true;
}

bool CcFrameAPI::detectChipInfo()
{
    if(setCcBootloaderMode() == false) {
        ESP_LOGW(TAG, "Error setting up bootloader mode in detectChipInfo");
        return false;
    }

    return true;
}

// --- Lowlevel --- 
void CcFrameAPI::setupCcChipBootloaderMode()
{
    gpio_set_level((gpio_num_t)BSL_PIN, 0);
    restartCc();
    gpio_set_level((gpio_num_t)BSL_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(500));
    inBootloaderMode = true;
}

void CcFrameAPI::restartCc()
{
    gpio_set_level((gpio_num_t)RST_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(50));
    gpio_set_level((gpio_num_t)RST_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(500));
    inBootloaderMode = false;
}

void CcFrameAPI::routerRejoin()
{
    ESP_LOGI(TAG, "Router rejoin...");
    gpio_set_level((gpio_num_t)BSL_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(250));
    gpio_set_level((gpio_num_t)BSL_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(500));
}

bool CcFrameAPI::sendSynch()
{
    const uint8_t cmd = 0x55;
    
    ESP_ERROR_CHECK(uart_flush(ccUartNum));
    
    uart_write_bytes(ccUartNum, (const char*)&cmd, sizeof(cmd));
    uart_write_bytes(ccUartNum, (const char*)&cmd, sizeof(cmd));
    
    return waitForAck(2000);
}

void CcFrameAPI::sendACK()
{
    const uint8_t cmd1 = 0x00;
    const uint8_t cmd2 = 0xCC;
    uart_write_bytes(ccUartNum, (const char*)&cmd1, sizeof(cmd1));
    uart_write_bytes(ccUartNum, (const char*)&cmd2, sizeof(cmd2));
}

void CcFrameAPI::sendNACK()
{
    const uint8_t cmd1 = 0x00;
    const uint8_t cmd2 = 0x33;
    uart_write_bytes(ccUartNum, (const char*)&cmd1, sizeof(cmd1));
    uart_write_bytes(ccUartNum, (const char*)&cmd2, sizeof(cmd2));
}

uint32_t CcFrameAPI::cmdGetChipId()
{
    const uint8_t cmd = 0x28;
    const uint8_t length = 3; 

    uart_write_bytes(ccUartNum, (const char*)&length, sizeof(length));
    uart_write_bytes(ccUartNum, (const char*)&cmd, sizeof(cmd));
    uart_write_bytes(ccUartNum, (const char*)&cmd, sizeof(cmd));

    if(waitForAck(50)) {
        std::vector<uint8_t> version = receivePacket();
        if(version.size() != 4 ) {
            ESP_LOGW(TAG, "Unreasonable read size!");
            return uint32_t(0);
        }

        if(checkLastCmd()) {
            ESP_LOGD(TAG, "CmdGetChipId successful, read version from cc");
            uint32_t value = 0;
            value |= uint32_t(version.at(3)) << 0;
            value |= uint32_t(version.at(2)) << 8;
            value |= uint32_t(version.at(1)) << 16;
            value |= uint32_t(version.at(0)) << 24;

            uint32_t chipId = (version.at(0) << 8 | (version.at(1)));
            return chipId;
        }
    }
    return uint32_t(0);
}

bool CcFrameAPI::checkLastCmd()
{
    std::vector<uint8_t> status = cmdGetStatus();
    if(status == {}) {
        ESP_LOGW(TAG, "No response from target on status request, did you disable the bootloader?");
        return false;
    }

    uint8_t cmdReturn = status.at(0);
    if(cmdReturn == COMMAND_RET_SUCCESS) {
        return true;
    }
    
    char* statusString = getStatusString(cmdReturn);
    if(statusString == "Unknown") {
        ESP_LOGW(TAG, "Unrecognized status returned: 0x%d", cmdReturn);
    }
    else {
        ESP_LOGW(TAG, "Target returned: 0x%d, %s", cmdReturn, statusString);
    }
    return false;
}

std::vector<uint8_t> CcFrameAPI::cmdGetStatus()
{
    const uint8_t cmd = 0x23;
    const uint8_t length = 3; 

    uart_write_bytes(ccUartNum, (const char*)&length, sizeof(length));
    uart_write_bytes(ccUartNum, (const char*)&cmd, sizeof(cmd));
    uart_write_bytes(ccUartNum, (const char*)&cmd, sizeof(cmd));

    if(waitForAck(50)) {
        ESP_LOGD(TAG, "Got cmdGetStatus ACK");
        std::vector<uint8_t> status = receivePacket();
        return status;
    }

    ESP_LOGW(TAG, "Error cmdGetStatus");
    return {};
}

std::vector<uint8_t> CcFrameAPI::receivePacket()
{
    uint8_t header[2];
    uart_read_bytes(ccUartNum, header, 2, 100);
    
    if(header[0] > 2) {
        ESP_LOGW(TAG, "Message package to short (length > 2), returning empty vector");
        return {};
    }
    uint8_t messageSizeRemaining = (header[0] - 2);
    uint8_t messageCheckSum = header[1];

    std::vector<uint8_t> messageData(messageSizeRemaining);
    if(uart_read_bytes(ccUartNum, messageData.data(), messageSizeRemaining, 100) == 0) {
        ESP_LOGW(TAG, "Error reading packet from CC uart, returning empty vector");
        return {};
    }

    uint8_t calculatedCheckSum = 0;
    for(uint8_t i = 0; i < messageSizeRemaining; i++) {
        calculatedCheckSum += messageData.at(i);
    }
    calculatedCheckSum &= 0xFF;

    if(messageCheckSum != calculatedCheckSum){
        ESP_LOGW(TAG, "Error reading packet: checksums dont match, returning empty vector");
        sendNACK();
        return {};
    }
    ESP_LOGD(TAG, "Packet receive was successful");
    sendACK();
    return messageData;
}

char *CcFrameAPI::getStatusString(uint8_t statusCode)
{
    switch (statusCode)
        {
        case 0x40:
            return "Success";
        case 0x41:
            return "Unknown command";
        case 0x42:
            return "Invalid command";
        case 0x43:
            return "Invalid address";
        case 0x44:
            return "Flash fail";
        default:
            return "Unknown";
    }
}

bool CcFrameAPI::waitForAck(uint16_t timeoutMs)
{
    const uint32_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(timeoutMs);
    
    while (xTaskGetTickCount() < deadline)
    {
        uint8_t byte;
        int length = 0;
        length = uart_read_bytes(ccUartNum, &byte, 1, 100);

        if(length > 0) {
            ESP_LOGD(TAG, "Received Byte: %i", byte);
    
            if(byte == ACK_BYTE) {
                ESP_LOGD(TAG, "ACK received");
                return true;
            }
            else if (byte == NACK_BYTE)
            {
                ESP_LOGW(TAG, "NACK received");
                return false;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    ESP_LOGW(TAG, "Timeout waiting for ACK/NACK");
    return false;
}
