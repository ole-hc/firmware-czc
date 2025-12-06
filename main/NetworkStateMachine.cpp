#include "NetworkStateMachine.h"

const char* NetworkStateMachine::TAG = "network-state-machine";

NetworkStateMachine::NetworkStateMachine(NvsAPI& _nvsAPI)
{ 
    ESP_LOGW(TAG, "Constructor");
    ethernetAPI = EthernetAPI();
    wirelessAPI = WirelessAPI();

    this->nvsAPI = _nvsAPI;
    nvsAPI.getNetworkConfigFromNvs(networkConfig);
    if(networkConfig.wifiConfigured) {
        wirelessAPI.setWirelessConfig(networkConfig.ssid, networkConfig.password, 2); // if wifi was previously configure, configure accordingly and start wifi 
        wirelessAPI.initWifi();
    }

    this->currentState = NetworkState::INIT;
    esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &NetworkStateMachine::got_ip_event_handler, this); // Ip handler for Ethernet and Wireless
}

NetworkStateMachine::~NetworkStateMachine()
{
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, &NetworkStateMachine::got_ip_event_handler);
    // netif delete i dont think its needed
}

void NetworkStateMachine::runNetworkStateMachine()
{
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
                nvsAPI.getNetworkConfigFromNvs(networkConfig);
                if(wirelessAPI.getActiveWirelessMode() != ActiveWirelessMode::WIFI) {
                    wirelessAPI.setWirelessConfig("czc-codm", "codm", 2); 
                    wirelessAPI.initAccessPoint();
                    networkConfig.wifiConfigured = false;
                }
                else if(ethernetAPI.getEthIsConnected()) {
                    wirelessAPI.closeAccessPoint();
                    currentState = NetworkState::ETHERNET;
                }
                else if (networkConfig.wifiConfigured == true) {
                    if(wirelessAPI.getActiveWirelessMode() == ActiveWirelessMode::ACCESSPOINT) {
                        wirelessAPI.closeAccessPoint();
                    }
                    wirelessAPI.setWirelessConfig(networkConfig.ssid, networkConfig.password, 2);
                    wirelessAPI.initWifi();
                    currentState = NetworkState::WLAN;
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
            auto* event = (ip_event_got_ip_t*) event_data;
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
            auto* event = (ip_event_got_ip_t*) event_data;
            ESP_LOGI(TAG, "Ethernet lost IP!");
            self->ethernetAPI.setEthIsConnected(false);
            break;
        }
    }
}
