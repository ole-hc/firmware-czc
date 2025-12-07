#include "NetworkStateMachine.h"

const char* NetworkStateMachine::TAG = "network-state-machine";
ESP_EVENT_DEFINE_BASE(NETWORK_EVENT);

NetworkStateMachine::NetworkStateMachine(EthernetAPI& _ethernetAPI, WirelessAPI& _wirelessAPI, NvsAPI& _nvsAPI) 
    : currentState(NetworkState::INIT), ethernetAPI(_ethernetAPI), wirelessAPI(_wirelessAPI), nvsAPI(_nvsAPI), networkConfigChanged(false)
{   
    nvsAPI.getNetworkConfigFromNvs(networkConfig); // Test and fix bugs !!!!!!!!!!!!!
}

NetworkStateMachine::~NetworkStateMachine()
{
}

void NetworkStateMachine::initNetworkStateMachine()
{
    ethernetAPI.initEthernet();
    ESP_ERROR_CHECK(esp_netif_init());
    if(networkConfig.wifiConfigured) {
        wirelessAPI.setWirelessConfig(networkConfig.ssid, networkConfig.password, 2); // if wifi was previously configured, configure accordingly and start wifi 
        wirelessAPI.initWifi();
    }
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &NetworkStateMachine::got_ip_event_handler, this)); // Ip handler for Ethernet and Wireless
    ESP_ERROR_CHECK(esp_event_handler_register(NETWORK_EVENT, ESP_EVENT_ANY_ID, &NetworkStateMachine::network_event_handler, this));
}

void NetworkStateMachine::closeNetworkStateMachine()
{
    ethernetAPI.closeEthernet();
    ActiveWirelessMode wirelessMode = wirelessAPI.getActiveWirelessMode();
    if(wirelessMode == ActiveWirelessMode::WIFI) wirelessAPI.closeWifi();
    else if(wirelessMode == ActiveWirelessMode::ACCESSPOINT) wirelessAPI.closeAccessPoint();
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, ESP_EVENT_ANY_ID, &NetworkStateMachine::got_ip_event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(NETWORK_EVENT, ESP_EVENT_ANY_ID, &NetworkStateMachine::network_event_handler));
}

void NetworkStateMachine::runNetworkStateMachine()
{
    // implement with eventstream or similar not polling -> cleaner and faster 
    switch (currentState)
    {
        case NetworkState::INIT:
            if(ethernetAPI.getEthIsConnected()) {
                currentState = NetworkState::ETHERNET;
            } 
            else if(wirelessAPI.getWifiIsConnected()) {
                currentState = NetworkState::WLAN;
            }
            else {
                // maybe retries or smn
                if(wirelessAPI.getActiveWirelessMode() != ActiveWirelessMode::WIFI) {
                    wirelessAPI.setWirelessConfig("czc-codm", "codm", 2); 
                    wirelessAPI.initAccessPoint();
                    networkConfig.wifiConfigured = false;
                }
                currentState = NetworkState::ACCESS_POINT;
            }
            break;
        
        case NetworkState::ETHERNET:
            if(!ethernetAPI.getEthIsConnected()) {
                if(wirelessAPI.getWifiIsConnected()) {
                    currentState = NetworkState::WLAN;
                }
            }
            break;
        
        case NetworkState::WLAN:
            if(!wirelessAPI.getWifiIsConnected()) {
                if(ethernetAPI.getEthIsConnected()) {
                    currentState = NetworkState::ETHERNET;
                }
            }   
            break;

        case NetworkState::ACCESS_POINT:
            if(networkConfigChanged == true) {
                nvsAPI.getNetworkConfigFromNvs(networkConfig);
                if(wirelessAPI.getActiveWirelessMode() == ActiveWirelessMode::ACCESSPOINT) {
                    wirelessAPI.closeAccessPoint();
                }
                wirelessAPI.setWirelessConfig(networkConfig.ssid, networkConfig.password, 2);
                wirelessAPI.initWifi();
                currentState = NetworkState::WLAN;
            }
            else if(ethernetAPI.getEthIsConnected()) {
                wirelessAPI.closeAccessPoint();
                currentState = NetworkState::ETHERNET;
            }    
            break;

        default:
            break;
    }
}

void NetworkStateMachine::got_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    auto* self = static_cast<NetworkStateMachine*>(arg);
    
    switch (event_id) {
        case IP_EVENT_STA_GOT_IP: {
            auto* event = (ip_event_got_ip_t*) event_data;
            ESP_LOGI(TAG, "Wifi got IP: " IPSTR, IP2STR(&event->ip_info.ip));
            self->wirelessAPI.setWifiIsConnected(true);
            break;
        }
        
        case IP_EVENT_STA_LOST_IP: {
            ESP_LOGI(TAG, "Wifi lost IP!");
            self->wirelessAPI.setWifiIsConnected(false);
            break;
        }

        case IP_EVENT_ETH_GOT_IP: {
            auto* event = (ip_event_got_ip_t*) event_data;
            ESP_LOGI(TAG, "Ethernet got IP: " IPSTR, IP2STR(&event->ip_info.ip));
            self->ethernetAPI.setEthIsConnected(true);
            break;
        }

        case IP_EVENT_ETH_LOST_IP: {
            ESP_LOGI(TAG, "Ethernet lost IP!");
            self->ethernetAPI.setEthIsConnected(false);
            break;
        }
    }
}

void NetworkStateMachine::network_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    auto* self = static_cast<NetworkStateMachine*>(arg);
    switch (event_id) {
        case NETWORK_EVENT_CONFIG_UPDATED: {
            ESP_LOGI(TAG, "Wifi config changed!");
            self->networkConfigChanged = true;
            break;
        }
    }
}
