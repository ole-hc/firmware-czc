#include "esp_log.h"
#include "esp_http_server.h"

// great tutorial: https://developer.espressif.com/blog/2025/06/basic_http_server/

class HttpServer
{
private:
    static const char* TAG;
    httpd_config_t httpConfig;
    httpd_handle_t httpServer;
    static const httpd_uri_t index_uri;
public:
    HttpServer();
    ~HttpServer();
    void startHttpServer();
    void closeHttpServer();
    void indexGetHandler();
};

