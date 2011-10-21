#ifndef PROCESSMANAGE_H
#define PROCESSMANAGE_H
#define MAX_TASK_NUM    255

class ProcessManage
{
    private:
        ProcessManage();
    protected:
        static void schedule();
        static void sleep();
        static void wake_up();
        static void wait();
        static void wait_pid();
    private:
        static Process task_array[MAX_TASK_NUM];
        static int p_task;
};

#endif // PROCESSMANAGE_H
