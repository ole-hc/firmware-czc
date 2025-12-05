#include "esp_eth.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"

struct ethernetConfig {
    uint8_t phyAddr = 0; 
    uint8_t powerPin = 5; 
    uint8_t mdcPin = 23;
    uint8_t mdiPin = 18; 
    emac_rmii_clock_gpio_t clkGpio = (emac_rmii_clock_gpio_t)17;
};

class EthernetAPI
{
private:
    ethernetConfig ethConfig;
    static const char* TAG;
    esp_eth_handle_t eth_handle = NULL;

    static void eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
    static void got_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
public:
    EthernetAPI();
    ~EthernetAPI();
};