
#include "config.h"

#include <FS.h> 
#include <ArduinoJson.h>
#include <WiFiManager.h>

#include "debug_info.h"
#include "wm_config.h"

#define CONFIG_FILENAME "/config.json"

WiFiManager g_wm;

CEInkCalendarConfig g_calendarConfig;

void show_device_config()
{
    Serial.printf("ssid: %s\n", g_calendarConfig._stassid);
    Serial.printf("pwd: %s\n", g_calendarConfig._stapwd);
    Serial.printf("dev_id: %s\n", g_calendarConfig._device_id.c_str());
    Serial.printf("server: %s\n", g_calendarConfig._server.c_str());
    Serial.printf("d_url: %s\n", g_calendarConfig._d_url.c_str());
    Serial.printf("city_name: %s\n", g_calendarConfig._city_name.c_str());
    Serial.printf("city_id: %s\n", g_calendarConfig._city_id.c_str());
}

String make_json_pair(const char* kval, const char* val)
{
    String ret = "\"";
    ret += kval;
    ret += "\":\"";
    ret += val;
    ret += "\"";
    return ret;
}

String save_device_config_to_json()
{
    String ret = "{";
    ret += make_json_pair("dev_id", g_calendarConfig._device_id.c_str());
    ret += ",";
    ret += make_json_pair("server", g_calendarConfig._server.c_str());
    ret += ",";
    ret += make_json_pair("d_url", g_calendarConfig._d_url.c_str());
    ret += ",";
    ret += make_json_pair("city_name", g_calendarConfig._city_name.c_str());
    ret += ",";
    ret += make_json_pair("city_id", g_calendarConfig._city_id.c_str());
    ret += ",";
    ret += make_json_pair("ssid", g_calendarConfig._stassid);
    ret += ",";
    ret += make_json_pair("pwd", g_calendarConfig._stapwd);
    ret += "}";
    return ret;
}

void parse_device_config(String& content)
{
    show_debug_info("\nbegin parse device config!");

    if (content.length() <= 0) {
        Serial.println("device config is empty!");
    }

    DynamicJsonDocument jdoc(1024);
    deserializeJson(jdoc, content);

    String wifi_val = "";
    
    g_calendarConfig._device_id = jdoc["dev_id"].as<String>();
    g_calendarConfig._server = jdoc["server"].as<String>();
    g_calendarConfig._d_url = jdoc["d_url"].as<String>();
    g_calendarConfig._city_name = jdoc["city_name"].as<String>();
    g_calendarConfig._city_id = jdoc["city_id"].as<String>();

    wifi_val = jdoc["ssid"].as<String>();
    strcpy(g_calendarConfig._stassid, wifi_val.c_str());

    wifi_val = jdoc["pwd"].as<String>();
    strcpy(g_calendarConfig._stapwd, wifi_val.c_str());

    show_debug_info("parse device config ok!");

    show_device_config();

}

bool load_calendar_config()
{
    String content = "";
    char rbuf[64];
    fs::File file;
    memset(g_calendarConfig._stassid, 0, sizeof(g_calendarConfig._stassid));
    memset(g_calendarConfig._stapwd, 0, sizeof(g_calendarConfig._stapwd));
    file = SPIFFS.open(CONFIG_FILENAME, "r");
    if (!file) {
        Serial.printf("Open Config: %s failed!", CONFIG_FILENAME);
        return false;
    }
    while (file.available())
    {
        memset(rbuf, 0, sizeof(rbuf));
        file.read((uint8_t*)rbuf, 60);
        content += rbuf;
    }
    file.close();

#if DEBUG_INFO > 0
    Serial.printf("Config: %s", content.c_str());
#endif // DEBUG_INFO

    if (content.length() > 0) {
        parse_device_config(content);
    }
    return true;
}

void save_calendar_config()
{
    String content = "";
    fs::File file;
    file = SPIFFS.open(CONFIG_FILENAME, "w");
    if (!file) {
        Serial.printf("Write Config: %s failed!", CONFIG_FILENAME);
        return ;
    }
    content = save_device_config_to_json();
    file.seek(0, SeekMode::SeekSet);
    file.write((uint8_t*)content.c_str(), strlen(content.c_str()));
    file.close();
    Serial.println("Write Config file Ok!");
}

bool connect_wifi_network()
{
    bool ret = false;

    WiFi.begin(g_calendarConfig._stassid, g_calendarConfig._stapwd);
    byte wcount = 0;

    while ( wcount <= 10) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.print(".");
        }
        else {
            Serial.printf("\nConnect Wifi : %s\n", WiFi.localIP().toString().c_str());
            ret = true;
            break;
        }
        delay(1000);
        wcount++;
    }
    return ret;
}

/**
 * wifi manager 参数保存回调函数
 */ 
void save_param_callback()
{
    Serial.println("[CALLBACK] save param callback fired");

    int pcount = g_wm.getParametersCount();
    WiFiManagerParameter** params = g_wm.getParameters();

    if (pcount <= 0) {
        return ;
    }

    if (params == NULL) {
        return ;
    }
    
    for (int pid = 0; pid < pcount; pid++)
    {
        WiFiManagerParameter* param = params[pid];
        if (param == NULL)
        {
            continue;
        }
            
        if (param->getID() != NULL && param->getValue() != NULL)
        {
            Serial.printf("%s: %s\n", param->getID(), param->getValue());
                
            if (strcmp(param->getID(), "device_id") == 0)
            {
                g_calendarConfig._device_id = "";
                g_calendarConfig._device_id = param->getValue();
            }
            
            if (strcmp(param->getID(), "server_ip") == 0)
            {
                g_calendarConfig._server = param->getValue();
            }

            if (strcmp(param->getID(), "server_url") == 0)
            {
                g_calendarConfig._d_url = param->getValue();
            }

            if (strcmp(param->getID(), "city_name") == 0)
            {
                g_calendarConfig._city_name = param->getValue();
            }

            if (strcmp(param->getID(), "city_id") == 0)
            {
                g_calendarConfig._city_id = param->getValue();
            }
        }
    }
}

/**
 * WEB配网函数
 * WiFi manager 启动配置
 */ 
void wm_config()
{
    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

    delay(3000);
    g_wm.resetSettings(); // wipe settings

    WiFiManagerParameter dev_id("device_id", "device_id", "V202201_0001", 12); 
    WiFiManagerParameter server_ip("server_ip", "server_ip", "192.168.11.28", 16);
    WiFiManagerParameter server_url("server_url", "server_url", "todotask/devtask.php", 20);
    WiFiManagerParameter city_name("city_name", "city_name", "上海", 10);
    WiFiManagerParameter city_id("city_id", "city_id", "101020100", 12);

    g_wm.addParameter(&dev_id);
    g_wm.addParameter(&server_ip);
    g_wm.addParameter(&server_url);
    g_wm.addParameter(&city_name);
    g_wm.addParameter(&city_id);
    
    g_wm.setSaveConfigCallback(save_param_callback);

    // wifi scan settings
    g_wm.setMinimumSignalQuality(30); // set min RSSI (percentage) to show in scans, null = 8%

    bool res;
    
    res = g_wm.autoConnect("ecalendar"); // anonymous ap
    
    if (res) {
        Serial.println("wifi manager is autoconn ok!");
    }
    else {
        Serial.println("wifi manager autoconnect failed!");
    }

    while (!res)
    {
        ;
    }
}

bool connect_wifi_with_config()
{
    bool ret = false;
    if (!load_calendar_config()) {
        return ret;
    }
    if (connect_wifi_network()) {
        ret = true;
    }
    return ret;
}

void setup_wifi_by_wifi_manager()
{
    // DONE: 显示 wifi manager 配置界面
    // wifi manager 配置网络
    wm_config();

    if (WiFi.status() == WL_CONNECTED)
    {
        strcpy(g_calendarConfig._stassid, WiFi.SSID().c_str());
        strcpy(g_calendarConfig._stapwd, WiFi.psk().c_str());
        save_calendar_config();
        load_calendar_config();
        Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
    }
}


