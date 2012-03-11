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

void ProcessManage::ProcessManageInit()
{
    //ctor
    currentPM = this;
    task_array = {0, };
    process_memeory_map = {0, };
    process_memeory_map[FIRST_TASK] ++;
    MemoryManage::put_page(PROCESS_MEMORY_BASE + FIRST_TASK * PAGE_SIZE);

    task_array[FIRST_TASK] = (Process*)PROCESS_MEMORY_BASE + FIRST_TASK * PAGE_SIZE;
    Process * p = (Process*)g_cpu.line2physical_addr((unsigned long)task_array[FIRST_TASK]);
    p -> ProcessInit();
    printf("Process inited, addr is %d!\n", p);

    printf("first task tss liner addr is %d seted to gdt\n", task_array[FIRST_TASK] -> getTSS());
    MemoryManage::set_TSS_desc(FIRST_TASK, task_array[FIRST_TASK] -> getTSS());
    MemoryManage::Init_LDT_desc(FIRST_TASK);
    printf("first task inited!\n");

    Process::current = task_array[FIRST_TASK];

    SystemCall::SetSystemCall(PROCESS_FORK, &fork_process);
    SystemCall::SetSystemCall(PROCESS_SLEEP, (int(*)())&sleep);
    SystemCall::SetSystemCall(PROCESS_WAKE_UP, (int(*)())&wake_up);
    printf("systemcall seted!\n");

    //__asm__("pushfl ; andl $0xffffbfff,(%esp) ; popfl");
    printf("first task tss choice is %d \n", MemoryManage::get_TSS_choice(FIRST_TASK));
    g_cpu.ltr(MemoryManage::get_TSS_choice(FIRST_TASK));
    printf("first task ltr loaded!\n");
    g_cpu.lldt(MemoryManage::get_LDT_choice(FIRST_TASK));
    printf("first task ldt loaded!\n");

    //before this the task0's ldt must be loaded to the ldtr....
    //move_to_user_mode();
    g_cpu.to_state(USER_LEVEL);
    printf("setting %d\n", MemoryManage::get_TSS_choice(FIRST_TASK) >> 3);
    int var = setjmp(g_cpu.jmp_env[MemoryManage::get_TSS_choice(FIRST_TASK) >> 3]);
    if(0 != var){
        printf("do task 1\n");
        do_first_task();
    }
    else{
        printf("going to task 1\n");
        g_cpu.switch_to_first_task(MemoryManage::get_TSS_choice(FIRST_TASK));
    }
}

void ProcessManage::do_first_task(){
    SleepQue que;
    printf("i am first task\n");
    int ret = 0;
    ret = fork_process();
    printf("hei! why this?(ret = %d)", ret);
    if(ret != 0){
        int task_num = find_process(ret);
        int choice = MemoryManage::get_TSS_choice(task_num);
        printf("task num is %d, choice is %d\n", task_num, choice);

        sleep(&que);
    }

    Process * current = (Process*)g_cpu.line2physical_addr((unsigned long)Process::current);
    printf("i am task %d\n", current->getPid());
    wake_up(&que);
    current = (Process*)g_cpu.line2physical_addr((unsigned long)Process::current);
    printf("i am task %d\n", current->getPid());
    sleep(&que);
    current = (Process*)g_cpu.line2physical_addr((unsigned long)Process::current);
    printf("i am task %d\n", current->getPid());
};

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
        if(currentPM -> task_array[i] && ((Process*)g_cpu.line2physical_addr((unsigned long)currentPM->task_array[i]))->getPid() == pid){
            printf("found process id %d, task _num %d\n", pid, i);
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
    currentPM -> task_array[index] = (Process*)(PROCESS_MEMORY_BASE + memory_index * PAGE_SIZE);
    Process * p = (Process*)g_cpu.line2physical_addr((unsigned long)currentPM->task_array[index]);
    if(!p){
        printf("liner_addr_not_exist!\n");
        return -1;
    }
    Process * current = (Process*)g_cpu.line2physical_addr((unsigned long)Process::current);
    printf("before copy current addr is %d, p addr is %d\n", current, p);
    p -> ProcessCopy(current, index);//??
    printf("end copy\n");
    MemoryManage::set_TSS_desc(index, currentPM -> task_array[index] -> getTSS());
    printf("end tss set\n");
	p -> setStart_code(
        MemoryManage::set_LDT_desc(index, find_process(current -> getPid()))
    );
    printf("before state\n");
	p -> setState(P_STATE_READY);
	printf("setting begin point of %d\n", MemoryManage::get_TSS_choice(index) >> 3);
	int var = setjmp(g_cpu.jmp_env[MemoryManage::get_TSS_choice(index) >> 3]);
	if(var == 0){
        return 	p -> getPid();
	}
	else{
	    printf("i am switched here, returning 0!\n");
	    return 0;
	}
}

void ProcessManage::schedule(){
    int cur_task = 0;

    for(int i = 0; i < MAX_TASK_NUM; ++i){
        if( currentPM -> task_array[i] &&
           ((Process*)g_cpu.line2physical_addr((unsigned long)currentPM -> task_array[i]))
                -> getState() == P_STATE_READY)
        {
            printf("task %d was ready\n", i);
            cur_task = ((Process*)g_cpu.line2physical_addr((unsigned long)currentPM -> task_array[cur_task]))
                -> getPriority() <
            ((Process*)g_cpu.line2physical_addr((unsigned long)currentPM -> task_array[i]))
                -> getPriority() ?
                i : cur_task;
        }
    }
    printf("schedule to %d\n", cur_task);
    switch_to(cur_task);
}

void ProcessManage::sleep(SleepQue* que){
    Process * tmp = que->QueHead;
    que->QueHead = Process::current;
    Process * current = (Process*)g_cpu.line2physical_addr((unsigned long)Process::current);
    current -> setState(P_STATE_WAITE);

    schedule();

    //wake up the others process in the que...
    if(tmp){
        ((Process*)g_cpu.line2physical_addr((unsigned long)tmp))-> setState(P_STATE_READY);
    }
}

void ProcessManage::wake_up(SleepQue* que){
    if(que->QueHead != 0){
       ((Process*)g_cpu.line2physical_addr((unsigned long) que->QueHead)) -> setState(P_STATE_READY);
        que->QueHead = NULL;
    }
}

inline void ProcessManage::switch_to(int next) {
    if(Process::current == currentPM -> task_array[next]){
        printf("do  no switch\n");
        return;
    }
    Process::current = currentPM -> task_array[next];

    g_cpu.switch_to(MemoryManage::get_TSS_choice(next));
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
//    MemoryManage::on_process_die(index);
//    FileSystem::on_process_die(index);
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
