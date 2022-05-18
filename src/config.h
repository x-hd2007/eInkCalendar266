
#ifndef _CONFIG_H_
#define _CONFIG_H_  1

// 和风天气接口说明
#define HEFENG_API_KEY "自行申请的和风key"

// 项目编译配置开关

// 配置是否显示调试信息及相关函数ls
#define DEBUG_INFO  0

// 配置使用 WIFI 模式
#define DEVICE_WIFI_MODE   1

#if DEVICE_WIFI_MODE > 0

// 设置使用 WIFI MANAGER 配置
#define WIFI_MANAGER_CONFIG  1

#endif // DEVICE_WIFI_MODE

// 定义程序固件版本
#define DEVICE_FIRMWARE_VERSION "V1.54fad8e9@20220209"

#endif // _CONFIG_H_

