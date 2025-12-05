idf_component_register(SRCS "main.cpp" "FilesystemAPI.cpp"
                       PRIV_REQUIRES spi_flash
                       REQUIRES littlefs
                       INCLUDE_DIRS "")
