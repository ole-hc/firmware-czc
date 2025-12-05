#include "FilesystemAPI.h"

const char* FilesystemAPI::TAG = "littlefs"; 

FilesystemAPI::FilesystemAPI()
{
    esp_vfs_littlefs_conf_t littleFsConfig = {
        .base_path = "/littlefs",
        .partition_label = "spiffs",
        .format_if_mount_failed = true,
        .read_only = false,
        .dont_mount = false,
        .grow_on_mount = false
    };
    esp_err_t ret = esp_vfs_littlefs_register(&littleFsConfig);
    if(ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to mount LittleFs (%s)\n", esp_err_to_name(ret));
    }
}

FilesystemAPI::~FilesystemAPI()
{
    const char* partitionLabel = "spiffs";
    esp_err_t ret = esp_vfs_littlefs_unregister(partitionLabel);
    if(ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to unregister LittleFs (%s)\n", esp_err_to_name(ret));
    }
}