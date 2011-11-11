#ifndef CONFIG_H
#define CONFIG_H

#define MAX_TASK_NUM    255
#define FIRST_TASK  0

#include "../include/kernelrescue.h"
#include "../include/process.h"
#include "../include/processmanage.h"
#include "../include/memorymanage.h"
#include "../include/filesystem.h"
#include "../include/systemcall.h"
#include "../include/interprocesscommu.h"

#define sti() __asm__ ("sti"::)
#define cli() __asm__ ("cli"::)
#define nop() __asm__ ("nop"::)
#define iret() __asm__ ("iret"::)

#endif // CONFIG_H
