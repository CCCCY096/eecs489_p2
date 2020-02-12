#include "cpu.h"
#include "cpu_impl.h"
#include "utility.h"
#include <iostream>

void timer_handler()
{
    thread::yield();
}

void ipi_handler()
{
    return;
}

void cpu::init(thread_startfunc_t user_func, void *user_arg)
{
    interrupt_vector_table[TIMER] = timer_handler;
    interrupt_vector_table[IPI] = ipi_handler;
    cpu::self()->impl_ptr = new cpu::impl;
    if (user_func)
    {
        thread::impl *start_impl = context_init(user_func, user_arg);
        cpu::self()->impl_ptr->thread_impl_ptr = start_impl;
        //interrupt TBD
        setcontext(start_impl->ctx_ptr);
    }
}
