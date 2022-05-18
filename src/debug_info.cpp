
#include "debug_info.h"

#include <Arduino.h>

/**
 * 显示调试信息
 */
void show_debug_info(const char* info)
{
#if DEBUG_INFO > 0
    Serial.println(info);
#endif
}

