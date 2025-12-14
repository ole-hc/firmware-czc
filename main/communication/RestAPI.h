#pragma once
#include "esp_log.h"

#include "HttpServer.h"
#include "FilesystemAPI.h"
#include "NvsAPI.h"
#include "IoAPI.h"
#include "SystemManager.h"
#include "NetworkStateMachine.h"

class RestAPI
{
private:
    static const char* TAG;

    httpd_handle_t httpServer;
    FilesystemAPI& filesystemAPI;
    NvsAPI& nvsAPI;
    IoAPI& ioAPI;
    // SystemManager& systemManager;
    NetworkStateMachine& networkStateMachine;

    httpd_uri_t indexUri;
    httpd_uri_t scriptsURI;
    httpd_uri_t styleURI;
    httpd_uri_t eventURI;
    static esp_err_t fileHandler(httpd_req_t* request, const char* path);
    static esp_err_t indexHandler(httpd_req_t* request);
    static esp_err_t scriptHandler(httpd_req_t* request);
    static esp_err_t styleHandler(httpd_req_t* request);
public:
    RestAPI(httpd_handle_t _httpServer, FilesystemAPI& _filesystemAPI, NvsAPI& _nvsAPI, IoAPI& _ioAPI, NetworkStateMachine& _networkStateMachine);
    ~RestAPI();
    void registerHandlers();
};
