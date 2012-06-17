#include "../include/global.h"
#include "../include/kernelrescue.h"

MutixCPU* g_cpu;
MutixDisk* g_disk;
MutixInput* g_input;
MutixMemory* g_memory;//(12, 1024*3);
MutixOutput* g_output;


int main()
{
    g_cpu = new MutixCPU();
    g_disk = new MutixDisk();
    g_input = new MutixInput();
    g_memory = new MutixMemory(12, 1024*2);
    g_output = new MutixOutput();

    unsigned long __tmp;
    __asm__(
        "mov %%ebp,%0"
        ::"m" (*&__tmp));

    g_cpu->set_ss_seg_base((unsigned long)(&__tmp));
    KernelRescue kr;
};
