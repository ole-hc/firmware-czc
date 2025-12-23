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
    ESP_LOGW(TAG, "IAM HERE");
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
    setupCcChip();
    if(sendSynch() == false) {
        ESP_LOGW(TAG, "No begin answer from the CC chip!");
        return;
    }
    ESP_LOGI(TAG, "CC Uart communication setup successfully!");
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

void CcFrameAPI::setupCcChip()
{
    gpio_set_level((gpio_num_t)BSL_PIN, 0);
    gpio_set_level((gpio_num_t)RST_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(50));
    gpio_set_level((gpio_num_t)RST_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(500));
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