#include "CcChipController.h"

const char* CcChipController::TAG = "CcChipController";

CcChipController::CcChipController(CcFrameAPI& _ccFrameAPI, OutsideInterface& _outsideInterface)
    : ccFrameAPI(_ccFrameAPI), outsideInterface(_outsideInterface), ccMode(CcMode::CONTROL), ccInfo()
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
                self->outsideInterface.writeOutside(buffer, readLength);
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
            int readLength = self->outsideInterface.readOutside(buffer, sizeof(buffer));
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

void CcChipController::setCcMode(CcMode _ccMode)
{
    ccMode = _ccMode;
}
