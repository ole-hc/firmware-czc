idf_component_register(
    SRCS 
        "FilesystemAPI.cpp" 
        "NvsAPI.cpp" 
    INCLUDE_DIRS "."
    REQUIRES 
        spi_flash
        littlefs
        nvs_flash
        Network
)
