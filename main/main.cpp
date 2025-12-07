#include <iostream>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_mac.h"

#include "FilesystemAPI.h"
#include "NvsAPI.h"
#include "IoAPI.h"
#include "CcFrameAPI.h"
#include "CcZnpAPI.h"
#include "NetworkStateMachine.h"

 /*
struct hardwareConfig {
    .txPin = 4, 
    .rxPin = 36, 
    .rstPin = 16, 
    .bslPin = 32
};
*/

extern "C" void app_main(){
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    FilesystemAPI filesystemAPI;
    NvsAPI nvsAPI;
    IoAPI ioAPI;
    EthernetAPI ethernetAPI;
    WirelessAPI wirelessAPI;
    NetworkStateMachine networkStateMachine(ethernetAPI, wirelessAPI, nvsAPI);
    nvsAPI.initNvs();
    networkStateMachine.initNetworkStateMachine();

    esp_log_level_set("*", ESP_LOG_DEBUG); 

    ioAPI.powerLedHigh();
    ioAPI.modeLedHigh();

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    nvsAPI.closeNvs();
    networkStateMachine.closeNetworkStateMachine();
}

// Todo: NetworkStateMachine 
//      -> Nvs bug
//      -> event based / no more polling !(Especially with the Accesspoint nvs-config polling)
//      
// Next step:
// task for NetworkStateMachine
// Http server when Connection logic runs 