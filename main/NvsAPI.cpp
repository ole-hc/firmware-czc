#include "NvsAPI.h"

const char* NvsAPI::TAG = "non-volatile-storage";

NvsAPI::NvsAPI()
{
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Error initialising NVS (%s)", esp_err_to_name(ret));
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
        ESP_LOGW(TAG, "Formatted und initialised Nvs (%s)", esp_err_to_name(ret));
    }
    ESP_ERROR_CHECK(ret); // Abort program if second init fails
}

NvsAPI::~NvsAPI()
{
}
