#include "NvsAPI.h"

const char* NvsAPI::TAG = "non-volatile-storage";

NvsAPI::NvsAPI()
    : networkConfigNamespace("netcfg"), configNamespace("config")
{
}

NvsAPI::~NvsAPI()
{
}

void NvsAPI::initNvs()
{
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Error initialising NVS (%s)", esp_err_to_name(ret));
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
        ESP_LOGW(TAG, "Formatted and initialised Nvs (%s)", esp_err_to_name(ret));
    }
    ESP_ERROR_CHECK(ret); // Abort program if second init fails
}

void NvsAPI::closeNvs()
{
}

bool NvsAPI::saveNetworkConfigToNvs(const NetworkConfig &networkConfig)
{
    nvs_handle_t handle;
    if(nvs_open(networkConfigNamespace, NVS_READWRITE, &handle) != ESP_OK) {
        ESP_LOGW(TAG, "Error while opening file");
        return false;
    }
    
    esp_err_t ret = nvs_set_blob(handle, configNamespace, &networkConfig, sizeof(networkConfig));
    if(ret != ESP_OK) return false;

    nvs_commit(handle);
    nvs_close(handle);
    esp_event_post(NETWORK_EVENT, NETWORK_EVENT_CONFIG_UPDATED, nullptr, 0, portMAX_DELAY);
    return true;
}

// maybe also boolean? and refactor. Function flow not great at all
void NvsAPI::getNetworkConfigFromNvs(NetworkConfig &networkConfig)
{
    nvs_handle_t handle;
    esp_err_t responseOpen = nvs_open(networkConfigNamespace, NVS_READONLY, &handle);
    if(responseOpen != ESP_OK) {
        ESP_LOGW(TAG, "Error while opening Nvs namespace %s,: %s", networkConfigNamespace, esp_err_to_name(responseOpen));
    }
    
    size_t size = sizeof(networkConfig);
    esp_err_t responseGet = nvs_get_blob(handle, configNamespace, &networkConfig, &size);
    nvs_close(handle);

    if(responseGet != ESP_OK) {
        ESP_LOGI(TAG, "Networkconfig not saved until now, using default, %s", esp_err_to_name(responseGet));
        networkConfig.wifiConfigured = false;
    }
}
