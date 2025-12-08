#include "esp_log.h"

#include "EthernetAPI.h"
#include "WirelessAPI.h"
#include "NvsAPI.h"
#include "network_event.h"
#include "freertos/task.h"

enum class NetworkState {
    INIT,
    ETHERNET,
    WLAN,
    ACCESS_POINT
};

class NetworkStateMachine
{
private:
    static const char* TAG;
    NetworkState currentState;
    EthernetAPI& ethernetAPI;
    WirelessAPI& wirelessAPI;
    NvsAPI& nvsAPI;
    
    NetworkConfig networkConfig;
    bool networkConfigChanged;
    TaskHandle_t nsmHandle;

    static void got_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
    static void network_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
public:
    NetworkStateMachine(EthernetAPI& _ethernetAPI, WirelessAPI& _wirelessAPI, NvsAPI& _nvsAPI);
    ~NetworkStateMachine();
    void initNetworkStateMachine();
    void closeNetworkStateMachine();
    void runNetworkStateMachine();
    static void taskLoopNetworkStateMachine(void* pvParameters);
};
