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
#include "NvsAPI.h"
#include "IoAPI.h"
#include "CcFrameAPI.h"
#include "CcZnpAPI.h"

 /*
struct hardwareConfig {
    .txPin = 4, 
    .rxPin = 36, 
    .rstPin = 16, 
    .bslPin = 32
};
*/

extern "C" void app_main(){
    FilesystemAPI filesystemAPI;
    EthernetAPI ethernetAPI;
    NvsAPI nvsAPI;
    IoAPI ioAPI;

    esp_log_level_set("*", ESP_LOG_DEBUG); 

    ioAPI.powerLedHigh();
    ioAPI.modeLedHigh();

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    delete &filesystemAPI;
    delete &ethernetAPI;
    delete &nvsAPI;
    delete &ioAPI;
}