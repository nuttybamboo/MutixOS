#include "../include/config.h"

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

#define ltr(n) __asm__("ltr %%ax"::"a" ( *&n ))
#define lldt(n) __asm__("lldt %%ax"::"a" ( *&n))

ProcessManage * ProcessManage::currentPM = 0;

void ProcessManage::ProcessManageInit()
{
    //ctor
    currentPM = this;
    task_array = {0, };
    process_memeory_map = {0, };
    process_memeory_map[FIRST_TASK] ++;
    MemoryManage::put_page(PROCESS_MEMORY_BASE + FIRST_TASK * PAGE_SIZE);
    Process * p = task_array[FIRST_TASK] = (Process*)(PROCESS_MEMORY_BASE + FIRST_TASK * PAGE_SIZE);
    p -> ProcessInit();
    MemoryManage::set_TSS_desc(FIRST_TASK, p -> getTSS());
    MemoryManage::Init_LDT_desc(FIRST_TASK);
    Process::current = task_array[FIRST_TASK];

    //__asm__("pushfl ; andl $0xffffbfff,(%esp) ; popfl");
    ltr(MemoryManage::get_TSS_choice(FIRST_TASK));
    lldt(MemoryManage::get_LDT_choice(FIRST_TASK));
    //before this the task0's ldt must be loaded to the ldtr....
    move_to_user_mode();
    //switch_to(FIRST_TASK);
}

Process * ProcessManage::getCurrent(){
    return Process::current;
}

int ProcessManage::find_empty_task(){
    for(int i = 0; i < MAX_TASK_NUM; ++i){
        if(currentPM -> task_array[i] == 0){
            return i;
        }
    }
    return -1;
}

int ProcessManage::find_process(int pid){
    for(int i = 0; i < MAX_TASK_NUM; ++i){
        if(currentPM -> task_array[i] && currentPM -> task_array[i]->getPid() == pid){
            return i;
        }
    }
    return -1;
}

int ProcessManage::fork_process(){
    int index = find_empty_task();
    if(index == -1){
        return -ERROR_PROCESS_FULL;
    }
    int memory_index = find_empty_page();
    if(memory_index == -1){
        return -ERROR_PM_MEMORY_FULL;
    }
    MemoryManage::put_page(PROCESS_MEMORY_BASE + memory_index * PAGE_SIZE);// alloc phsical memory for the PM part..
    Process * p = currentPM -> task_array[index] = (Process*)(PROCESS_MEMORY_BASE + memory_index * PAGE_SIZE);
    p -> ProcessCopy(*Process::current, index);//??
    MemoryManage::set_TSS_desc(index, p -> getTSS());
	p -> setStart_code(
        MemoryManage::set_LDT_desc(index, find_process(Process::current -> getPid()))
    );
	p -> setState(P_STATE_READY);
	return 	p -> getPid();
}

void ProcessManage::schedule(){
    int cur_task = 0;
    for(int i = 0; i < MAX_TASK_NUM; ++i){
        if(currentPM -> task_array[i] -> getState() == P_STATE_READY){
            cur_task = currentPM -> task_array[i] -> getPriority() <
                currentPM -> task_array[cur_task] -> getPriority() ?
                i : cur_task;
        }
    }

    switch_to(cur_task);
}

void ProcessManage::sleep(SleepQue& que){
    Process * tmp = que.QueHead;
    que.QueHead = Process::current;
    Process::current -> setState(P_STATE_WAITE);

    schedule();

    //wake up the others process in the que...
    if(tmp){
        tmp -> setState(P_STATE_READY);
    }
}

void ProcessManage::wake_up(SleepQue& que){
    if(que.QueHead != 0){
        que.QueHead -> setState(P_STATE_READY);
        que.QueHead = 0;
    }
}

inline void ProcessManage::switch_to(int next) {
    if(Process::current == currentPM -> task_array[next]){
        return;
    }
    Process::current = currentPM -> task_array[next];
    struct {long a,b;} __tmp;
    __asm__(
        "movw %%dx,%1\n\t"
        "ljmp *%0\n\t"
        ::"m" (*&__tmp.a),"m" (*&__tmp.b),
        "d" (*&MemoryManage::get_TSS_choice(next)));
    return ;
}

int ProcessManage::kill_process(int pid){
    int index = find_process(pid);
    if(index == -1){
        return -ERROR_PROCESS_NOT_FOUND;
    }
    Process * pr = currentPM -> task_array[index];

    for (int i=0 ; i<MAX_TASK_NUM; i++){
		if (currentPM -> task_array[i] && currentPM -> task_array[i] -> getFather() == pr -> getPid()) {
			currentPM -> task_array[i] -> setFather(1);
			//if (task[i]->state == TASK_ZOMBIE)
				/* assumption task[1] is always init */
				//(void) send_sig(SIGCHLD, task[1], 1);
		}
    }
    MemoryManage::on_process_die(index);
    FileSystem::on_process_die(index);
    delete pr;
    return 0;
}


int ProcessManage::find_empty_page(){
    for(int i = 0; i < PROCESS_MEMORY_PAGES_NUM; ++i){
        if(currentPM -> process_memeory_map[i] == 0){
            currentPM -> process_memeory_map[i] = 1;
            return i;
        }
    }
    return -1;
}
