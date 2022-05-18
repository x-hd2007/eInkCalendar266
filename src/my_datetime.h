
#ifndef MY_DATE_TIME_H
#define MY_DATE_TIME_H   1

// 一天描述
#define DAY_SECONDS (3600 * 24)

const char* g_WEEKDAY_CN[] = {"周日", "周一", "周二", "周三", "周四", "周五", "周六"};
// const char* g_WEEKDAY_CN[] = {"日", "一", "二", "三", "四", "五", "六"};
const char* g_WEEKDAY_EN[] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
// const char *g_MONTH_CN[] = {"一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "十一月", "十二月"};
const char *g_MONTH_EN[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

// 记录当月星期
uint8_t g_month_week[35];

/**
 * @param [in]
 * @param [in]
 * @return
 */
int get_month_days(int year, int month)
{
    int days = 0;
    switch(month)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        days = 31;
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        days = 30;
        break;
    }
        
    if(((year % 100 != 0 && year % 4 == 0) || year % 400 == 0)) //闰年
    {
        if(month == 2)
        {
            days = 29;
        }
    }
    else//不是闰年
    {
        if(month == 2)
        {
            days = 28;
        }
    }

    // Serial.printf("Month days: %d-%d %d\n", year, month, days);
    
    return days;
}

/**
 * 显示计算的当月星期日历
 * @return 
 */
void print_month_days()
{
    int mdays = g_month_week[0];
    Serial.printf("month days: %d\n", mdays);
    Serial.println("Mo Tu We Th Fr Sa Su");
    int index = 0;
    for (index = 1; index <= mdays; index++)
    {
        uint8_t weekday = g_month_week[index];
        uint8_t poffset = 0;
        while (poffset < weekday)
        {
            Serial.print("   ");
            poffset++;
        }
        Serial.printf("%d ", index);
        if (weekday == 6)
        {
            Serial.println();
        }
    }
    Serial.println();
}

/**
 * @return 
 */
void show_month_days()
{
    int mdays = g_month_week[0];
    int index = 0;
    for (index = 1; index <= mdays; index++)
    {
        Serial.printf("%d ", g_month_week[index]);
        if (g_month_week[index] >= 6)
        {
            Serial.println();
        }
    }
}

/**
 * 根据今天, 星期天数, 计算本月某天 week day
 * 周日开始 为 0
 */
int calc_week_day(int today, int wtoday, int mday)
{
    int ret = 0;
    ret = today - mday;
    ret = ret % 7;
    ret = 7 - ret;
    ret = (wtoday + ret) % 7;
    return ret;
}

/**
 * 设置时间日期
 */
void setup_datetime()
{
#if DEBUG_INFO > 0
    Serial.println("begin config time!");
#endif

    DateTime.setTimeZone(8);
    DateTime.setServer("time1.aliyun.com");
    DateTime.begin(5000);
    if (!DateTime.isTimeValid())
    {
        DateTime.setTime(1574870400 + 3600);
        Serial.println("Failed to get time from server.");
    }
    String nowStr = DateTime.toString();
    Serial.println(nowStr.c_str());

#if DEBUG_INFO > 0
    Serial.println("set datetime ok!");
#endif 

}

/**
 * @param [in]
 * @return 
 */
void calc_month_calendar()
{
    memset(g_month_week, 0, sizeof(g_month_week));

    int mday = DateTime.getParts().getMonthDay();
    int wday = DateTime.getParts().getWeekDay();

    int mdays = get_month_days(DateTime.getParts().getYear(),
                               DateTime.getParts().getMonth() + 1);

    int index = 0;
    g_month_week[0] = (uint8_t)mdays;
    for (index = 1; index <= mdays; index++)
    {
        g_month_week[index] = (uint8_t)calc_week_day(mday, wday, index);
    }
    
}

/**
 * 更新系统时间
 */
void update_datetime(unsigned long newtime)
{
    DateTime.setTime(newtime);
    String nowStr = DateTime.toString();
    Serial.println();
    Serial.println(nowStr.c_str());
    calc_month_calendar();

#if DEBUG_INFO > 0
    print_month_days();
    show_month_days();
#endif
    
}

#endif // MY_DATE_TIME_H   1
