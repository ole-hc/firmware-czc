#pragma once
#include "nvs_flash.h"
#include "esp_log.h"

#include "configStructs.h"

class NvsAPI
{
private:
    static const char* TAG;
    char* networkConfigNamespace = "netcfg";
public:
    NvsAPI();
    ~NvsAPI();
    bool saveNetworkConfigToNvs(const NetworkConfig& networkConfig);
    void getNetworkConfigFromNvs(NetworkConfig& networkConfig);
};