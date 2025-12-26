#include "OutsideInterface.h"

const char* OutsideInterface::TAG = "OutsideInterface";

OutsideInterface::OutsideInterface(OutsideUart& _outsideUart, OutsideSocket& _outsideSocket)
    :outsideUart(_outsideUart), outsideSocket(_outsideSocket) ,proxyMode(ProxyMode::NETWORK)
{
}

OutsideInterface::~OutsideInterface()
{
}

void OutsideInterface::initOutsideInterface()
{
}

void OutsideInterface::closeOutsideInterface()
{
}

int OutsideInterface::writeOutside(uint8_t *buffer, uint8_t bufferLength)
{
    if(proxyMode == ProxyMode::USB) {

    }
    else if (proxyMode == ProxyMode::NETWORK) {

    }
    ESP_LOGW(TAG, "Error sending data to the outside");
    return 0;
}

int OutsideInterface::readOutside(uint8_t *buffer, uint8_t bufferLength)
{
    if(proxyMode == ProxyMode::USB) {

    }
    else if (proxyMode == ProxyMode::NETWORK) {

    }
    ESP_LOGW(TAG, "Error reading data from the outside");
    return 0;
}

void OutsideInterface::setOutsideInterface(ProxyMode _proxyMode)
{
    if(proxyMode == _proxyMode) return;

    // state exit 
    switch (proxyMode)
    {
    case ProxyMode::USB:
        /* code */
        break;
    
    case ProxyMode::NETWORK:
        break;
    }

    proxyMode = _proxyMode;

    // state init
    switch (proxyMode)
    {
    case ProxyMode::USB:
        /* code */
        break;
    
    case ProxyMode::NETWORK:
        break;
    }
}
