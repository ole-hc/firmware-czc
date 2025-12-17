#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

struct SseEvent
{
    char type[32];
    char data[32];
};


class EventQueue
{
private:
    static const char* TAG;
    QueueHandle_t eventQueue;
public:
    EventQueue();
    ~EventQueue();
    void initQueue();
    void push(const char* _type, const char* _data);
    SseEvent pop();
};
