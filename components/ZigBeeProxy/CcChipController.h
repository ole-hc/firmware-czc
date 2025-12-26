#include "esp_log.h"

#include "CcFrameAPI.h"
#include "OutsideInterface.h"

#define PROXY_BUFFER_SIZE 512

enum class CcMode {
    COORDINATOR,
    ROUTER, 
    THREAD, 
    CONTROL
};

class CcChipController
{
private:
    static const char* TAG;
    CcFrameAPI& ccFrameAPI;
    OutsideInterface& outsideInterface;
    CcMode ccMode;
    CcInfo ccInfo;


public:
    CcChipController(CcFrameAPI& _ccFrameAPI, OutsideInterface& _outsideInterface);
    ~CcChipController();

    void initCc();
    void closeCc();
    void testFunction();
    static void ccToOutsideTask(void* pvParameters);
    static void outsideToCcTask(void* pvParameters);
    void setProxyMode(ProxyMode _proxyMode);
    void setCcMode(CcMode _ccMode);
};
