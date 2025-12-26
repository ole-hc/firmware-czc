#pragma once
#include "esp_log.h"

#include "OutsideUart.h"
#include "OutsideSocket.h"

enum class ProxyMode {
    USB,
    NETWORK
};

class OutsideInterface
{
private:
    static const char* TAG;

    OutsideUart& outsideUart;
    OutsideSocket& outsideSocket;
    ProxyMode proxyMode;

public:
    OutsideInterface(OutsideUart& _outsideUart, OutsideSocket& _outsideSocket);
    ~OutsideInterface();

    void initOutsideInterface();
    void closeOutsideInterface();

    int writeOutside(uint8_t *buffer, uint8_t bufferLength);
    int readOutside(uint8_t *buffer, uint8_t bufferLength);

    void setOutsideInterface(ProxyMode _proxyMode);
};
