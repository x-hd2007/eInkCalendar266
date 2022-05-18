
#ifndef _WM_CONFIG_H
#define _WM_CONFIG_H  1

#include <WString.h>

// 定义使用 WIFI MANAGER 进行初始配置
class CEInkCalendarConfig
{
public:
    char _stassid[32];
    char _stapwd[32];
    String _device_id;
    String _server;
    String _d_url;
    String _city_name;
    String _city_id;
};

extern CEInkCalendarConfig g_calendarConfig;

extern bool connect_wifi_with_config();

extern void setup_wifi_by_wifi_manager();

#endif // _WM_CONFIG_H

