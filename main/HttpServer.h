#include "esp_log.h"
#include "esp_http_server.h"

class HttpServer
{
private:
    static const char* TAG;
public:
    HttpServer();
    ~HttpServer();
};

