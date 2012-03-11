#ifndef PROCESSMANAGE_H
#define PROCESSMANAGE_H
#include "../include/config.h"
#include "../include/process.h"
//#include "../include/memorymanage.h"
#include "../include/systemcall.h"

#define TSS_ITEM    0
#define LDT_ITEM    1

#define PROCESS_MEMORY_PAGES_NUM    255

#define PROCESS_MEMORY_BASE 0x100000

#define ERROR_PROCESS_FULL  1
#define ERROR_PM_MEMORY_FULL    2
#define ERROR_PROCESS_NOT_FOUND 3

class Process;

class SleepQue{
    SleepQue(){
        QueHead = NULL;
    }
    friend class ProcessManage;
    private:
    Process * QueHead;
};


class ProcessManage
{
    friend class KernelRescue;
    private:
        void ProcessManageInit();
        static void switch_to(int next);
        static int find_empty_task();
        static int find_empty_page();
        static int find_process(int pid);
        static void do_first_task();
    public:
        static Process * getCurrent();
        static int fork_process();
        static void schedule();
        static void sleep(SleepQue* que);  //the  que was modedied by the function..so we should use &
        static void wake_up(SleepQue* que);
        static int kill_process(int pid);
        static void wait();
        static void wait_pid();
    private:
        Process * task_array[MAX_TASK_NUM];
        char process_memeory_map[PROCESS_MEMORY_PAGES_NUM];
        static ProcessManage * currentPM;
};

#endif // PROCESSMANAGE_H
