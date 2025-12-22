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

CcChipController::CcChipController()
{
}

CcChipController::~CcChipController()
{
}