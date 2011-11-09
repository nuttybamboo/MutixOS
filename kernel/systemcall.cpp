#include "../include/config.h"

SystemCall * SystemCall::currentSCI = 0;

void SystemCall::SystemCallInit()
{
    //ctor
    currentSCI = this;
    system_call_table = {0, };
    //traps and interaption setting...
}
