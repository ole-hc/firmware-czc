#include "CcFrameAPI.h"

const char* CcFrameAPI::TAG = "CC-lowlevel-API";

CcFrameAPI::CcFrameAPI()
    :ccUartBufferSize(1024 * 2), ccUartQueue(NULL), ccUartNum(UART_NUM_1), inBootloaderMode(false)
{
}

CcFrameAPI::~CcFrameAPI()
{
}

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
    ESP_LOGD(TAG, "CC Uart communication setup successfully");
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
    ESP_LOGD(TAG, "CC GPIO setup complete");
}

bool CcFrameAPI::setCcBootloaderMode()
{
    if(inBootloaderMode == false) {
        setupCcChipBootloaderMode();
        if(sendSynch() == false) {
            ESP_LOGW(TAG, "Error setting up Bootloader mode");
            return false;
        }
    }
    return true;
}

bool CcFrameAPI::getCcInfo(CcInfo& ccInfo)
{
    if(inBootloaderMode == true) {
        if(setCcBootloaderMode() == false) {
            ESP_LOGW(TAG, "Error setting up bootloader mode in cmdCheckIeeeAddress");
            return false;
        }
    }

    strncpy(ccInfo.hwRevision, "CC2652P7", sizeof(ccInfo.hwRevision));

    if(cmdCheckFwVersion(ccInfo) == false) {
        ESP_LOGW(TAG, "Error in cmdCheckFwVersion called from detectChipInfo");
        return false;
    }

    if(cmdCheckIeeeAddress(ccInfo) == false) {
        ESP_LOGW(TAG, "Error in cmdCheckIeeeAddress called from detectChipInfo");
        return false;
    }

    return true;
}

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

// ping method 
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

bool CcFrameAPI::cmdEraseFlash()
{
    const uint8_t cmd = 0x2C;
    const uint8_t length = 3;

    uart_write_bytes(ccUartNum, (const char*)&length, sizeof(length));
    uart_write_bytes(ccUartNum, (const char*)&cmd, sizeof(cmd));
    uart_write_bytes(ccUartNum, (const char*)&cmd, sizeof(cmd));

    if(waitForAck(50)) {
        if(checkLastCmd()) {
            ESP_LOGD(TAG, "Successfully send eraseFlash cmd to CC");
            return true;
        }
    }

    ESP_LOGW(TAG, "Error sending eraseFlash cmd to CC");
    return false;
}

bool CcFrameAPI::cmdDownload(uint32_t address, uint32_t size)
{
    const uint8_t cmd = 0x21;
    const uint8_t length = 11;

    if((size % 4) != 0) {
        ESP_LOGW(TAG, "Invalid data size: %i. Size must be a multiple of 4.", size);
        return false;
    }

    uint8_t addressBytes[4];
    encodeAddress(address, addressBytes);

    uint8_t sizeBytes[4];
    encodeAddress(size, sizeBytes);

    uint8_t checksum = calculateChecksum(cmd, address, size);

    uart_write_bytes(ccUartNum, (const char*)&length, sizeof(length));
    uart_write_bytes(ccUartNum, (const char*)&checksum, sizeof(checksum));
    uart_write_bytes(ccUartNum, (const char*)&cmd, sizeof(cmd));

    uart_write_bytes(ccUartNum, (const char*)&addressBytes[0], sizeof(addressBytes[0]));
    uart_write_bytes(ccUartNum, (const char*)&addressBytes[1], sizeof(addressBytes[1]));
    uart_write_bytes(ccUartNum, (const char*)&addressBytes[2], sizeof(addressBytes[2]));
    uart_write_bytes(ccUartNum, (const char*)&addressBytes[3], sizeof(addressBytes[3]));

    uart_write_bytes(ccUartNum, (const char*)&sizeBytes[0], sizeof(sizeBytes[0]));
    uart_write_bytes(ccUartNum, (const char*)&sizeBytes[1], sizeof(sizeBytes[1]));
    uart_write_bytes(ccUartNum, (const char*)&sizeBytes[2], sizeof(sizeBytes[2]));
    uart_write_bytes(ccUartNum, (const char*)&sizeBytes[3], sizeof(sizeBytes[3]));

    if(waitForAck(50)) {
        if(checkLastCmd()) {
            ESP_LOGD(TAG, "Successfully send cmdDownload to CC");
            return true;
        }
    }

    ESP_LOGW(TAG, "Error sending downloadCmd to CC");
    return false;
}

bool CcFrameAPI::cmdSendData(std::vector<uint8_t> data)
{
    const uint8_t cmd = 0x24;
    const uint8_t maxDataSize = 252;
    uint8_t dataSize = data.size();

    if(dataSize > maxDataSize) {
        ESP_LOGW(TAG, "Data size to large: %i. Maximum allowed ist 252 Bytes.", dataSize);
        return false;
    }

    uint8_t length = dataSize + 3;
    uint8_t checksum = cmd;
    for (size_t i = 0; i < dataSize; i++)
    {
        checksum += data.at(i);
    }
    checksum &= 0xFF;
    
    uart_write_bytes(ccUartNum, (const char*)&length, sizeof(length));
    uart_write_bytes(ccUartNum, (const char*)&checksum, sizeof(checksum));
    uart_write_bytes(ccUartNum, (const char*)&cmd, sizeof(cmd));
    for (size_t i = 0; i < dataSize; i++)
    {
        uart_write_bytes(ccUartNum, (const char*)&data.at(i), sizeof(data.at(i)));
    }

    if(waitForAck(50)) {
        if(checkLastCmd()) {
            ESP_LOGD(TAG, "Successfully send data to CC");
            return true;
        }
    }

    ESP_LOGW(TAG, "Error while sending data to CC");
    return false;
}

bool CcFrameAPI::cmdSetLedState(bool ledState)
{
    uint8_t zigLedCmd[7] = {CMD_FRAME_START, 0x02, 0x27, 0x0A, 0x01, 0, 0};
    const uint8_t zigLedResponse[6] = {CMD_FRAME_START, 0x01, 0x67, 0x0A, 0x00, 0x6C};

    uart_flush(ccUartNum);
    if(ledState == 0) {
        zigLedCmd[5] = 0x01;
        zigLedCmd[6] = 0x2F;
    }
    else {
        zigLedCmd[5] = 0x00;
        zigLedCmd[6] = 0x2E;
    }
    uart_write_bytes(ccUartNum, &zigLedCmd, sizeof(zigLedCmd));
    uart_flush(ccUartNum);
    vTaskDelay(pdMS_TO_TICKS(400));
    for (size_t i = 0; i < 5; i++)
    {
        uint8_t readByte;
        uart_read_bytes(ccUartNum, &readByte, 1, 50);
        if(readByte != CMD_FRAME_START) {
            ESP_LOGW(TAG, "Error reading CC after LED cmd was send");
            uart_read_bytes(ccUartNum, &readByte, 1, 50);
            return false;
        }
        else {
            for (size_t j = 0; j < 4; j++)
            {
                uart_read_bytes(ccUartNum, &readByte, 1, 50);
                if(readByte != zigLedResponse[j]) {
                    ESP_LOGW(TAG, "Answer from LED cmd wrong");
                    return false;
                }
            }
        }
    }
    
    return true;
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

std::vector<uint8_t> CcFrameAPI::cmdMemRead(uint32_t address)
{
    const uint8_t cmd = 0x2A;
    const uint8_t length = 9; 

    uint8_t addressBytes[4];
    encodeAddress(address, addressBytes);
    uint8_t checksum = calculateChecksum(cmd, address, 2);
    uint8_t endBytes = 1;

    uart_write_bytes(ccUartNum, (const char*)&length, sizeof(length));
    uart_write_bytes(ccUartNum, (const char*)&checksum, sizeof(checksum));
    uart_write_bytes(ccUartNum, (const char*)&cmd, sizeof(cmd));

    uart_write_bytes(ccUartNum, (const char*)&addressBytes[0], sizeof(addressBytes[0]));
    uart_write_bytes(ccUartNum, (const char*)&addressBytes[1], sizeof(addressBytes[1]));
    uart_write_bytes(ccUartNum, (const char*)&addressBytes[2], sizeof(addressBytes[2]));
    uart_write_bytes(ccUartNum, (const char*)&addressBytes[3], sizeof(addressBytes[3]));

    uart_write_bytes(ccUartNum, (const char*)&endBytes, sizeof(endBytes));
    uart_write_bytes(ccUartNum, (const char*)&endBytes, sizeof(endBytes));

    if(waitForAck(50)) {
        std::vector<uint8_t> memData = receivePacket();
        if(checkLastCmd()) {
            ESP_LOGD(TAG, "Successfully read CC memory");
            return memData;
        }
    }

    ESP_LOGW(TAG, "Error reading CC memory!");
    return {};
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

bool CcFrameAPI::cmdCheckFwVersion(CcInfo& chip)
{
    if(inBootloaderMode == true) {
        ESP_LOGD(TAG, "[cmdCheckFwVersion] Switches out of bootloader mode -> restarted CC");
        restartCc();
        vTaskDelay(pdTICKS_TO_MS(50));
    }

    const uint8_t zbVerLen = 11;
    const uint8_t cmdSysVersion[5] = {CMD_FRAME_START, 0x00, 0x21, 0x02, 0x23};

    uart_flush(ccUartNum);
    int response = uart_write_bytes(ccUartNum, (const char*)&cmdSysVersion, sizeof(cmdSysVersion));
    ESP_LOGD(TAG, "[cmdCheckFwVersion] Return of send: %i", response);
    vTaskDelay(pdMS_TO_TICKS(100));

    uint8_t zbVersionBuffer[zbVerLen];
    int readLength = uart_read_bytes(ccUartNum, &zbVersionBuffer, zbVerLen, 50);
    ESP_LOGD(TAG, "[cmdCheckFwVersion] Return of read: %i", readLength);

    if(readLength > 0) {
        chip.fwRevision = zbVersionBuffer[5] | (zbVersionBuffer[6] << 8) | (zbVersionBuffer[7] << 16) | (zbVersionBuffer[8] << 24);
        chip.maintrel = zbVersionBuffer[4];
        chip.minorrel = zbVersionBuffer[3];
        chip.majorrel = zbVersionBuffer[2];
        chip.product = zbVersionBuffer[1];
        chip.transportrev = zbVersionBuffer[0];
        ESP_LOGI(TAG, "Successfully read Fw Version of the CC, Message size: %i", readLength);
        return true;
    }
    uart_flush(ccUartNum);

    ESP_LOGW(TAG, "Error reading Fw Version from CC!");
    return false;
}

bool CcFrameAPI::cmdCheckIeeeAddress(CcInfo& ccInfo) {
    if(inBootloaderMode == false) {
        if(setCcBootloaderMode() == false) {
            ESP_LOGW(TAG, "Error setting up bootloader mode in cmdCheckIeeeAddress");
            return false;
        }
    }

    std::vector<uint8_t> ieeeByteOne = cmdMemRead(IEEE_MEM_ADDRESS + 4);
    std::vector<uint8_t> ieeeByteTwo = cmdMemRead(IEEE_MEM_ADDRESS);
    
    if(ieeeByteOne.empty() || ieeeByteTwo.empty()) {
        ESP_LOGW(TAG, "Error reading IEEE MEM Addresses");
        return false;
    }

    snprintf(ccInfo.ieee, sizeof(ccInfo.ieee), "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", 
            ieeeByteOne[3], ieeeByteOne[2], ieeeByteOne[1], ieeeByteOne[0],
            ieeeByteTwo[3], ieeeByteTwo[2], ieeeByteTwo[1], ieeeByteTwo[0]);

    ESP_LOGW(TAG, "Read IEEE Address: %s", ccInfo.ieee);

    return true;
}

std::vector<uint8_t> CcFrameAPI::receivePacket()
{
    uint8_t header[2];
    uart_read_bytes(ccUartNum, header, 2, 100);
    ESP_LOGW(TAG, "Received Message size give in header: %i", header[0]);

    if(header[0] < 2) {
        ESP_LOGW(TAG, "Message package to short (length < 2), returning empty vector");
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
            ESP_LOGW(TAG, "Received Byte: %i", byte);
    
            if(byte == ACK_BYTE) {
                ESP_LOGW(TAG, "ACK received");
                return true;
            }
            else if (byte == NACK_BYTE)
            {
                ESP_LOGW(TAG, "NACK received");
                return false;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    ESP_LOGW(TAG, "Timeout waiting for ACK/NACK");
    return false;
}

void CcFrameAPI::encodeAddress(uint32_t address, uint8_t encodedAddress[4])
{
    encodedAddress[3] = (uint8_t)((address >> 0) & 0xFF);
    encodedAddress[2] = (uint8_t)((address >> 8) & 0xFF);
    encodedAddress[1] = (uint8_t)((address >> 16) & 0xFF);
    encodedAddress[0] = (uint8_t)((address >> 24) & 0xFF);
}

uint8_t CcFrameAPI::calculateChecksum(uint8_t cmd, uint32_t address, uint32_t size)
{
    uint8_t addressBytes[4];
    encodeAddress(address, addressBytes);
    uint8_t sizeBytes[4];
    encodeAddress(size, sizeBytes);

    uint32_t checksum = 0;
    for (uint8_t i = 0; i < 4; i++)
    {
        checksum += addressBytes[i];
        checksum += sizeBytes[i];
    }
    checksum += cmd;
    
    return (uint8_t)(checksum & 0xFF);
}

bool CcFrameAPI::checkLastCmd()
{
    std::vector<uint8_t> status = cmdGetStatus();
    if(status.empty()) {
        ESP_LOGW(TAG, "No response from target on status request, did you disable the bootloader?");
        return false;
    }

    uint8_t cmdReturn = status.at(0);
    if(cmdReturn == COMMAND_RET_SUCCESS) {
        return true;
    }
    
    char* statusString = getStatusString(cmdReturn);
    if(strcmp(statusString, "Unknown")) { 
        ESP_LOGW(TAG, "Unrecognized status returned: 0x%d", cmdReturn);
    }
    else {
        ESP_LOGW(TAG, "Target returned: 0x%d, %s", cmdReturn, statusString);
    }
    return false;
}
