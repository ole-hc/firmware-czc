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
#include "RestAPI.h"
#include "EventQueue.h"

extern "C" void app_main(){
    // --- setup ---
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    FilesystemAPI filesystemAPI;
    
    NvsAPI nvsAPI;
    nvsAPI.initNvs();
    
    IoAPI ioAPI;
    ioAPI.initIo();
    
    SystemManager systemManager;
    //systemManager.initSystemManager();

    EthernetAPI ethernetAPI;
    
    WirelessAPI wirelessAPI("czc-codm", "codmcodm", 2);
    
    NetworkStateMachine networkStateMachine(ethernetAPI, wirelessAPI, nvsAPI);
    networkStateMachine.initNetworkStateMachine();

    HttpServer httpServer;
    httpServer.startHttpServer();

    EventQueue eventQueue;
    eventQueue.initQueue();

    RestAPI restAPI(httpServer.getHandle(), filesystemAPI, nvsAPI, ioAPI, systemManager, networkStateMachine, eventQueue);
    restAPI.registerHandlers();
    TaskHandle_t updateFrontendTask = NULL; 
    xTaskCreate(restAPI.pollFrontendDataTask, "SendEventstreamDataToFrontend", 4096, &restAPI, 5, &updateFrontendTask);

    esp_log_level_set("*", ESP_LOG_INFO); 

    // --- program --- 
    ioAPI.powerLedHigh();
    ioAPI.modeLedHigh();

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // --- close --- 
    nvsAPI.closeNvs();
    networkStateMachine.closeNetworkStateMachine();
    httpServer.closeHttpServer();
    //systemManager.closeSystemManager();
    if(updateFrontendTask != NULL) {
        vTaskDelete(updateFrontendTask);
    }
}

// Todo: NetworkStateMachine 
//      -> Nvs bug
//      -> save in nvs if there was previously a connection to the ethernet or wifi -> dont open AP
//      -> test network state machine behavior -> add way to config wifi -> test reconnection loop with eth and wifi every 5 seconds
//      -> add ethernet on / off (init or only object created) for better querys in state machine -> cant get information if ethernet is initialised but not connected 
// Put object init from constructor to initialisation method (Filesystem, nvs, ioApi)      
//
// Next step:
// Http server
// own SseEvent class 