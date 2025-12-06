#include "WirelessAPI.h"

const char* WirelessAPI::TAG = "wireless";

WirelessAPI::WirelessAPI(char* _ssid, char* _password, uint8_t _maxConnected)
{
    this->ssid = _ssid;
    this->password = _password;
    this->maxConnected = _maxConnected;
}

WirelessAPI::~WirelessAPI()
{
    esp_wifi_stop();
    esp_wifi_deinit();
}

void WirelessAPI::initAccessPoint()
{
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t initAccessPointConfig = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&initAccessPointConfig));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WirelessAPI::ap_event_handler, NULL, NULL));

    // config
    wifi_config_t accessPointConfig = {0};
    strncpy((char*)accessPointConfig.ap.ssid, this->ssid, sizeof(accessPointConfig.ap.ssid));
    strncpy((char*)accessPointConfig.ap.password, this->password, sizeof(accessPointConfig.ap.password));
    accessPointConfig.ap.ssid_len = strlen((char*)accessPointConfig.ap.ssid);
    accessPointConfig.ap.authmode = WIFI_AUTH_WPA2_PSK;
    accessPointConfig.ap.max_connection = maxConnected;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &accessPointConfig));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Accesspoint started and open, SSID: %s Psw: %s", accessPointConfig.ap.ssid, accessPointConfig.ap.password);
}

void WirelessAPI::closeAccessPoint()
{
    esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &WirelessAPI::ap_event_handler);
}

void WirelessAPI::ap_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d, reason=%d",
                 MAC2STR(event->mac), event->aid, event->reason);
    }
}
