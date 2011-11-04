#include "processmanage.h"

ProcessManage::ProcessManage()
{
    //ctor
}

static int ProcessManage::find_empty_task(){
    for(int i = 0; i < MAX_TASK_NUM; ++i){
        if(task_array[i] == 0){
            return i;
        }
    }
    return -1;
}

static int ProcessManage::find_process(int pid){
    for(int i = 0; i < MAX_TASK_NUM; ++i){
        if(task_array[i] && task_array[i]->getPid() == pid){
            return i;
        }
    }
    return -1;
}

static int ProcessManage::fork_process(){
    int index = find_empty_task();
    if(index == -1){
        return -ERROR_PROCESS_FULL;
    }
    int memory_index = find_empty_page();
    if(memory_index == -1){
        return -ERROR_PM_MEMORY_FULL;
    }
    MemoryManage::put_page(KERNEL_BASE + memory_index * PAGE_SIZE);// alloc phsical memory for the PM part..
    Process * p = task_array[index] = KERNEL_BASE + memory_index * PAGE_SIZE;
    (task_array[index])(Process::current, index);//??
    MemoryManage::set_TSS_desc(index,task_array[index] -> getTSS());
	task_array[index] -> setStart_code(
        MemoryManage::set_LDT_desc(index, find_process(Process::current -> getPID()))
    );
	task_array[index] -> setState(P_STATE_READY);
	return 	task_array[index] -> getPid();
}

static void ProcessManage::schedule(){
    int cur_task = 0;
    for(int i = 0; i < MAX_TASK_NUM; ++1){
        if(task_array[i] -> getState() == P_STATE_READY){
            cur_task = task_array[i] -> getPriority() < task_array[cur_task] -> getPriority() ? i : cur_task;
        }
    }

    switch_to(cur_task);
}

static void ProcessManage::sleep(SleepQue& que){
    Process * tmp = que.QueHead;
    que.QueHead = Process::current;
    Process::current -> setState(P_STATE_WAITE);

    schedule();

    //wake up the others process in the que...
    if(tmp){
        tmp -> setState(P_STATE_READY);
    }
}

static void ProcessManage::wake_up(SleepQue& que){
    if(que.QueHead != 0){
        que.QueHead -> setState(P_STATE_READY);
        que.QueHead = 0;
    }
}

static inline void ProcessManage::switch_to(n) {
    if(Process::current == task_array[n]){
        return;
    }
    Process::current = task_array[n];
    struct {long a,b;} __tmp;
    __asm__(
        "movw %%dx,%1\n\t"
        "ljmp *%0\n\t"
        ::"m" (*&__tmp.a),"m" (*&__tmp.b),
        "d" (MemoryMange::get_TSS_desc(n)));
    return ;
}

static void ProcessManage::kill_process(int pid){
    int index = find_process(pid);
    if(index == -1){
        return -ERROR_PROCESS_NOT_FOUND;
    }
    Process * pr = task_array[index];

    for (int i=0 ; i<MAX_TASK_NUM; i++){
		if (task_array[i] && task_array[i] -> getFather() == pr -> getPid()) {
			task_array[i] -> setFather(1);
			//if (task[i]->state == TASK_ZOMBIE)
				/* assumption task[1] is always init */
				//(void) send_sig(SIGCHLD, task[1], 1);
		}
    }
    MemoryManage::on_process_die(index);
    FileSystem::on_process_die(index);
    delete pr;
    return;
}


static int ProcessManage::find_empty_page(){
    for(int i = 0; i < PROCESS_MEMORY_PAGES_NUM; ++1){
        if(process_memeory_map[i] == 0){
            process_memeory_map[i] == 1;
            return i;
        }
    }
    return -1;
}
