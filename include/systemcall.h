#ifndef SYSTEMCALL_H
#define SYSTEMCALL_H

#define SYS_CALL_MAX_NUM    127

typedef int (*syscall_op_function) (void);

class SystemCall
{
    friend class KernelRescue;
    public:
        void SystemCallInit();
        void SetSystemCall(int system_call_number, syscall_op_function fuction);
    private:
        void on_system_call();
    private:
        syscall_op_function system_call_table[SYS_CALL_MAX_NUM];
        static SystemCall * currentSCI;
};

#endif // SYSTEMCALL_H
