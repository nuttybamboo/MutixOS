/**
File:process.h
Function:The head file for dafination of class Process.
class Process: the abstract of the process in operation system
**/

#ifndef PROCESS_H
#define PROCESS_H

#define P_STATE_READY   1
#define P_STATE_WAITE   2
#define P_STATE_DEAD    0

#define STACK_SIZE  1024

/*
struct i387_struct {
	long	cwd;
	long	swd;
	long	twd;
	long	fip;
	long	fcs;
	long	foo;
	long	fos;
	long	st_space[20];//	/* 8*10 bytes for each FP-reg = 80 bytes */  /*
};
//*/
typedef struct desc_struct {
	unsigned long a,b;
};

struct tss_struct {
	long	back_link;	/* 16 high bits zero */
	long	esp0;
	long	ss0;		/* 16 high bits zero */
	long	esp1;
	long	ss1;		/* 16 high bits zero */
	long	esp2;
	long	ss2;		/* 16 high bits zero */
	long	cr3;
	long	eip;
	long	eflags;
	long	eax,ecx,edx,ebx;
	long	esp;
	long	ebp;
	long	esi;
	long	edi;
	long	es;		/* 16 high bits zero */
	long	cs;		/* 16 high bits zero */
	long	ss;		/* 16 high bits zero */
	long	ds;		/* 16 high bits zero */
	long	fs;		/* 16 high bits zero */
	long	gs;		/* 16 high bits zero */
	long	ldt;		/* 16 high bits zero */
	long	trace_bitmap;	/* bits: trace 0, bitmap 16-31 */
	//struct i387_struct i387;
};


class Process
{
    friend:
    ProcessManage::getCurrent();
    protected:
        static Process * current;
    private:

        long pid,father;
        long state;

        //long counter;
        long priority;
        int exit_code;

        unsigned long start_code,end_code,end_data,brk,start_stack;

        //unsigned short uid,euid,suid;
        //unsigned short gid,egid,sgid;

        struct desc_struct ldt[3];

        struct tss_struct tss;

        char[STACK_SIZE] stack;

    protected:
        Process();

        Process(const Process& other, int task_index);
        //Process(const Process& other); //bitwise copy maybe a good idea...
        //Process& operator=(const Process& other);
        ~Process();

        int getPid(){
            return pid;
        }
        int getFather(){
            return father;
        }
        int getState(){
            return state;
        }
        void setState(int s){
            this -> state = s;
            return ;
        }
        int getPriority(){
            return priority;
        }
        void setPriority(int p){
            this -> priority = p;
            return ;
        }
        int getStart_code(){
            return start_code;
        }
        void setStart_code(int sc){
            this -> start_code = sc;
            return ;
        }
        int getEnd_code(){
            return end_code;
        }
        void setEnd_code(int ec){
            this -> end_code = ec;
            return ;
        }
        int getEnd_data(){
            return end_data;
        }
        void setEnd_data(int ed){
            this -> end_data = ed;
            return ;
        }
        int getBrk(){
            return brk;
        }
        void setBrk(int brk){
            this -> brk = brk;
            return ;
        }
        int getStart_stack(){
            return start_stack;
        }
        void setStart_stack(int start_stack){
            this -> start_stack = start_stack;
            return ;
        }
        unsigned long getTSS(){
            return &tss;
        }
        unsigned long getLDT(){
            return &ldt;
        }
    private:
};

#endif // PROCESS_H





/*
struct task_struct {
// /* these are hardcoded - don't touch */   /*
	long state;	// /* -1 unrunnable, 0 runnable, >0 stopped */  /*
	long counter;
	long priority;
	long signal;
	struct sigaction sigaction[32];
	long blocked; //	/* bitmap of masked signals */ /*
// /* various fields */ /*
	int exit_code;
	unsigned long start_code,end_code,end_data,brk,start_stack;
	long pid,father,pgrp,session,leader;
	unsigned short uid,euid,suid;
	unsigned short gid,egid,sgid;
	long alarm;
	long utime,stime,cutime,cstime,start_time;
	unsigned short used_math;
// /* file system info */ /*
	int tty;		// /* -1 if no tty, so it must be signed */ /*
	unsigned short umask;
	struct m_inode * pwd;
	struct m_inode * root;
	struct m_inode * executable;
	unsigned long close_on_exec;
	struct file * filp[NR_OPEN];
// /* ldt for this task 0 - zero 1 - cs 2 - ds&ss */   /*
	struct desc_struct ldt[3];
// /* tss for this task */  /*
	struct tss_struct tss;
};
//*/
