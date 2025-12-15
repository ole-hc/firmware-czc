#include "RestAPI.h"

const char* RestAPI::TAG = "RestAPI";

RestAPI::RestAPI(httpd_handle_t _httpServer, FilesystemAPI& _filesystemAPI, NvsAPI& _nvsAPI, IoAPI& _ioAPI, NetworkStateMachine& _networkStateMachine)
    :httpServer(_httpServer), filesystemAPI(_filesystemAPI), nvsAPI(_nvsAPI), ioAPI(_ioAPI), networkStateMachine(_networkStateMachine), eventStreamRequest(nullptr)
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

esp_err_t RestAPI::indexHandler(httpd_req_t* request) {
    return fileHandler(request, "/littlefs/index.html");
}

esp_err_t RestAPI::scriptHandler(httpd_req_t* request) {
    return fileHandler(request, "/littlefs/script.js");
}

esp_err_t RestAPI::styleHandler(httpd_req_t* request) {
    return fileHandler(request, "/littlefs/style.css");
}

// setup event stream 
esp_err_t RestAPI::eventsHandler(httpd_req_t* request) {
    RestAPI* self = static_cast<RestAPI*>(request->user_ctx);
    httpd_resp_set_type(request, "text/event-stream");
    httpd_resp_set_hdr(request, "Cache-Control", "no-cache");
    httpd_resp_set_hdr(request, "Connection", "keep-alive");

    self->eventStreamRequest = request;

    return ESP_OK;
}

void RestAPI::sendEvent(std::string event, std::string data)
{
    if(eventStreamRequest == nullptr) return;

    std::string messageEvent = std::string("event: ") + event + std::string("\n");
    std::string messageData = std::string("data: ") + data + std::string("\n\n");
    httpd_resp_sendstr_chunk(eventStreamRequest, messageEvent.c_str());
    httpd_resp_sendstr_chunk(eventStreamRequest, messageData.c_str());
}

esp_err_t RestAPI::fileHandler(httpd_req_t *request, const char *path)
{
    ESP_LOGI(TAG, "Index handler called ..");
    
    FILE* f = fopen("/littlefs/index.html", "r");
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
