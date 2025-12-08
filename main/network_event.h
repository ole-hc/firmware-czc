#pragma once
#include "esp_event.h"

// base NetworkStateMachine on events
ESP_EVENT_DECLARE_BASE(NETWORK_EVENT);
enum {
    NETWORK_EVENT_CONFIG_UPDATED
};