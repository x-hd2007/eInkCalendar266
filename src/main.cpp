
#include "config.h"

// 各种库说明
#include "library.h"

#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_7C.h>
#include <U8g2_for_Adafruit_GFX.h>

// esp 8266 显示函数
#if defined(ESP8266)
#include "_esp8266.h"
#endif

#include <ArduinoJson.h>

#include <FS.h>
#define FileClass fs::File
#define EPD_CS SS

#include <WString.h>

// 使用 文泉驿等宽微米黑
#include "u8g2_wqymonoweimi_12_gb2312.h"
#include "u8g2_wqymonoweimi_24_number.h"

#include <ESPDateTime.h>
#include "QWeather.h"

// 文字显示
U8G2_FOR_ADAFRUIT_GFX g_u8g2Fonts;

int16_t DISPLAY_WIDTH;
int16_t DISPLAY_HEIGHT;

QWeather g_hfApi;
GeoInfo g_gInfo;
CurrentWeather g_cw;

// may affect performance
static const uint16_t g_input_buffer_pixels = 400; 

// for wave share 4.2 inch e_paper module B  400*300
static const uint16_t max_row_width = 400;

// for depth <= 8
static const uint16_t max_palette_pixels = 256; 

uint8_t input_buffer[3 * g_input_buffer_pixels];        // up to depth 24
uint8_t output_row_mono_buffer[max_row_width / 8];    // buffer for at least one row of b/w bits
uint8_t output_row_color_buffer[max_row_width / 8];   // buffer for at least one row of color bits
uint8_t mono_palette_buffer[max_palette_pixels / 8];  // palette buffer for depth <= 8 b/w
uint8_t color_palette_buffer[max_palette_pixels / 8]; // palette buffer for depth <= 8 c/w
uint16_t rgb_palette_buffer[max_palette_pixels];      // palette buffer for depth <= 8 for buffered graphics, needed for 7-color display

/* Conversion factor for micro seconds to seconds */
#define uS_TO_S_FACTOR 1000000ULL 

/* 休眠时间
*/ 
#if DEBUG_INFO > 0
#define TIME_TO_SLEEP 60 * 60
#else
#define TIME_TO_SLEEP 60 * 60
#endif

#include "my_task.h"

#include "my_datetime.h"
#include "my_utils.h"

#ifdef DEVICE_WIFI_MODE

#if WIFI_MANAGER_CONFIG > 0
#include "wm_config.h"
#endif // WIFI_MANAGER_CONFIG

#endif

/**
 * 读取 uint16_t
 */ 
uint16_t read16(fs::File &f)
{
    // BMP data is stored little-endian, same as Arduino.
    uint16_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read(); // MSB
    return result;
}

/**
 * 读取 uint32_t
 */ 
uint32_t read32(fs::File &f)
{
    // BMP data is stored little-endian, same as Arduino.
    uint32_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read();
    ((uint8_t *)&result)[2] = f.read();
    ((uint8_t *)&result)[3] = f.read(); // MSB
    return result;
}

/**
 * 绘制bmp图片
 * @param [in] filename 文件名
 * @param [in] x, y 坐标
 */ 
void drawBitmapFromSpiffs_Buffered(const char *filename, int16_t x, int16_t y, 
                                   bool with_color, bool partial_update, 
                                   bool overwrite)
{
    fs::File file;
    bool valid = false; // valid format to be handled
    bool flip = true;   // bitmap is stored bottom-to-top
    bool has_multicolors = display.epd2.panel == GxEPD2::ACeP565;
    uint32_t startTime = millis();
    if ((x >= DISPLAY_WIDTH) || (y >= DISPLAY_HEIGHT))
        return;
    Serial.println();
    Serial.print("Loading image '");
    Serial.print(filename);
    Serial.println('\'');
#if defined(ESP32)
    file = SPIFFS.open(String("/") + filename, "r");
#else
    file = SPIFFS.open(filename, "r");
#endif
    if (!file)
    {     
        Serial.print("File not found");
        return;
    }
    // Parse BMP header
    if (read16(file) == 0x4D42) // BMP signature
    {
        uint32_t fileSize = read32(file);
        uint32_t creatorBytes = read32(file);
        uint32_t imageOffset = read32(file); // Start of image data
        uint32_t headerSize = read32(file);
        uint32_t width = read32(file);
        uint32_t height = read32(file);
        uint16_t planes = read16(file);
        uint16_t depth = read16(file); // bits per pixel
        uint32_t format = read32(file);
        if ((planes == 1) && ((format == 0) || (format == 3))) // uncompressed is handled, 565 also
        {
            Serial.print("File size: ");
            Serial.println(fileSize);
            Serial.print("Image Offset: ");
            Serial.println(imageOffset);
            Serial.print("Header size: ");
            Serial.println(headerSize);
            Serial.print("Bit Depth: ");
            Serial.println(depth);
            Serial.print("Image size: ");
            Serial.print(width);
            Serial.print('x');
            Serial.println(height);
            // BMP rows are padded (if needed) to 4-byte boundary
            uint32_t rowSize = (width * depth / 8 + 3) & ~3;
            if (depth < 8)
                rowSize = ((width * depth + 8 - depth) / 8 + 3) & ~3;
            if (height < 0)
            {
                height = -height;
                flip = false;
            }
            uint16_t w = width;
            uint16_t h = height;
            if ((x + w - 1) >= DISPLAY_WIDTH)
                w = DISPLAY_WIDTH - x;
            if ((y + h - 1) >= DISPLAY_HEIGHT)
                h = DISPLAY_HEIGHT - y;
            //if (w <= max_row_width) // handle with direct drawing
            {
                valid = true;
                uint8_t bitmask = 0xFF;
                uint8_t bitshift = 8 - depth;
                uint16_t red, green, blue;
                bool whitish, colored;
                if (depth == 1)
                    with_color = false;
                if (depth <= 8)
                {
                    if (depth < 8)
                        bitmask >>= depth;
                    //file.seek(54); //palette is always @ 54
                    file.seek(imageOffset - (4 << depth)); // 54 for regular, diff for colorsimportant
                    for (uint16_t pn = 0; pn < (1 << depth); pn++)
                    {
                        blue = file.read();
                        green = file.read();
                        red = file.read();
                        file.read();
                        whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                        colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0));                                                  // reddish or yellowish?
                        if (0 == pn % 8)
                            mono_palette_buffer[pn / 8] = 0;
                        mono_palette_buffer[pn / 8] |= whitish << pn % 8;
                        if (0 == pn % 8)
                            color_palette_buffer[pn / 8] = 0;
                        color_palette_buffer[pn / 8] |= colored << pn % 8;
                        rgb_palette_buffer[pn] = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
                    }
                }
                if (partial_update)
                    display.setPartialWindow(x, y, w, h);
                else
                    display.setFullWindow();
                display.firstPage();
                do
                {
                    if (!overwrite)
                        display.fillScreen(GxEPD_WHITE);
                    uint32_t rowPosition = flip ? imageOffset + (height - h) * rowSize : imageOffset;
                    for (uint16_t row = 0; row < h; row++, rowPosition += rowSize) // for each line
                    {
                        uint32_t in_remain = rowSize;
                        uint32_t in_idx = 0;
                        uint32_t in_bytes = 0;
                        uint8_t in_byte = 0; // for depth <= 8
                        uint8_t in_bits = 0; // for depth <= 8
                        uint16_t color = GxEPD_WHITE;
                        file.seek(rowPosition);
                        for (uint16_t col = 0; col < w; col++) // for each pixel
                        {
                            // Time to read more pixel data?
                            if (in_idx >= in_bytes) // ok, exact match for 24bit also (size IS multiple of 3)
                            {
                                in_bytes = file.read(input_buffer, in_remain > sizeof(input_buffer) ? sizeof(input_buffer) : in_remain);
                                in_remain -= in_bytes;
                                in_idx = 0;
                            }
                            switch (depth)
                            {
                                case 24:
                                    blue = input_buffer[in_idx++];
                                    green = input_buffer[in_idx++];
                                    red = input_buffer[in_idx++];
                                    whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                                    colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0));                                                  // reddish or yellowish?
                                    color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
                                    break;
                                case 16:
                                    {
                                        uint8_t lsb = input_buffer[in_idx++];
                                        uint8_t msb = input_buffer[in_idx++];
                                        if (format == 0) // 555
                                        {
                                            blue = (lsb & 0x1F) << 3;
                                            green = ((msb & 0x03) << 6) | ((lsb & 0xE0) >> 2);
                                            red = (msb & 0x7C) << 1;
                                            color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
                                        }
                                        else // 565
                                        {
                                            blue = (lsb & 0x1F) << 3;
                                            green = ((msb & 0x07) << 5) | ((lsb & 0xE0) >> 3);
                                            red = (msb & 0xF8);
                                            color = (msb << 8) | lsb;
                                        }
                                        whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                                        colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0));                                                  // reddish or yellowish?
                                    }
                                    break;
                                case 1:
                                case 4:
                                case 8:
                                    {
                                        if (0 == in_bits)
                                        {
                                            in_byte = input_buffer[in_idx++];
                                            in_bits = 8;
                                        }
                                        uint16_t pn = (in_byte >> bitshift) & bitmask;
                                        whitish = mono_palette_buffer[pn / 8] & (0x1 << pn % 8);
                                        colored = color_palette_buffer[pn / 8] & (0x1 << pn % 8);
                                        in_byte <<= depth;
                                        in_bits -= depth;
                                        color = rgb_palette_buffer[pn];
                                    }
                                    break;
                            }
                            if (with_color && has_multicolors)
                            {
                                // keep color
                            }
                            else if (whitish)
                            {
                                color = GxEPD_WHITE;
                            }
                            else if (colored && with_color)
                            {
                                color = GxEPD_COLORED;
                            }
                            else
                            {
                                color = GxEPD_BLACK;
                            }
                            uint16_t yrow = y + (flip ? h - row - 1 : row);
                            display.drawPixel(x + col, yrow, color);
                        } // end pixel
                    }   // end line
                } while (display.nextPage());
                Serial.print("loaded in ");
                Serial.print(millis() - startTime);
                Serial.println(" ms");
            }
        }
    }
    file.close();
    if (!valid)
    {
        Serial.println("bitmap format not handled.");
    }
}

#define CALENDAR_BLOCK_WIDTH 40
#define CALENDAR_BLOCK_HEIGHT 30

#define MAX_TODOLIST_COUNT 10

/**
 * 显示待办工作
 */
void show_todolist_impl(int start_y)
{
    // DONE: 绘制待办工作

    // 设置 12 号字体
    g_u8g2Fonts.setFont(u8g2_wqymonoweimi_12_gb2312);

    // 绘制底部内容
    start_y += 10;
    display.fillRect(10, start_y, 280, 2, GxEPD_RED);

    start_y += 22;
    int taskcount = get_task_count();
    if (taskcount > MAX_TODOLIST_COUNT) {
        // 最多显示 10 条记录
        taskcount = MAX_TODOLIST_COUNT;
    }
    for (int index = 0; index < taskcount; index++)
    {
        my_task* task = get_task_info(index);
        if (task != NULL)
        {
            if (task->m_priority <= 2)
            {
                g_u8g2Fonts.setForegroundColor(GxEPD_RED);
            }
            else
            {
                g_u8g2Fonts.setForegroundColor(GxEPD_BLACK);
            }
            String tinfo = "[ ] ";
            tinfo += task->m_taskInfo;
            g_u8g2Fonts.drawUTF8(10 + 10, start_y, tinfo.c_str());
            start_y += CALENDAR_BLOCK_HEIGHT;
        }
        if (start_y >= 395)
        {
            break;
        }
    }
}

/**
 * 显示工作任务实现
 * @param [in] start_y 起始高度
 */ 
void show_todolist_impl2(int start_y)
{
    // DONE: 绘制待办工作

    // 设置 12 号字体
    g_u8g2Fonts.setFont(u8g2_wqymonoweimi_12_gb2312);

    // 绘制底部内容
    start_y += 10;

    start_y += 22;
    int taskcount = get_task_count();
    if (taskcount > MAX_TODOLIST_COUNT)
    {
        // 最多显示 10 条记录
        taskcount = MAX_TODOLIST_COUNT;
    }
    for (int index = 0; index < taskcount; index++)
    {
        my_task* task = get_task_info(index);
        if (task != NULL)
        {
            if (task->m_priority <= 2)
            {
                g_u8g2Fonts.setForegroundColor(GxEPD_RED);
            }
            else
            {
                g_u8g2Fonts.setForegroundColor(GxEPD_BLACK);
            }
            String tinfo = "[ ] ";
            tinfo += task->m_taskInfo;
            g_u8g2Fonts.drawUTF8(10 + 10, start_y, tinfo.c_str());
            start_y += CALENDAR_BLOCK_HEIGHT;
        }
        if (start_y >= 395)
        {
            break;
        }
    }
}

/**
 * 显示天气及待办工作清单
 * @param [in] cname 城市名称
 */ 
void show_weather_todolist(const char* cname)
{
    String dateStr = DateTime.format("%d").c_str();

    g_u8g2Fonts.setForegroundColor(GxEPD_RED);
    g_u8g2Fonts.setFont(u8g2_wqymonoweimi_24_number);
    uint16_t strWidth = g_u8g2Fonts.getUTF8Width(dateStr.c_str());

    // 修改为靠左显示, 右边显示时间
    g_u8g2Fonts.drawUTF8(10, 65, dateStr.c_str());

    g_u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    dateStr = DateTime.format("%Y.%m").c_str();
    g_u8g2Fonts.setFont(u8g2_wqymonoweimi_12_gb2312);
    g_u8g2Fonts.drawUTF8(10, 25, dateStr.c_str());

    // 显示农历纪年
    dateStr = g_lunarYear;
    strWidth = g_u8g2Fonts.getUTF8Width(dateStr.c_str());
    g_u8g2Fonts.drawUTF8(150 - strWidth - 10, 25, dateStr.c_str());

    // 显示阴历日期
    dateStr = g_lunarDay;
    strWidth = g_u8g2Fonts.getUTF8Width(dateStr.c_str());
    g_u8g2Fonts.drawUTF8(150 - strWidth - 10, 55, dateStr.c_str());

    dateStr = g_WEEKDAY_CN[DateTime.getParts().getWeekDay()];
    strWidth = g_u8g2Fonts.getUTF8Width(dateStr.c_str());
    g_u8g2Fonts.drawUTF8(150 - strWidth - 10, 90, dateStr.c_str());

    dateStr = DateTime.format("%H:%M:%S");
    g_u8g2Fonts.drawUTF8(10,  90, dateStr.c_str());

    dateStr = cname;
    g_u8g2Fonts.setFont(u8g2_wqymonoweimi_12_gb2312);
    g_u8g2Fonts.setForegroundColor(GxEPD_RED);
    g_u8g2Fonts.drawUTF8(160, 30, dateStr.c_str());

    g_u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    g_u8g2Fonts.setFont(u8g2_wqymonoweimi_12_gb2312);

    // 显示天气
    dateStr = g_cw.text;
    strWidth = g_u8g2Fonts.getUTF8Width(dateStr.c_str());
    g_u8g2Fonts.drawUTF8(290 - strWidth - 10, 90, dateStr.c_str());

    // 显示温度
    dateStr = g_cw.temp;
    dateStr += "℃";
    g_u8g2Fonts.drawUTF8(160, 60, dateStr.c_str());

    // 显示湿度
    dateStr = "湿度: ";
    dateStr += g_cw.humidity;
    g_u8g2Fonts.drawUTF8(160, 90, dateStr.c_str());

    g_u8g2Fonts.setFont(u8g2_wqymonoweimi_12_gb2312);
    dateStr = "* 我的待办";

    // 增加显示用户名称
    if (strlen(g_userName.c_str()) > 0)
    {
        dateStr += " - ";
        dateStr += g_userName;
    }

    g_u8g2Fonts.setForegroundColor(GxEPD_RED);
    g_u8g2Fonts.drawUTF8(10, 125, dateStr.c_str());

    display.writeLine(149, 10, 149, 100, GxEPD_RED);
    display.writeLine(150, 10, 150, 100, GxEPD_RED);
    display.writeLine(10, 99, 290, 99, GxEPD_RED);
    display.writeLine(10, 100, 290, 100, GxEPD_RED);

    show_todolist_impl2(120);

}

void show_month_calendar2(const char* cname)
{
    String dateStr = DateTime.format("%d").c_str();

    g_u8g2Fonts.setForegroundColor(GxEPD_RED);
    g_u8g2Fonts.setFont(u8g2_wqymonoweimi_24_number);
    uint16_t strWidth = g_u8g2Fonts.getUTF8Width(dateStr.c_str());

    // 修改为靠左显示, 右边显示时间
    g_u8g2Fonts.drawUTF8(10, 65, dateStr.c_str());

    g_u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    dateStr = DateTime.format("%Y.%m").c_str();
    g_u8g2Fonts.setFont(u8g2_wqymonoweimi_12_gb2312);
    g_u8g2Fonts.drawUTF8(10, 25, dateStr.c_str());

    // 显示农历纪年
    dateStr = g_lunarYear;
    strWidth = g_u8g2Fonts.getUTF8Width(dateStr.c_str());
    g_u8g2Fonts.drawUTF8(150 - strWidth - 10, 25, dateStr.c_str());

    // 显示阴历日期
    dateStr = g_lunarDay;
    strWidth = g_u8g2Fonts.getUTF8Width(dateStr.c_str());
    g_u8g2Fonts.drawUTF8(150 - strWidth - 10, 55, dateStr.c_str());

    dateStr = g_WEEKDAY_CN[DateTime.getParts().getWeekDay()];
    strWidth = g_u8g2Fonts.getUTF8Width(dateStr.c_str());
    g_u8g2Fonts.drawUTF8(150 - strWidth - 10, 90, dateStr.c_str());

    dateStr = DateTime.format("%H:%M:%S");
    g_u8g2Fonts.drawUTF8(10,  90, dateStr.c_str());

    dateStr = cname;
    g_u8g2Fonts.setFont(u8g2_wqymonoweimi_12_gb2312);
    g_u8g2Fonts.setForegroundColor(GxEPD_RED);
    g_u8g2Fonts.drawUTF8(160, 30, dateStr.c_str());

    g_u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    g_u8g2Fonts.setFont(u8g2_wqymonoweimi_12_gb2312);

    // 显示天气
    dateStr = g_cw.text;
    strWidth = g_u8g2Fonts.getUTF8Width(dateStr.c_str());
    g_u8g2Fonts.drawUTF8(290 - strWidth - 10, 90, dateStr.c_str());

    // 显示温度
    dateStr = g_cw.temp;
    dateStr += "℃";
    g_u8g2Fonts.drawUTF8(160, 60, dateStr.c_str());

    // 显示湿度
    dateStr = "湿度: ";
    dateStr += g_cw.humidity;
    g_u8g2Fonts.drawUTF8(160, 90, dateStr.c_str());

    display.writeLine(149, 10, 149, 100, GxEPD_RED);
    display.writeLine(150, 10, 150, 100, GxEPD_RED);
    display.writeLine(10, 99, 290, 99, GxEPD_RED);
    display.writeLine(10, 100, 290, 100, GxEPD_RED);

    int start_y = 120;

    // 显示日历
    dateStr = DateTime.format("%d").c_str();
    dateStr += " ";
    dateStr += g_MONTH_EN[DateTime.getParts().getMonth()];
    dateStr += ", ";
    dateStr += DateTime.format("%Y").c_str();
    g_u8g2Fonts.setFont(u8g2_wqymonoweimi_12_gb2312);

    // 居中显示年月
    strWidth = g_u8g2Fonts.getUTF8Width(dateStr.c_str());
    g_u8g2Fonts.setForegroundColor(GxEPD_RED);
    // g_u8g2Fonts.drawUTF8((DISPLAY_WIDTH - strWidth) / 2, 16 + 12, dateStr.c_str());
    g_u8g2Fonts.drawUTF8((DISPLAY_WIDTH - strWidth) / 2, start_y + 16 + 12, dateStr.c_str());

    // 显示顶部横线
    // display.writeLine(10, 36 + start_y, 290, 36 + start_y, GxEPD_BLACK);

    int index = 0;

    // 显示日历 偏移: 10, 宽度 30 * 7, 占屏幕一半 
    // 显示头部
    for (index = 0; index < 7; index++)
    {
        // 每星期周一开始
        if (index == 0 || index == 6)
        {
            g_u8g2Fonts.setForegroundColor(GxEPD_RED);
        }
        else
        {
            g_u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        }
        g_u8g2Fonts.drawUTF8(10 + 10 + ((index + 6) % 7) * CALENDAR_BLOCK_WIDTH, start_y + 46 + 12, g_WEEKDAY_EN[index]);
    }

    start_y  += 76 + 12;
    int mday = g_month_week[0];
    int curday = DateTime.getParts().getMonthDay();
    g_u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    for (index = 1; index <= mday; index++)
    {
        String dStr(index);
        if (index < 10)
        {
            dStr = " " + dStr;
        }
        // 绘制当月日历
        if (index == curday)
        {
            // 红色显示当天
            g_u8g2Fonts.setForegroundColor(GxEPD_RED);

            // 当天增加下划线显示
            display.writeLine(10 + 10 + ((g_month_week[index] + 6) % 7) * CALENDAR_BLOCK_WIDTH, start_y + 2,
                              10 + 10 + ((g_month_week[index] + 6) % 7) * CALENDAR_BLOCK_WIDTH + (CALENDAR_BLOCK_WIDTH / 2), 
                              start_y + 2, GxEPD_RED);

        }
        else
        {
            g_u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        }
        g_u8g2Fonts.drawUTF8(10 + 10 + ((g_month_week[index] + 6) % 7) * CALENDAR_BLOCK_WIDTH, start_y, dStr.c_str());
        if (g_month_week[index] <= 0 && index != mday) {
            start_y += CALENDAR_BLOCK_HEIGHT;
        }
    }

}

/**
 * 显示当月日历
 * @param [in] stask 是否显示工作任务
 * @return
 */ 
void show_month_calendar(int stask)
{
    Serial.println("show month calendar be called!");

    String dateStr = DateTime.format("%d").c_str();
    dateStr += " ";
    dateStr += g_MONTH_EN[DateTime.getParts().getMonth()];
    // String dateStr = g_MONTH_CN[DateTime.getParts().getMonth()];
    dateStr += ", ";
    dateStr += DateTime.format("%Y").c_str();
    g_u8g2Fonts.setFont(u8g2_wqymonoweimi_12_gb2312);

    // 居中显示年月
    uint16_t strWidth = g_u8g2Fonts.getUTF8Width(dateStr.c_str());
    g_u8g2Fonts.setForegroundColor(GxEPD_RED);
    g_u8g2Fonts.drawUTF8((DISPLAY_WIDTH - strWidth) / 2, 16 + 12, dateStr.c_str());

    // 显示顶部横线
    display.writeLine(10, 36, 290, 36, GxEPD_BLACK);

    int start_y  = 76 + 12;
    int index = 0;

    // 显示日历 偏移: 10, 宽度 30 * 7, 占屏幕一半 
    // 显示头部
    for (index = 0; index < 7; index++)
    {
        // 每星期周一开始
        if (index == 0 || index == 6)
        {
            g_u8g2Fonts.setForegroundColor(GxEPD_RED);
        }
        else
        {
            g_u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        }
        g_u8g2Fonts.drawUTF8(10 + 10 + ((index + 6) % 7) * CALENDAR_BLOCK_WIDTH, 46 + 12, g_WEEKDAY_EN[index]);
    }
    int mday = g_month_week[0];
    int curday = DateTime.getParts().getMonthDay();
    g_u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    for (index = 1; index <= mday; index++)
    {
        String dStr(index);
        if (index < 10)
        {
            dStr = " " + dStr;
        }
        // 绘制当月日历
        if (index == curday)
        {
            // 红色显示当天
            g_u8g2Fonts.setForegroundColor(GxEPD_RED);

            // 当天增加下划线显示
            display.writeLine(10 + 10 + ((g_month_week[index] + 6) % 7) * CALENDAR_BLOCK_WIDTH, start_y + 2,
                              10 + 10 + ((g_month_week[index] + 6) % 7) * CALENDAR_BLOCK_WIDTH + (CALENDAR_BLOCK_WIDTH / 2), 
                              start_y + 2, GxEPD_RED);

        }
        else
        {
            g_u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        }
        g_u8g2Fonts.drawUTF8(10 + 10 + ((g_month_week[index] + 6) % 7) * CALENDAR_BLOCK_WIDTH, start_y, dStr.c_str());
        if (g_month_week[index] <= 0 && index != mday) {
            start_y += CALENDAR_BLOCK_HEIGHT;
        }
    }
    if (stask > 0)
    {
        show_todolist_impl(start_y);
    }
    else {
        show_todolist_impl(start_y);
    }
}

/**
 * 显示天气内容
 */
void show_weather_content()
{
}

/**
 * WEB配网 LCD 显示函数
 * 设备配置显示界面
 */
void show_wm_config_wnd()
{
    Serial.println("WiFi Manager Config Start!");
    // DONE: 显示配网信息
    g_u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    g_u8g2Fonts.setFont(u8g2_wqymonoweimi_12_gb2312);
    g_u8g2Fonts.drawUTF8(10, 30, "请连接热点：");
    g_u8g2Fonts.drawUTF8(10, 80, "  ecalendar");
    g_u8g2Fonts.drawUTF8(10, 130, "并访问： 192.168.4.1");
    g_u8g2Fonts.drawUTF8(10, 180, "    设置设备参数");
}

void show_wifi_manager_config_info()
{
    display.setFullWindow();
    display.clearScreen((uint8_t)GxEPD_WHITE);
    
    g_u8g2Fonts.setFontMode(1);
    g_u8g2Fonts.setFontDirection(0);
    g_u8g2Fonts.setForegroundColor(GxEPD_RED);
    g_u8g2Fonts.setBackgroundColor(GxEPD_WHITE);

    // @brief 先写文字
    display.firstPage();
    do
    {
        show_wm_config_wnd();
    }
    while (display.nextPage());
}

/**
 * 显示页面内容
 * @param [in] cname 城市名称
 */ 
void show_page(const char* cname)
{
    uint16_t show_todolist = 0;

    display.setFullWindow();
    display.clearScreen((uint8_t)GxEPD_WHITE);
    
    g_u8g2Fonts.setFontMode(1);
    g_u8g2Fonts.setFontDirection(0);
    g_u8g2Fonts.setForegroundColor(GxEPD_RED);
    g_u8g2Fonts.setBackgroundColor(GxEPD_WHITE);

    // @brief 先写文字
    display.firstPage();
    do
    {
        // @brief 头部都用一样的吧

        int nhour = DateTime.getParts().getHours();
        // 8 ~ 20 点显示待办事项
        // 否则显示日历
        if (nhour >= 8 && nhour <= 20)
        {
            show_weather_todolist(cname);
            show_todolist = 1;
            
        }
        else 
        {
            // show_month_calendar(0);
            show_month_calendar2(cname);
            show_todolist = 1;
        }

    } while (display.nextPage());

    if (show_todolist > 0)
    {
        // 显示天气图标        
        String iconName = "/64/";
        iconName.concat(g_cw.icon);
        iconName.concat(".bmp");
        // 显示彩色图片
        drawBitmapFromSpiffs_Buffered(iconName.c_str(), 225, 2, true, true, false);
    }

}

/**
 * 获取 wifi 参数
 */ 
void show_wiFi_smart_config()
{
}

void write_device_config()
{
    fs::File file;
    file = SPIFFS.open("/config.json", "w");
    if (!file) {
        Serial.println("Open Config.json failed!");
        return ;
    }
    const char* testinfo = "SPIFFS file!";
    file.seek(0, SeekMode::SeekSet);
    file.write((uint8_t*)testinfo, strlen(testinfo));
    file.close();
    Serial.println("SPIFFS file write finished!");
}

void read_device_config()
{
    fs::File file;
    file = SPIFFS.open("/config.json", "r");
    if (!file) {
        Serial.println("Open Config.json failed!");
        return ;
    }
    char rbuf[20];
    while (file.available())
    {
        memset(rbuf, 0, sizeof(rbuf));
        file.read((uint8_t*)rbuf, sizeof(rbuf));
        Serial.printf("%s\n", rbuf);
    }
    Serial.println("read device config finished!");
    file.close();
}

/**
 * 按时间段设置睡眠
 * 8 ~ 22 2小时
 * 22 ~ 8 10小时
 */
uint32_t get_sleep_duration()
{
    uint32_t ret = TIME_TO_SLEEP;
    // ESP8266 最长支持 71分钟睡眠
    // 设置为 1 小时
    int dhour = 1;
    ret *= dhour;
    return ret;
}

/**
 * 获取天气信息
 * @param [in] cityid 城市编号
 * @return
 */ 
void get_weather_info()
{
    g_hfApi.Config(HEFENG_API_KEY);

    // 获取城市天气信息
    if (strlen(g_calendarConfig._city_id.c_str())  > 0)
    {
        g_cw = g_hfApi.GetCurrentWeather(g_calendarConfig._city_id);
    }
    else 
    {
        g_cw = g_hfApi.GetCurrentWeather("101020100");
    }
}

// 使用的 电子墨水屏 型号
// epd4in2bc： 对应4.2inch e-paper B（黑白红）；
// mcu 类型  esp32 | esp8266

void init_display()
{
    display.init();

    // 设置左右镜像
    // display.mirror(true);

    // 设置显示方向 , 水平方向
    // display.setRotation(2);

    // 设置方向, 竖直方向
    display.setRotation(3);

    DISPLAY_WIDTH = display.width();
    DISPLAY_HEIGHT = display.height();

    // 字体初始化
    g_u8g2Fonts.begin(display);                  
    g_u8g2Fonts.setFontMode(1);                  
    g_u8g2Fonts.setFontDirection(0);             
    g_u8g2Fonts.setForegroundColor(GxEPD_BLACK); 
    g_u8g2Fonts.setBackgroundColor(GxEPD_WHITE); 

    Serial.println("init display ok!");
}

/**
 * 显示版本信息
 */ 
void show_version()
{
    Serial.println();
    Serial.println("------------------");
    Serial.print("Device Info: ");
    Serial.println(DEVICE_FIRMWARE_VERSION);
}

/**
 * 初始化spi接口
 */ 
void init_spi()
{
    // SPI 接口初始化
    SPI.end();
#ifdef ESP8266
    // bool SPIClass::pins(int8_t sck, int8_t miso, 
    // int8_t mosi, int8_t ss)
    SPI.pins(14, 12, 13, 15);
    SPI.begin();
#endif 
    Serial.println("SPI init ok!");
}

/**
 * 初始化 SPIFFS 
 */ 
void init_spiffs()
{
    // 初始化 SPIFFS
    if (!SPIFFS.begin())
    {
        Serial.println("SPIFFS initialisation failed!");
        int wait_index = 0;
        while (1) {
            wait_index++;
            delay(1000);
            if (wait_index >= 10)
            {
                break;
            }
            yield(); 
        }
    }
    Serial.println("\r\nSPIFFS Initialisation done.");
}

/**
 * 初始化网络
 */ 
void init_wifi()
{
#ifdef DEVICE_WIFI_MODE

#if WIFI_MANAGER_CONFIG > 0

    if (!connect_wifi_with_config())
    {
        show_wifi_manager_config_info();
        setup_wifi_by_wifi_manager();
    }

#endif // WIFI_MANAGER_CONFIG

#endif

    Serial.println("wifi init done!");
}

/**
 * 系统初始化
 */ 
void setup()
{
    // 串口初始化
    Serial.begin(115200);
    delay(1000);
    Serial.println("setup begin");

    show_version();

    init_spi();

    init_spiffs();

    // 显示初始化
    init_display();

    delay(1000);

    init_wifi();

    // 启动时设置时间
    setup_datetime();

    // 计算日历星期
    calc_month_calendar();

    delay(1000);

    // 获取天气信息
    get_weather_info();

    // DONE: 实现获取工作任务清单
    load_todo_task();

    // DONE: 实现显示内容
    g_gInfo.name = g_calendarConfig._city_name;
    if (g_gInfo.name == "") {
        g_gInfo.name = "上海";
    }
    show_page(g_gInfo.name.c_str());

    display.hibernate();

    Serial.println("ready to Sleep zzzzzZZZZZZZ~~ ~~ ~~");

#ifdef ESP8266
    Serial.println("Setup ESP8266 to sleep for " + String(get_sleep_duration()) + " Seconds");
    Serial.flush();

    delay(1000);
    WiFi.mode(WIFI_OFF);

    delay(1000);
    // 分时间段设置睡眠时间
    uint64_t dura = (get_sleep_duration() * uS_TO_S_FACTOR);
    ESP.deepSleep(dura, RF_DISABLED);
#endif 
}

/**
 * 业务循环
 */ 
void loop()
{
    delay(5000);
    Serial.println("hello loop!");
}
