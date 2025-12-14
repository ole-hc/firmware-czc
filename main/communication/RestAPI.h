#pragma once
#include "esp_log.h"

#include "HttpServer.h"

class RestAPI
{
private:
    static const char* TAG;
    httpd_handle_t httpServer;

    httpd_uri_t indexUri;
    httpd_uri_t scriptsURI;
    httpd_uri_t styleURI;
    httpd_uri_t eventURI;
    static esp_err_t fileHandler(httpd_req_t* request, const char* path, const char* mime);
    static esp_err_t indexHandler(httpd_req_t* request);
    static esp_err_t scriptHandler(httpd_req_t* request);
    static esp_err_t styleHandler(httpd_req_t* request);
public:
    RestAPI(httpd_handle_t _httpServer);
    ~RestAPI();
    void startRestAPI();
};
