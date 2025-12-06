#pragma once
// config Structs used in multiple different classes

struct NetworkConfig {
    bool wifiConfigured;
    char ssid[32];
    char password[64];
};