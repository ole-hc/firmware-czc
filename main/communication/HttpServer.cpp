#include "HttpServer.h"

const char* HttpServer::TAG = "Http-server";

HttpServer::HttpServer()
    : httpConfig(HTTPD_DEFAULT_CONFIG()), httpServer(NULL)
{
    indexUri = {
        .uri       = "/",               // the address at which the resource can be found
        .method    = HTTP_GET,          // The HTTP method (HTTP_GET, HTTP_POST, ...)
        .handler   = indexGetHandler,   // The function which process the request
        .user_ctx  = NULL               // Additional user data for context
    };
}

HttpServer::~HttpServer()
{
}

void HttpServer::startHttpServer()
{
    ESP_ERROR_CHECK(httpd_start(&httpServer, &httpConfig));
    ESP_ERROR_CHECK(httpd_register_uri_handler(httpServer, &indexUri));
    ESP_LOGI(TAG, "Http server started!");
}

void HttpServer::closeHttpServer()
{
    ESP_ERROR_CHECK(httpd_stop(&httpServer));
}

esp_err_t HttpServer::indexGetHandler(httpd_req_t* request)
{
    ESP_LOGI(TAG, "Index handler called ..");
    const char* resp_str = "<h1>Hallo Mario</h1>";
    httpd_resp_send(request, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
