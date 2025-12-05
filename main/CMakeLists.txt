idf_component_register(SRCS "main.cpp" "FilesystemAPI.cpp" "EthernetAPI.cpp" "NvsAPI.cpp"
                       PRIV_REQUIRES spi_flash
                       REQUIRES littlefs
                       REQUIRES esp_netif
                       REQUIRES esp_eth
                       REQUIRES nvs_flash
                       INCLUDE_DIRS "")
