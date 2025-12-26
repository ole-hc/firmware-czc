// add in documentation:
// https://software-dl.ti.com/simplelink/esd/plugins/simplelink_zigbee_sdk_plugin/1.60.01.09/exports/docs/zigbee_user_guide/html/zigbee/index.html
// https://software-dl.ti.com/simplelink/esd/simplelink_cc13x2_26x2_sdk/3.20.00.68/exports/docs/zigbee/html/zigbee/z-stack-overview.html

// Default settings are COORDINATOR IN NETWORK MODE
// Tasks run the whole time but will be set in control mode when bootloader mode is needed or the CC is updated. In control mode the tasks idle. The actual control functions 
// will be called through other components like the webinterface or the IO interface. No need for a third task.
// Logging will be disabled in USB mode as the UART is used to transmit zigbee data. 
// This configuration is done in the changemode functions where transistion between modes is handled safely.
// Explanation of different modes:
// Coordinator / Thread = Proxy
// Router = do nothing
// Control = talk with CC for maybe also in bootloader mode
// ---
// USB: Proxy output on USB -> DEBUG output disabled
// NETWORK: Proxy output on in NetworkStateMachine configured media 
#include "CcChipController.h"

const char* CcChipController::TAG = "CcChipController";

CcChipController::CcChipController(CcFrameAPI& _ccFrameAPI)
    :ccFrameAPI(_ccFrameAPI), ccMode(CcMode::CONTROL), proxyMode(ProxyMode::NETWORK), ccInfo()
{
}

CcChipController::~CcChipController()
{
}

void CcChipController::initCc()
{
    ccFrameAPI.initCcUart();
    ccFrameAPI.initBslAndRst();
}

void CcChipController::closeCc()
{
}

void CcChipController::testFunction()
{
    ESP_LOGW(TAG, "Called test function");
    ccFrameAPI.setCcBootloaderMode();
    // ccFrameAPI.cmdCheckFwVersion(ccInfo);
    // ESP_LOGW(TAG, "Fw revision of CC chip: %i", ccInfo.fwRevision);
    ccFrameAPI.getCcInfo(ccInfo);
}

void CcChipController::ccToOutsideTask(void *pvParameters)
{
    auto* self = static_cast<CcChipController*>(pvParameters);

    uint8_t buffer[PROXY_BUFFER_SIZE];
    while (true)
    {
        if(self->ccMode == CcMode::COORDINATOR || self->ccMode == CcMode::THREAD) {
            int readLength = self->ccFrameAPI.readCcUart(buffer, sizeof(buffer));
            if(readLength > 0) {
                self->writeOutsideUart(buffer, readLength);
            }

            vTaskDelay(100);
        }
        else {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

void CcChipController::outsideToCcTask(void *pvParameters)
{
    auto* self = static_cast<CcChipController*>(pvParameters);

    uint8_t buffer[PROXY_BUFFER_SIZE];
    while (true)
    {
        if(self->ccMode == CcMode::COORDINATOR || self->ccMode == CcMode::THREAD) {
            int readLength = self->readOutsideUart(buffer, sizeof(buffer));
            if(readLength > 0) {
                self->ccFrameAPI.writeCcUart(buffer, readLength);
            }

            vTaskDelay(100);
        }
        else {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

void CcChipController::setProxyMode(ProxyMode _proxyMode)
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

void CcChipController::setCcMode(CcMode _ccMode)
{
    ccMode = _ccMode;
}
