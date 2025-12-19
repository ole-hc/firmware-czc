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

bool EventQueue::push(const char* _type, const char* _data)
{
    if(eventQueue == nullptr) {
        ESP_LOGW(TAG, "Tried to push event to uninitialized event queue");
        return false;
    }
    SseEvent event;
    strncpy(event.type, _type, strlen(_type) + 1); // copy \0 as well 
    strncpy(event.data, _data, strlen(_data) + 1);
    xQueueSend(eventQueue, &event, 0);
    return true;
}

bool EventQueue::pop(SseEvent& event)
{
    if(eventQueue == nullptr) {
        ESP_LOGW(TAG, "Tried to pop event from uninitialized event queue");
        return false;
    }
    
    BaseType_t response = xQueueReceive(eventQueue, &event, 0);
    
    if (response == pdFALSE)
    {
        ESP_LOGD(TAG, "Queue is empty, returning false");
        return false;
    }
    return true;
}
