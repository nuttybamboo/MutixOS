#ifndef PROCESSMANAGE_H
#define PROCESSMANAGE_H

#define TSS_ITEM    0
#define LDT_ITEM    1

#define PROCESS_MEMORY_PAGES_NUM    255

#define KERNEL_BASE 0x100000

#define ERROR_PROCESS_FULL  1
#define ERROR_PM_MEMORY_FULL    2
#define ERROR_PROCESS_NOT_FOUND 3


class ProcessManage
{
    private:
        ProcessManage();
        switch_to();
        static Process * getCurrent(){
            return Process::current;
        }
        static int find_empty_task();
        static int find_empty_page();
        static int find_process(int pid);
    protected:
        static int fork_process();
        static void schedule();
        static void sleep(SleepQue& que);  //the  que was modedied by the function..so we should use &
        static void wake_up(SleepQue& que);
        static void kill_process();
        static void wait();
        static void wait_pid();
    private:
        static Process * task_array[MAX_TASK_NUM];
        static int p_counter;
        static char process_memeory_map[PROCESS_MEMORY_PAGES_NUM];
};

class SleepQue{
    Process * QueHead;
}

#endif // PROCESSMANAGE_H
