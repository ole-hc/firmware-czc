// State machine event based 
//      -> switching between modes
//      -> save mode in nvs
//      -> init Serial com
//      -> init and implement proxy queue  

// add in documentation:
// https://software-dl.ti.com/simplelink/esd/plugins/simplelink_zigbee_sdk_plugin/1.60.01.09/exports/docs/zigbee_user_guide/html/zigbee/index.html
// https://software-dl.ti.com/simplelink/esd/simplelink_cc13x2_26x2_sdk/3.20.00.68/exports/docs/zigbee/html/zigbee/z-stack-overview.html

#include "CcChipController.h"

const char* CcChipController::TAG = "CcChipController";

CcChipController::CcChipController(CcFrameAPI& _ccFrameAPI)
    :ccFrameAPI(_ccFrameAPI), ccInfo()
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
