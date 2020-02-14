#include "cpu.h"
#include "cpu_impl.h"
#include "utility.h"
#include <stdexcept>
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
    try{
        cpu::self()->impl_ptr = new cpu::impl;
    }catch(std::bad_alloc&){
        throw;
    }
    if (user_func)
    {
        thread::impl *start_impl = context_init(user_func, user_arg);
        cpu::self()->impl_ptr->thread_impl_ptr = start_impl;
        //interrupt TBD
        setcontext(start_impl->ctx_ptr);
    }
    while (ready_queue.empty())
    {
        sleep_queue.push(cpu::self());
        cpu::interrupt_enable_suspend();
    }
    thread::impl *next_impl = ready_queue.front();
    cpu::self()->impl_ptr->thread_impl_ptr = next_impl;
    ready_queue.pop();
    setcontext(next_impl->ctx_ptr);
}

