#include "esp_log.h"

#include "CcFrameAPI.h"

#define PROXY_BUFFER_SIZE 512

enum class CcMode {
    COORDINATOR,
    ROUTER, 
    THREAD, 
    CONTROL
};

enum class ProxyMode {
    USB,
    NETWORK
};

class CcChipController
{
private:
    static const char* TAG;
    CcFrameAPI& ccFrameAPI;
    CcMode ccMode;
    ProxyMode proxyMode;
    uart_port_t outsideUartNum;
    CcInfo ccInfo;

    void initOutsideUart();
    void closeOutsideUart();
    int readOutsideUart(uint8_t* buffer, uint8_t bufferLength);
    int writeOutsideUart(uint8_t* buffer, uint8_t bufferLength);

public:
    CcChipController(CcFrameAPI& _ccFrameAPI);
    ~CcChipController();

    void initCc();
    void closeCc();
    void testFunction();
    static void ccToOutsideTask(void* pvParameters);
    static void outsideToCcTask(void* pvParameters);
    void setProxyMode(ProxyMode _proxyMode);
    void setCcMode(CcMode _ccMode);
};
