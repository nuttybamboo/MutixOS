#include "../include/systemcall.h"

SystemCall * SystemCall::currentSCI = 0;

SystemCall::SystemCall()
{
    //ctor
    currentSCI = this;
    system_call_table = {0, };
    //traps and interaption setting...
}
