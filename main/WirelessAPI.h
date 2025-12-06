#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_mac.h"
#include "esp_log.h"

class WirelessAPI
{
private:
    static const char* TAG;
    char* ssid;
    char* password;
    uint8_t maxConnected;

    static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    static void ap_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
public:
    WirelessAPI(char* _ssid, char* _password, uint8_t _maxConnected);
    ~WirelessAPI();
    void initAccessPoint();
    void closeAccessPoint();
};

