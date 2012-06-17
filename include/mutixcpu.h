#ifndef MUTIXCPU_H
#define MUTIXCPU_H

#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <map>

#include "mutixmemory.h"

#define BIT_OF_REGISTER 32

struct cpu_struct
{
    unsigned long   tr;
    unsigned long    cr2;
    unsigned long    idt;
    unsigned long    gdt;
    unsigned long	cr3;
    unsigned long	eip;
    unsigned long	eflags;
    unsigned long	eax,ecx,edx,ebx;
    unsigned long	esp;
    unsigned long	ebp;
    unsigned long	esi;
    unsigned long	edi;
    unsigned long	es;
    unsigned long	cs;
    unsigned long	ss;
    unsigned long	ds;
    unsigned long	fs;
    unsigned long	gs;
    unsigned long	ldt;
};
typedef void (*op_function) (void);
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

typedef struct _tss_struct
{
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
} tss_struct;

enum DescType
{
    DESC_TYPE_LDT, DESC_TYPE_TSS, DESC_TYPE_CALL_GATE,
    DESC_TYPE_INTER_GATE, DESC_TYPE_TRAP_GATE, DESC_TYPE_TASK_GATE,
    DESC_TYPE_CODE_SEG, DESC_TYPE_DATA_SEG, DESC_TYPE_NOT_IN_MEM, DESC_TYPE_ILLEGAL
};
enum CPUState
{
    RESCUE_LEVEL = 0, KERNEL_LEVEL = 1, KERNEL_IO_LEVEL = 2, USER_LEVEL = 3
};

typedef struct _choice
{
    long a, b;
} Desc;

typedef struct _jmp_point
{
    jmp_buf point;
} JumpPoint;

struct stack_mirror
{
    unsigned long esp;
    int size;
    char * pointer;
};

class MutixCPU
{
    friend class ProcessManage;
    friend class MemoryManage;
    friend class SystemCall;
public:
    MutixCPU();
    virtual ~MutixCPU();
    void set_ss_seg_base(unsigned seg_base);
protected:
private:

    void push(long l);
    long pop();


    long line2physical_addr(unsigned long line_addr);

    CPUState get_state();

    void to_state(CPUState state);

    long get_cr3();
    long	get_eip();
    long	get_eflags();
    long	get_eax();
    long    get_ecx();
    long    get_edx();
    long    get_ebx();
    long	get_esp();
    long	get_ebp();
    long	get_esi();
    long	get_edi();
    long	get_es();
    long	get_cs();
    long	get_ss();
    long	get_ds();
    long	get_fs();
    long	get_gs();
    long	get_ldt();

    void    set_cr2(long cr2);
    void    set_cr3(long cr3);
    void	set_eip(long eip);
    void	set_eflags(long eflags);
    void	set_eax(long eax);
    void    set_ecx(long ecx);
    void    set_edx(long edx);
    void    set_ebx(long ebx);
    void	set_esp(long esp);
    void	set_ebp(long ebp);
    void	set_esi(long esi);
    void	set_edi(long edi);
    void	set_es(long es);
    void	set_cs(long cs);
    void	set_ss(long ss);
    void	set_ds(long ds);
    void	set_fs(long fs);
    void	set_gs(long gs);
    void	lldt(long ldt);
    void    lidt(long idt);
    void    lgdt(long gdt);
    unsigned long   get_gdt();

    void str();
    unsigned long get_tr();
    void set_tr(const tss_struct* tss);
    bool is_32bit_gate(const Desc* desc);
    bool is_besy_tss(const Desc* desc);
    DescType check_type(const Desc* desc);
    unsigned long get_base_from_desc(const Desc* desc);
    unsigned long get_limit_frome_desc(const Desc* desc);

    const tss_struct * get_tss(const Desc* tss_desc);
    void load_tss(const tss_struct* tss);
    void ltr(int tss_choice);


    int inline restore_stack(int index);
    int inline switch_to(int tss_choice, bool is_new_task = false);
    void switch_to_first_task(int tss_choice);
    unsigned long get_offset_from_gate(const Desc* gate);
    int get_choice_from_gate(const Desc* gate);
    const Desc* get_desc_from_choice(int choice);
    void deal_with_task(const Desc* gate);
    void deal_with_inter(const Desc* gate);
    void deal_with_trap(const Desc* gate);
    void deal_with_call(const Desc* gate);
    void deal_with_gate(const Desc* gate);
public:
    jmp_buf jmp_env[256];
private:
    unsigned long ss_seg_base;
    stack_mirror stack_mirror_arr[256];
    struct cpu_struct _cpu;
    CPUState _state;
    static int cur_task;
};

#endif // MUTIXCPU_H
