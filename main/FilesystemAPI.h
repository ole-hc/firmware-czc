#include "esp_littlefs.h"
#include "esp_log.h"

class FilesystemAPI
{
private: 
    static const char* TAG;
public:
    FilesystemAPI();
    ~FilesystemAPI();
};