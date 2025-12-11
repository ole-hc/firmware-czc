#include "IoAPI.h"

const char* IoAPI::TAG = "Input/Output";

IoAPI::IoAPI()
{
    initLed(ioToGpio.modeLed);
    initLed(ioToGpio.powerLed);
    initButton(ioToGpio.button);
}

IoAPI::~IoAPI()
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
    gpio_set_level((gpio_num_t)ioToGpio.modeLed, 1);
}

void IoAPI::modeLedLow()
{
    gpio_set_level((gpio_num_t)ioToGpio.modeLed, 0);
}

void IoAPI::powerLedHigh()
{
    gpio_set_level((gpio_num_t)ioToGpio.powerLed, 1);
}

void IoAPI::powerLedLow()
{
    gpio_set_level((gpio_num_t)ioToGpio.powerLed, 0);
}
