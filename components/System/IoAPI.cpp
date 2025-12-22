#include "IoAPI.h"

const char* IoAPI::TAG = "Input/Output";

IoAPI::IoAPI()
{
}

IoAPI::~IoAPI()
{
    initLed(MODE_LED_PIN);
    initLed(POWER_LED_PIN);
    initButton(BUTTON_PIN);
}

void IoAPI::initIo()
{
}

void IoAPI::initLed(uint8_t gpio)
{
    gpio_config_t ledConfig = {
        .pin_bit_mask = (1ULL << gpio),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&ledConfig);
}

void IoAPI::initButton(uint8_t gpio)
{
    gpio_config_t buttonConfig = {
        .pin_bit_mask = (1ULL << gpio),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&buttonConfig);
}

void IoAPI::modeLedHigh()
{
    gpio_set_level((gpio_num_t)MODE_LED_PIN, 1);
}

void IoAPI::modeLedLow()
{
    gpio_set_level((gpio_num_t)MODE_LED_PIN, 0);
}

void IoAPI::powerLedHigh()
{
    gpio_set_level((gpio_num_t)POWER_LED_PIN, 1);
}

void IoAPI::powerLedLow()
{
    gpio_set_level((gpio_num_t)POWER_LED_PIN, 0);
}
