#pragma once
#include "esp_log.h"
#include "esp_http_server.h"

#include "RestAPI.h"

// great tutorial: https://developer.espressif.com/blog/2025/06/basic_http_server/

class HttpServer
{
private:
    static const char* TAG;
    httpd_config_t httpConfig;
    httpd_handle_t httpServer;

public:
    HttpServer();
    ~HttpServer();
    void startHttpServer();
    void closeHttpServer();
    httpd_handle_t getHandle();
};

