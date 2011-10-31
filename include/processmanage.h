#ifndef PROCESSMANAGE_H
#define PROCESSMANAGE_H
#define MAX_TASK_NUM    255
#define TSS_ITEM    0
#define LDT_ITEM    1


class ProcessManage
{
    private:
        ProcessManage();
        switch_to();
        static Process * getCurrent(){
            return Process::current;
        }
        static int find_empty_task();
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
};

class SleepQue{
    Process * QueHead;
}

#endif // PROCESSMANAGE_H
