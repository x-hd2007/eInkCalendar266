
// 加载配置文件
#include "config.h"

// 用到的库文件
#include <Arduino.h>
#include <ArduinoJson.h>

#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#endif 

#include <WiFiClient.h>
#include <vector>
#include <ESPDateTime.h>
#include "MD5Builder.h"

#include "debug_info.h"
#include "my_task.h"


#if WIFI_MANAGER_CONFIG > 0
#include "wm_config.h"
#endif // WIFI_MANAGER_CONFIG

#if FIRMWARE_FILE_CONFIG > 0
#include "wifi_config.h"
#endif // FIRMWARE_FILE_CONFIG

using namespace std;

String g_userName;
String g_lunarYear;
String g_lunarDay;

// 工作任务清单
vector<my_task> g_taskList;

/**
 * @param [in]
 * @return
 */
void show_task_list()
{
    if (g_taskList.size() > 0)
    {
        vector<my_task>::iterator it;
        for (it = g_taskList.begin(); it != g_taskList.end(); it++)
        {
            Serial.printf("Task Info: %s %s %d %ld %ld\n", it->m_taskInfo.c_str(),
                          it->m_taskTag.c_str(), it->m_priority, it->m_beginTime, it->m_endTime);
        }
    }
}

/**
 * @param [in]
 * @return 
 */
int get_task_count()
{
    int ret = 0;
    ret = g_taskList.size();
    return ret;
}

/**
 * @param [in]
 * @return
 */ 
my_task* get_task_info(int tid)
{
    my_task* ret = NULL;
    if (tid >= 0 && tid < g_taskList.size())
    {
        ret = &g_taskList[tid];
    }
    return ret;
}

/**
 * 解析工作任务
 * @param [in] tasks 工作任务
 * @return
 */ 
void parse_my_task_list(String& tasks)
{
    // DONE: 执行任务清单解析

    show_debug_info("\nbegin parse task json");

    if (tasks.length() <= 6) {
        Serial.println("task json is empty!");
    }

    g_taskList.clear();

    DynamicJsonDocument jdoc(2048);
    deserializeJson(jdoc, tasks);

    JsonArray taskArray = jdoc["task_list"];

    for (JsonVariant item : taskArray)
    {
        JsonObject lobj = item.as<JsonObject>();
        my_task task;

        task.m_taskInfo = lobj["ti"].as<String>();
        task.m_taskTag = lobj["tt"].as<String>();
        task.m_priority = lobj["pri"].as<uint8_t>();
        task.m_beginTime = lobj["bt"].as<time_t>();
        task.m_endTime = lobj["et"].as<time_t>();

        g_taskList.push_back(task);
    }

    g_userName = jdoc["userName"].as<String>();
    g_lunarYear = jdoc["lYear"].as<String>();
    g_lunarDay = jdoc["lDay"].as<String>();

    show_debug_info("parse task list ok!");

#if DEBUG_INFO > 0
    show_task_list();
#endif

}

#define DEVICE_MD5_PREFIX "eInkCalendar"

/**
 * 计算 MD5 HASH 值
 * @param [in] val 待校验值
 * @return
 */ 
String calcMd5(const char* val)
{
    String ret = "";
    MD5Builder md5;
    md5.begin();
    md5.add(val);
    md5.calculate();
    ret = md5.toString().c_str();
    return ret;
}

/**
 * 返回工作任务地址
 */ 
String get_task_url()
{
#if WIFI_MANAGER_CONFIG > 0

    String ret = "http://";
    ret += g_calendarConfig._server.c_str();
    ret += "/";
    ret += g_calendarConfig._d_url.c_str();
    ret += "?uid=";
    ret += g_calendarConfig._device_id.c_str();
    ret += "&ak=";

#endif 

#if FIRMWARE_FILE_CONFIG > 0

    String ret = "http://";
    ret += g_server.c_str();
    ret += "/";
    ret += g_task_url;
    ret += "?uid=";
    ret += g_device_id;
    ret += "&ak=";

#endif 

    String dak = DEVICE_MD5_PREFIX;
    dak += DateTime.format("%H").c_str();

#if WIFI_MANAGER_CONFIG > 0
    dak += g_calendarConfig._device_id.c_str();
#endif 

#if FIRMWARE_FILE_CONFIG > 0    
    dak += g_device_id;
#endif 

    ret += calcMd5(dak.c_str());

    return ret;
}

/**
 * 加载待办工作实现
 * @param [in] url 网络地址
 */ 
void load_todo_task_impl(const char* url)
{

    WiFiClient client;
    HTTPClient httpClient;

    show_debug_info(url);

    if(httpClient.begin(client, url))
    {
        u8_t httpCode = httpClient.GET();
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {
            String payload = httpClient.getString();
            show_debug_info(payload.c_str());
            parse_my_task_list(payload);
        }
        else
        {
            Serial.println("http code is failed\n");
        }
    }
    else
    {
        Serial.println("httpClient failed.");
    }
    httpClient.end();
    client.stop();
}

/**
 * 加载待办工作
 */ 
void load_todo_task()
{
    String turl = get_task_url();
    load_todo_task_impl(turl.c_str());
}
