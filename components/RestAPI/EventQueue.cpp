#include "EventQueue.h"

const char* EventQueue::TAG = "SseEventQueue";

EventQueue::EventQueue()
    :eventQueue(nullptr)
{
}

EventQueue::~EventQueue()
{
}

void EventQueue::initQueue()
{
    eventQueue = xQueueCreate(10, sizeof(SseEvent));
    ESP_LOGI(TAG, "Event queue initialised");
}

void EventQueue::push(const char* _type, const char* _data)
{
    if(eventQueue == nullptr) {
        ESP_LOGW(TAG, "Tried to push event to uninitialized event queue");
        return;
    }
    SseEvent event;
    strncpy(event.type, _type, sizeof(_type));
    strncpy(event.data, _data, sizeof(_data));
    xQueueSend(eventQueue, &event, 0);
}

SseEvent EventQueue::pop()
{
    SseEvent event {"NULL", "NULL"};
    
    if(eventQueue == nullptr) {
        ESP_LOGW(TAG, "Tried to pop event from uninitialized event queue");
        return event;
    }
    xQueueReceive(eventQueue, &event, 0);
    return event;
}
