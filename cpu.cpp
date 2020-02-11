#include "cpu.h"
#include "utility.h"
#include <iostream>
void cpu::init(thread_startfunc_t user_func, void * user_arg)
{
    setcontext(context_init(user_func, user_arg)->ctx_ptr);
}
