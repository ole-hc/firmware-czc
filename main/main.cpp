#include <iostream>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_mac.h"

#include "FilesystemAPI.h"
#include "EthernetAPI.h"

 /*
struct hardwareConfig {
    .addr = 0, 
    .pwrPin = 5, 
    .mdcPin = 23, 
    .mdiPin = 18, 
    .phyType = ETH_PHY_LAN8720, 
    .clkMode = ETH_CLOCK_GPIO17_OUT

    .txPin = 4, 
    .rxPin = 36, 
    .rstPin = 16, 
    .bslPin = 32

    .btnPin = 35, 
    .btnPlr = 1, 
    .uartSelPin = 33, 
    .uartSelPlr = 1, 
    .ledModePin = 12, 
    .ledModePlr = 1, 
    .ledPwrPin = 14, 
    .ledPwrPlr = 1
};
*/

extern "C" void app_main(){
    FilesystemAPI filesystemAPI;
    EthernetAPI ethernetAPI;

    esp_log_level_set("*", ESP_LOG_DEBUG); 

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    delete &filesystemAPI;
    delete &ethernetAPI;
}