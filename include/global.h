#ifndef GLOBAL_H
#define GLOBAL_H

#include <iostream>
#include <stdio.h>
#include "mutixmemory.h"
#include "mutixcpu.h"
#include "mutixdisk.h"

#include "mutixoutput.h"
#include "mutixinput.h"

extern MutixMemory* g_memory;
extern MutixCPU* g_cpu;
extern MutixDisk* g_disk;
extern MutixInput* g_input;

extern MutixOutput* g_output;


#endif   //GLOBAL_H
