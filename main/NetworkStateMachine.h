#include "esp_log.h"

#include "EthernetAPI.h"
#include "WirelessAPI.h"
#include "NvsAPI.h"
#include "configStructs.h"

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
    NetworkState currentState = NetworkState::INIT;
    EthernetAPI ethernetAPI;
    WirelessAPI wirelessAPI;
    NvsAPI nvsAPI;
    
    NetworkConfig networkConfig;

    static void got_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
public:
    NetworkStateMachine(NvsAPI& _nvsAPI);
    ~NetworkStateMachine();
    void runNetworkStateMachine();
};
