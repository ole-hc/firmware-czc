#pragma once
#include "string"
#include "esp_log.h"

#include <vector>

#include "HttpServer.h"
#include "FilesystemAPI.h"
#include "NvsAPI.h"
#include "IoAPI.h"
#include "SystemManager.h"
#include "NetworkStateMachine.h"
#include "EventQueue.h"

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
    EventQueue& eventQueue;

    httpd_uri_t indexUri;
    httpd_uri_t scriptsURI;
    httpd_uri_t styleURI;
    httpd_uri_t eventURI;
    static std::vector<httpd_req_t*> eventStreamClients;

    static esp_err_t fileHandler(httpd_req_t* request, const char* path);
    static esp_err_t indexHandler(httpd_req_t* request);
    static esp_err_t scriptHandler(httpd_req_t* request);
    static esp_err_t styleHandler(httpd_req_t* request);
    static esp_err_t eventsHandler(httpd_req_t* request);

    esp_err_t sendEvent(httpd_req_t* eventStreamRequest, std::string event, std::string data);
    static void sendEventToAllClients(void* pvParameters);
    void pollEventDataFromComponents();
public:
    RestAPI(httpd_handle_t _httpServer, FilesystemAPI& _filesystemAPI, NvsAPI& _nvsAPI, IoAPI& _ioAPI, NetworkStateMachine& _networkStateMachine, EventQueue& _eventQueue);
    ~RestAPI();
    void registerHandlers();
    static void pollFrontendDataTask(void* pvParameters);
};
