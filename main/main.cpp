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
#include "HttpServer.h"

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
    WirelessAPI wirelessAPI("czc-codm", "codmcodm", 2);
    NetworkStateMachine networkStateMachine(ethernetAPI, wirelessAPI, nvsAPI);
    HttpServer httpServer;

    nvsAPI.initNvs();
    networkStateMachine.initNetworkStateMachine();
    httpServer.startHttpServer();

    esp_log_level_set("*", ESP_LOG_INFO); 

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
//      -> save in nvs if there was previously a connection to the ethernet or wifi -> dont open AP
//      
// Next step:
// Http server