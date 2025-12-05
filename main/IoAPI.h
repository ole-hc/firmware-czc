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

struct ioGpio {
    uint8_t button = 35;
    uint8_t modeLed = 12;
    uint8_t powerLed = 14;
};

class IoAPI
{
private:
    ioGpio ioToGpio;
    static const char* TAG;
public:
    IoAPI();
    ~IoAPI();
    void initLed(uint8_t gpio);
    void initButton(uint8_t gpio);
    void modeLedHigh();
    void modeLedLow();
    void powerLedHigh();
    void powerLedLow();
};