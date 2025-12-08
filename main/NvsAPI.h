#pragma once
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_log.h"

#include "network_event.h"

struct NetworkConfig {
    bool wifiConfigured;
    char ssid[32];
    char password[64];
};

class NvsAPI
{
private:
    static const char* TAG;
    char* networkConfigNamespace;
    char* configNamespace;
public:
    NvsAPI();
    ~NvsAPI();
    void initNvs();
    void closeNvs();
    bool saveNetworkConfigToNvs(const NetworkConfig& networkConfig);
    void getNetworkConfigFromNvs(NetworkConfig& networkConfig);
};