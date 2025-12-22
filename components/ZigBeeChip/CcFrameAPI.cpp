#include "CcFrameAPI.h"

const char* CcFrameAPI::TAG = "CC-lowlevel-API";

CcFrameAPI::CcFrameAPI()
    :ccUartBufferSize(1024 * 2), ccUartQueue(NULL), ccUartNum(UART_NUM_1)
{
}

CcFrameAPI::~CcFrameAPI()
{
}

void CcFrameAPI::initCcUart()
{
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, ccUartBufferSize, ccUartBufferSize, 10, &ccUartQueue, 0));
    uart_config_t ccUartConfig = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
    };
    ESP_ERROR_CHECK(uart_param_config(ccUartNum, &ccUartConfig));
    ESP_ERROR_CHECK(uart_set_pin(ccUartNum, TX_PIN, RX_PIN, RST_PIN, UART_PIN_NO_CHANGE));
    initBsl();
    if(sendSynch() == false) {
        ESP_LOGW(TAG, "No begin answer from the CC chip!");
        return;
    }
    ESP_LOGI(TAG, "CC Uart communication setup successfully!");
}

void CcFrameAPI::closeCcUart()
{
}

void CcFrameAPI::initBsl()
{
    gpio_config_t bslConfig = {
        .pin_bit_mask = (1ULL << BSL_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&bslConfig);
    gpio_set_level((gpio_num_t)BSL_PIN, 1);
}

bool CcFrameAPI::sendSynch()
{
    const uint32_t cmd = 0x55;
    
    ESP_ERROR_CHECK(uart_flush(ccUartNum));
    
    uart_write_bytes(ccUartNum, (const char*)cmd, sizeof(cmd));
    uart_write_bytes(ccUartNum, (const char*)cmd, sizeof(cmd));
    
    return waitForAck(2000);
}

bool CcFrameAPI::waitForAck(uint16_t timeoutMs)
{
    const uint32_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(timeoutMs);
    
    while (xTaskGetTickCount() < deadline)
    {
        uint8_t byte;
        int length = 0;
        length = uart_read_bytes(ccUartNum, &byte, 1, 100);

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
    ESP_LOGW(TAG, "Timeout waiting for ACK/NACK");
    return false;
}