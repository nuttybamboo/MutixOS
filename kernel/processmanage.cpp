#include "../include/processmanage.h"

/*
#define move_to_user_mode() \
__asm__ (\
    "movl %%esp,%%eax\n\t" \
	"pushl $0x17\n\t" \
	"pushl %%eax\n\t" \
	"pushfl\n\t" \
	"pushl $0x0f\n\t" \
	"pushl $1f\n\t" \
	"iret\n" \
	"1:\tmovl $0x17,%%eax\n\t" \
	"movw %%ax,%%ds\n\t" \
	"movw %%ax,%%es\n\t" \
	"movw %%ax,%%fs\n\t" \
	"movw %%ax,%%gs" \
	:::"ax")
//*/
//#define ltr(n) __asm__("ltr %%ax"::"a" (n))
//#define lldt(n) __asm__("lldt %%ax"::"a" (n))

ProcessManage * ProcessManage::currentPM = 0;
SleepQue ProcessManage::_sleepQue[] = {SleepQue(), };

void ProcessManage::ProcessManageInit()
{
    //ctor
    currentPM = this;
    task_array = {0, };
    process_memeory_map = {0, };
    process_memeory_map[FIRST_TASK] ++;

    task_array[FIRST_TASK] = (Process*)PROCESS_MEMORY_BASE + FIRST_TASK * PAGE_SIZE;

    Process * p = (Process*)g_memory->get_address((unsigned long)task_array[FIRST_TASK]);
    p -> ProcessInit();



    MemoryManage::set_TSS_desc(FIRST_TASK, task_array[FIRST_TASK] -> getTSS());
    MemoryManage::Init_LDT_desc(FIRST_TASK);


    Process::current = task_array[FIRST_TASK];

    SystemCall::SetSystemCall(PROCESS_FORK, &fork_process);
    SystemCall::SetSystemCall(PROCESS_SLEEP, (int(*)())&sleep);
    SystemCall::SetSystemCall(PROCESS_WAKE_UP, (int(*)())&wake_up);

    //__asm__("pushfl ; andl $0xffffbfff,(%esp) ; popfl");

    g_cpu->ltr(MemoryManage::get_TSS_choice(FIRST_TASK));

    g_cpu->lldt(MemoryManage::get_LDT_choice(FIRST_TASK));


    //before this the task0's ldt must be loaded to the ldtr....
    //move_to_user_mode();

    g_cpu->to_state(USER_LEVEL);

    g_cpu->switch_to(MemoryManage::get_TSS_choice(FIRST_TASK));
    do_first_task();
}

void ProcessManage::do_first_task()
{
    SleepQue * que = sleepQueGet();

    Process * current = getCurrentTaskAddr();
    printf("i am task %d, please inter cmd:\n", current->getPid());
    char cmd = ' ';
    while(std::cin >> cmd){
        int ret = 0;
        int task_num = 0;
        int choice = 0;
        if ( 'F' == cmd ){
            ret = fork_process();
            printf("hei! fork finished! (ret = %d)\n", ret);
            if(ret != 0)
            {
                printf("fork ret none zero, i am the father task!\n");
                task_num = find_process(ret);
                choice = MemoryManage::get_TSS_choice(task_num);
                printf("my recently forked child task num is %d, choice is %d, well i'm going to sleep\n", task_num, choice);
                sleep(que);

                current = getCurrentTaskAddr();
                printf("i am task %d, i am wakeed up after sleep\n", current->getPid());
            }
            else
            {
                printf("fork ret zero, i am the child task!\n");
                current = getCurrentTaskAddr();
                wake_up(que);
                printf("i am task %d, i wake up the que! and going to sleep\n", current->getPid());
                sleep(que);
                current = getCurrentTaskAddr();
                printf("i am task %d, i am wakeed up after sleep\n", current->getPid());
            }
        }
        else if( 'S' == cmd){
            current = getCurrentTaskAddr();
            printf("i am task %d, i am going to sleep\n", current->getPid());
            sleep(que);
            current = getCurrentTaskAddr();
            printf("i am task %d, i am wakeed up after sleep\n", current->getPid());
        }
        else if ( 'W' == cmd ){
            current = getCurrentTaskAddr();
            printf("i am task %d, i am going to wake up the que\n", current->getPid());
            wake_up(que);
        }
        else if ( 'N' == cmd ){
            current = getCurrentTaskAddr();
            printf("i am task %d, i am nicing my priority!\n", current->getPid());
            current -> setPriority(current->getPriority() + 1);
        }
        else if ( 'T' == cmd ){
            KernelRescue::ShowState();
        }
        else if ( 'M' == cmd ){
            KernelRescue::ShowMMDetial();
        }
        else if ( 'P' == cmd ){
            KernelRescue::ShowPMDetial();
        }
        else if ( 'Q' == cmd || 'E' == cmd ){
            break;
        }
        else {
            printf("bad cmd!\n");
        }
        printf("\n\ni am task %d, please inter cmd:\n", current->getPid());
    }
    sleepQueRelease(que);
};

Process * ProcessManage::getCurrentTaskAddr()
{
    return getTaskAddr(Process::current);
}

Process * ProcessManage::getTaskAddrByIndex(int index)
{
    return getTaskAddr(currentPM->task_array[index]);
}

Process * ProcessManage::getTaskAddr(Process * p)
{
    return (Process*)g_memory->get_address(g_cpu->line2physical_addr((unsigned long)p));
}

int ProcessManage::find_empty_task()
{
    for(int i = 0; i < MAX_TASK_NUM; ++i)
    {
        if(currentPM -> task_array[i] == 0)
        {
            return i;
        }
    }
    return -1;
}

int ProcessManage::find_process(int pid)
{
    for(int i = 0; i < MAX_TASK_NUM; ++i)
    {
        if(currentPM -> task_array[i]
                && getTaskAddrByIndex(i)->getPid() == pid)
        {
            return i;
        }
    }
    return -1;
}

int inline ProcessManage::fork_process()
{
    int index = find_empty_task();
    if(index == -1)
    {
        return -ERROR_PROCESS_FULL;
    }
    int memory_index = find_empty_page();
    if(memory_index == -1)
    {
        return -ERROR_PM_MEMORY_FULL;
    }
    currentPM -> task_array[index] = (Process*)(PROCESS_MEMORY_BASE + memory_index * PAGE_SIZE);
    Process * p = getTaskAddrByIndex(index);
    if(!p)
    {
        printf("liner_addr_not_exist!\n");
        return -1;
    }
    Process * current = getCurrentTaskAddr();

    p -> ProcessCopy(current, index);

    MemoryManage::set_TSS_desc(index, currentPM -> task_array[index] -> getTSS());

    p -> setStart_code(
        MemoryManage::set_LDT_desc(index, find_process(current -> getPid()))
    );
    p -> setState(P_STATE_READY);

    int var = g_cpu->switch_to(MemoryManage::get_TSS_choice(index), true);

    if(0 == var)
    {
        return var;
    }
    else
    {
        return p->getPid();
    }
}

void inline ProcessManage::schedule()
{
    int cur_task = 0;

    for(int i = 0; i < MAX_TASK_NUM; ++i)
    {
        if( currentPM -> task_array[i] && getTaskAddrByIndex(i) -> getState() == P_STATE_READY)
        {
            cur_task = getTaskAddrByIndex(cur_task)->getPriority() <= getTaskAddrByIndex(i)->getPriority() ?
                       i : cur_task;
        }
    }
    switch_to(cur_task);
}

void inline ProcessManage::sleep(SleepQue* que)
{
    Process * tmp = NULL;
    if(getCurrentTaskAddr() -> getPid() != 0){
        tmp = que->QueHead;
        que->QueHead = Process::current;
        Process * current = getCurrentTaskAddr();
        current -> setState(P_STATE_WAITE);
    }
    schedule();
    //wake up the others process in the que...

    if( getCurrentTaskAddr() -> getState() == P_STATE_READY  && tmp)
    {
            getTaskAddr(tmp)-> setState(P_STATE_READY);
    }
}

void inline ProcessManage::wake_up(SleepQue* que)
{
    if(que->QueHead != 0)
    {
        getTaskAddr(que->QueHead) -> setState(P_STATE_READY);
        que->QueHead = NULL;
    }
}

inline void ProcessManage::switch_to(int next)
{
    if(Process::current == currentPM -> task_array[next])
    {
        return;
    }
    Process::current = currentPM -> task_array[next];

    g_cpu->switch_to(MemoryManage::get_TSS_choice(next));
    /*
       struct {long a,b;} __tmp;
       __asm__(
           "movw %%dx,%1\n\t"
           "ljmp *%0\n\t"
           ::"m" (*&__tmp.a),"m" (*&__tmp.b),
           "d" (MemoryManage::get_TSS_choice(next)));
    //*/
    return ;
}

int ProcessManage::kill_process(int pid)
{
    int index = find_process(pid);
    if(index == -1)
    {
        return -ERROR_PROCESS_NOT_FOUND;
    }
    Process * pr = currentPM -> task_array[index];

    for (int i=0 ; i<MAX_TASK_NUM; i++)
    {
        if (currentPM -> task_array[i] && currentPM -> task_array[i] -> getFather() == pr -> getPid())
        {
            currentPM -> task_array[i] -> setFather(1);
            //if (task[i]->state == TASK_ZOMBIE)
            /* assumption task[1] is always init */
            //(void) send_sig(SIGCHLD, task[1], 1);
        }
    }
//    MemoryManage::on_process_die(index);
//    FileSystem::on_process_die(index);
    delete pr;
    return 0;
}


int ProcessManage::find_empty_page()
{
    for(int i = 0; i < PROCESS_MEMORY_PAGES_NUM; ++i)
    {
        if(currentPM -> process_memeory_map[i] == 0)
        {
            currentPM -> process_memeory_map[i] = 1;
            return i;
        }
    }
    return -1;
}


SleepQue* ProcessManage::sleepQueGet()
{
    for(int i = 0; i < MAX_SLEEP_QUE_NUM; ++i)
    {
        if(_sleepQue[i].is_used == false)
        {
            _sleepQue[i].is_used = true;
            return &(_sleepQue[i]);
        }
    }

}

void ProcessManage::sleepQueRelease(SleepQue* que)
{
    que->is_used = false;
}
