
#ifndef _MY_TASK_H_
#define _MY_TASK_H_  1

// 工作任务清单

struct my_task
{
    String m_taskInfo;

    String m_taskTag;

    uint8_t m_priority;

    time_t m_beginTime;

    time_t m_endTime;
    
};

extern String g_userName;

// 显示农历日期
extern String g_lunarYear;
extern String g_lunarDay;

// Todo Task 相关函数
extern void show_task_list();
extern void parse_my_task_list(String& );
extern int get_task_count();
extern my_task* get_task_info(int tid);
extern void load_todo_task();

#endif // _MY_TASK_H

