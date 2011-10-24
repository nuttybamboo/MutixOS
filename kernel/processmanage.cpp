#include "processmanage.h"

ProcessManage::ProcessManage()
{
    //ctor
}

static void ProcessManage::schedule(){
    int cur_task = 0;
    for(int i = 0; i < MAX_TASK_NUM; ++1){
        if(task_array[i].getState() == P_STATE_READY){
            cur_task = task_array[i].getPriority() < task_array[cur_task].getPriority() ? i : cur_task;
        }
    }
    switch_to(cur_task);
}

static void ProcessManage::sleep(SleepQue& que){
    Process * tmp = que.QueHead;
    que.QueHead = Process::current;
    Process::current.setState(P_STATE_WAITE);

    schedule();

    //wake up the others process in the que...
    if(tmp){
        tmp.setState(P_STATE_READY);
    }
}

static void ProcessManage::wake_up(SleepQue& que){
    if(que.QueHead != 0){
        que.QueHead -> setState(P_STATE_READY);
        que.QueHead = 0;
    }
}
