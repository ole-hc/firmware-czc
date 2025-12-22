#pragma once
#include "esp_log.h"
#include "driver/gpio.h"

/*
.btnPin = 35, 
    .btnPlr = 1, 
    .uartSelPin = 33, 
    .uartSelPlr = 1, 
    .ledModePin = 12, 
    .ledModePlr = 1, 
    .ledPwrPin = 14, 
    .ledPwrPlr = 1
*/

#define BUTTON_PIN 35
#define MODE_LED_PIN 12
#define POWER_LED_PIN 14

class IoAPI
{
private:
    static const char* TAG;
    void initLed(uint8_t gpio);
    void initButton(uint8_t gpio);
public:
    IoAPI();
    ~IoAPI();
    void initIo();
    void modeLedHigh();
    void modeLedLow();
    void powerLedHigh();
    void powerLedLow();
};