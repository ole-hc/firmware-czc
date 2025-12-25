#include "esp_log.h"

#include "CcFrameAPI.h"

class CcChipController
{
private:
    static const char* TAG;
    CcFrameAPI& ccFrameAPI;
    CcInfo ccInfo;

public:
    CcChipController(CcFrameAPI& _ccFrameAPI);
    ~CcChipController();

    void initCc();
    void closeCc();
    void testFunction();
};
