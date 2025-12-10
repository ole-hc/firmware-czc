idf_component_register(SRCS "HttpServer.cpp" "main.cpp" "FilesystemAPI.cpp" "EthernetAPI.cpp" "NvsAPI.cpp" "IoAPI.cpp" "CcZnpAPI.cpp" "CcFrameAPI.cpp" "WirelessAPI.cpp" "NetworkStateMachine.cpp" "HttpServer.cpp"
                       PRIV_REQUIRES spi_flash
                       REQUIRES littlefs
                       REQUIRES esp_netif
                       REQUIRES esp_eth
                       REQUIRES nvs_flash
                       REQUIRES esp_driver_gpio
                       REQUIRES esp_wifi
                       REQUIRES esp_http_server
                       REQUIRES esp_timer
                       INCLUDE_DIRS "")
