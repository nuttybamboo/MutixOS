#ifndef SYSTEMCALL_H
#define SYSTEMCALL_H

#define SYS_CALL_MAX_NUM    256
#define IDT_TABLE_MAX_SIZE  256



typedef void (*op_function) (void);
typedef struct desc_struct idt_desc_table[IDT_TABLE_MAX_SIZE];

class SystemCall
{
    friend class KernelRescue;
    public:
        void SystemCallInit();
        static void SetSystemCall(int system_call_number, op_function fuction);
        static void SetPageFaultTraps(op_function fuction);
    private:
        static void on_divide_error();
        static void on_debug();
        static void on_nmi();
        static void on_int3();
        static void on_overflow();
        static void on_bounds();
        static void on_invalid_op();
        static void on_device_not_available();
        static void on_double_fault();
        static void on_coprocessor_segment_overrun();
        static void on_invalid_TSS();
        static void on_segment_not_present();
        static void on_stack_segment();
        static void on_general_protection();

        static void on_reserved();
        static void on_coprocessor_error();

        static void on_irq13();
        static void on_parallel_interrupt();
        static void on_timer_interrupt();

        static void on_system_call();

        static void set_IDT(unsigned long address);
        static void inline set_gate(desc_struct * gate_addr, unsigned type, unsigned dpl, unsigned long addr);
        static void inline set_intr_gate(int index, op_function addr);
        static void inline set_trap_gate(int index, op_function addr);
        static void inline set_system_gate(int index, op_function addr);
    private:
        idt_desc_table  idt;
        op_function system_call_table[SYS_CALL_MAX_NUM];
        static SystemCall * currentSCI;
};

#endif // SYSTEMCALL_H
