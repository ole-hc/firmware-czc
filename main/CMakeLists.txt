idf_component_register(SRCS "WirelessAPI.cpp" "main.cpp" "FilesystemAPI.cpp" "EthernetAPI.cpp" "NvsAPI.cpp" "IoAPI.cpp" "CcZnpAPI.cpp" "CcFrameAPI.cpp" "WirelessAPI.cpp" "NetworkAPI.cpp"
                       PRIV_REQUIRES spi_flash
                       REQUIRES littlefs
                       REQUIRES esp_netif
                       REQUIRES esp_eth
                       REQUIRES nvs_flash
                       REQUIRES esp_driver_gpio
                       REQUIRES esp_wifi
                       INCLUDE_DIRS "")
