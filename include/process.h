/**
File:process.h
Function:The head file for dafination of class Process.
class Process: the abstract of the process in operation system
**/

#ifndef PROCESS_H
#define PROCESS_H

#include "../include/mutixcpu.h"
#include "../include/memorymanage.h"

#define P_STATE_READY   1
#define P_STATE_WAITE   2
#define P_STATE_DEAD    0

#define STACK_SIZE  1024



class Process
{
    friend class ProcessManage;
    friend class KernelRescue;
private:
    static int p_counter;
    static Process * current;

    long pid,father;
    long state;

    //long counter;
    long priority;
    int exit_code;

    unsigned long start_code,end_code,end_data,brk,start_stack;//maybe should in MM parts

    //unsigned short uid,euid,suid;
    //unsigned short gid,egid,sgid;

    //ldt_table ldt[3]; //maybe should in MM parts

    tss_struct tss;

    char stack[STACK_SIZE];

private:
    void ProcessInit();

    void ProcessCopy(const Process* other, const int& task_index);
    //Process(const Process& other); //bitwise copy maybe a good idea...
    //Process& operator=(const Process& other);
    //~Process();
public:
    int getPid()
    {
        return pid;
    }
    int getFather()
    {
        return father;
    }
    void setFather(int f)
    {
        father = f;
        return;
    }
    int getState()
    {
        return state;
    }
    void setState(int s)
    {
        this -> state = s;
        return ;
    }
    int getPriority()
    {
        return priority;
    }
    void setPriority(int p)
    {
        this -> priority = p;
        return ;
    }
    int getStart_code()
    {
        return start_code;
    }
    void setStart_code(int sc)
    {
        this -> start_code = sc;
        return ;
    }
    int getEnd_code()
    {
        return end_code;
    }
    void setEnd_code(int ec)
    {
        this -> end_code = ec;
        return ;
    }
    int getEnd_data()
    {
        return end_data;
    }
    void setEnd_data(int ed)
    {
        this -> end_data = ed;
        return ;
    }
    int getBrk()
    {
        return brk;
    }
    void setBrk(int brk)
    {
        this -> brk = brk;
        return ;
    }
    int getStart_stack()
    {
        return start_stack;
    }
    void setStart_stack(int start_stack)
    {
        this -> start_stack = start_stack;
        return ;
    }
    unsigned long getTSS()
    {
        return (unsigned long)&tss;
    }

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
