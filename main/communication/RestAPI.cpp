#include "RestAPI.h"

const char* RestAPI::TAG = "RestAPI";

RestAPI::RestAPI(httpd_handle_t _httpServer)
    :httpServer(_httpServer)
{
}

RestAPI::~RestAPI()
{
}

void RestAPI::startRestAPI()
{
    indexUri = {
        .uri       = "/",               // the address at which the resource can be found
        .method    = HTTP_GET,          // The HTTP method (HTTP_GET, HTTP_POST, ...)
        .handler   = indexHandler,   // The function which process the request
        .user_ctx  = NULL               // Additional user data for context
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

    // eventURI = {
    //     .uri = "/events",
    //     .method = HTTP_GET,
    //     .handler = eventsHandler,
    //     .user_ctx = NULL
    // };
    // ESP_ERROR_CHECK(httpd_register_uri_handler(httpServer, &eventURI));
}

esp_err_t RestAPI::indexHandler(httpd_req_t* request) {
    return fileHandler(request, "/littlefs/index.html", "text/html");
}

esp_err_t RestAPI::scriptHandler(httpd_req_t* request) {
    return fileHandler(request, "/littlefs/script.js", "application/javascript");
}

esp_err_t RestAPI::styleHandler(httpd_req_t* request) {
    return fileHandler(request, "/littlefs/style.css", "text/css");
}

esp_err_t RestAPI::fileHandler(httpd_req_t *request, const char *path, const char *mime)
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
