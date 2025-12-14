#include "HttpServer.h"

const char* HttpServer::TAG = "Http-server";

HttpServer::HttpServer()
    : httpConfig(HTTPD_DEFAULT_CONFIG()), httpServer(NULL)
{
}

HttpServer::~HttpServer()
{
}

void HttpServer::startHttpServer()
{
    ESP_ERROR_CHECK(httpd_start(&httpServer, &httpConfig));
    RestAPI restAPI(httpServer);
    restAPI.startRestAPI();
    ESP_LOGI(TAG, "Http server started!");
}

void HttpServer::closeHttpServer()
{
    ESP_ERROR_CHECK(httpd_stop(&httpServer));
}
