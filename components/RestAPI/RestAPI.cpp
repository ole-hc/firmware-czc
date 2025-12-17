#include "RestAPI.h"

const char* RestAPI::TAG = "RestAPI";
std::vector<httpd_req_t*> RestAPI::eventStreamClients;

RestAPI::RestAPI(httpd_handle_t _httpServer, FilesystemAPI& _filesystemAPI, NvsAPI& _nvsAPI, IoAPI& _ioAPI, NetworkStateMachine& _networkStateMachine, EventQueue& _eventQueue)
    :httpServer(_httpServer), filesystemAPI(_filesystemAPI), nvsAPI(_nvsAPI), ioAPI(_ioAPI), networkStateMachine(_networkStateMachine), eventQueue(_eventQueue)
{
}

RestAPI::~RestAPI()
{
}

void RestAPI::registerHandlers()
{
    indexUri = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = indexHandler,
        .user_ctx  = NULL
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(httpServer, &indexUri));

    scriptsURI = {
    .uri = "/script.js",
    .method = HTTP_GET,
    .handler = scriptHandler,
    .user_ctx = NULL
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(httpServer, &scriptsURI));

    styleURI = {
        .uri = "/style.css",
        .method = HTTP_GET,
        .handler = styleHandler,
        .user_ctx = NULL
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(httpServer, &styleURI));

    eventURI = {
        .uri = "/events",
        .method = HTTP_GET,
        .handler = eventsHandler,
        .user_ctx = this
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(httpServer, &eventURI));
}

// poll event based data and 
void RestAPI::pollFrontendDataTask(void* pvParameters)
{
    auto* self = static_cast<RestAPI*>(pvParameters);
    while (true)
    {
        self->pollEventDataFromComponents();
        self->sendEventToAllClients();
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }    
}

esp_err_t RestAPI::indexHandler(httpd_req_t* request) {
    httpd_resp_set_type(request, "text/html");
    return fileHandler(request, "/littlefs/index.html");
}

esp_err_t RestAPI::scriptHandler(httpd_req_t* request) {
    httpd_resp_set_type(request, "application/javascript");
    return fileHandler(request, "/littlefs/script.js");
}

esp_err_t RestAPI::styleHandler(httpd_req_t* request) {
    httpd_resp_set_type(request, "text/css");
    return fileHandler(request, "/littlefs/style.css");
}

// setup event stream 
esp_err_t RestAPI::eventsHandler(httpd_req_t* request) {
    RestAPI* self = static_cast<RestAPI*>(request->user_ctx);
    httpd_resp_set_type(request, "text/event-stream");
    httpd_resp_set_hdr(request, "Cache-Control", "no-cache");
    httpd_resp_set_hdr(request, "Connection", "keep-alive");

    self->eventStreamClients.push_back(request); // register Client
    ESP_LOGI(TAG, "Client connected to event stream");

    return ESP_OK;
}

esp_err_t RestAPI::fileHandler(httpd_req_t *request, const char *path)
{
    ESP_LOGI(TAG, "Index handler called ..");
    
    FILE* f = fopen(path, "r");
    if (!f) {
        httpd_resp_send_404(request);
        return ESP_FAIL;
    }
    char buf[512];
    size_t read_bytes;
    while ((read_bytes = fread(buf, 1, sizeof(buf), f)) > 0) {
        httpd_resp_send_chunk(request, buf, read_bytes);
    }
    fclose(f);
    httpd_resp_send_chunk(request, NULL, 0); 
    return ESP_OK;
}

esp_err_t RestAPI::sendEvent(httpd_req_t* eventStreamRequest, std::string event, std::string data)
{
    if(eventStreamRequest == nullptr) return ESP_FAIL;
    esp_err_t response = ESP_OK;

    std::string messageEvent = std::string("event: ") + event + std::string("\n");
    std::string messageData = std::string("data: ") + data + std::string("\n\n");
    httpd_resp_sendstr_chunk(eventStreamRequest, messageEvent.c_str());
    response = httpd_resp_sendstr_chunk(eventStreamRequest, messageData.c_str());
    return response;
}

// REFACTOR AAGGGHGHHH
// Fehler mit der Gültigkeit der Request handels. Mögliche Lösung ist es im SSE handler eine Task zu starten welche sich mit dem management auseinandersetzt 
// im handler bleibt die referenz auf jeden fall gültig und mehrere clients wären auch ohne vector möglich. 
// vorher aber lieber nochmal weiter rechachieren über mehrere clients im eventstream aufm esp32 
void RestAPI::sendEventToAllClients()
{
    ESP_LOGW(TAG, "Start send event");
    uint8_t counter = 0;
    std::string sseEventErrorState = "NULL";
    for (httpd_req_t* client : eventStreamClients)
    {
        ESP_LOGW(TAG, "Start send event");
        SseEvent toBeSend = eventQueue.pop();
        if(toBeSend.type == sseEventErrorState && toBeSend.data == sseEventErrorState) {
            ESP_LOGW(TAG, "Received faulty Sse event!");
            break;
        }
        
        esp_err_t response = sendEvent(client, std::string(toBeSend.type), std::string(toBeSend.data));
        if (response) // TCP connection closed -> removing client
        {
            ESP_LOGI(TAG, "Client disconnected from event stream");
            eventStreamClients.erase(eventStreamClients.begin() + counter);
        }
        counter++;
    }
}

void RestAPI::pollEventDataFromComponents()
{
    SseEvent newEvent("Test", "CheckCheck");
    eventQueue.push(newEvent.type, newEvent.data);
}
