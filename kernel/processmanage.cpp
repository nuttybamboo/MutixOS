#include "processmanage.h"

ProcessManage::ProcessManage()
{
    //ctor
}
static int ProcessManage::create_process(){
    int index = find_empty_task();
    task_array[index](Process::current, index);
    MemoryManage::set_TSS_desc(index,task_array[index].getTSS());
	MemoryManage::set_LDT_desc(index,task_array[index].getLDT());
	task_array[index].setState(P_STATE_READY);	/* do this last, just in case */
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

#define switch_to(n) {\
struct {long a,b;} __tmp; \
__asm__("cmpl %%ecx,current\n\t" \
	"je 1f\n\t" \
	"movw %%dx,%1\n\t" \
	"xchgl %%ecx,current\n\t" \
	"ljmp *%0\n\t" \
	"cmpl %%ecx,last_task_used_math\n\t" \
	"jne 1f\n\t" \
	"clts\n" \
	"1:" \
	::"m" (*&__tmp.a),"m" (*&__tmp.b), \
	"d" (MemoryMange::get_TSS_desc(n)),"c" ((long) task_array[n])); \
}
