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
    //////////////////
    while(cpu::guard.exchange(1)){}
    //////////////////

    assert_interrupts_disabled();
    interrupt_vector_table[TIMER] = timer_handler;
    interrupt_vector_table[IPI] = ipi_handler;
    try{
        cpu::self()->impl_ptr = new cpu::impl;
    }catch(std::bad_alloc&){
        throw;
    }
    if (user_func)
    {
        assert_interrupts_disabled();
        thread::impl *start_impl = context_init(user_func, user_arg);
        cpu::self()->impl_ptr->thread_impl_ptr = start_impl;
        //interrupt TBD
        assert_interrupts_disabled();
        setcontext(start_impl->ctx_ptr);
    }
    assert_interrupts_disabled();
    while (ready_queue.empty())
    {
        sleep_queue.push(cpu::self());
        cpu::guard.store(0);
        cpu::interrupt_enable_suspend();
        assert_interrupts_enabled();
        // raii_interrupt interrupt;
        ////////////////////////////
        cpu::interrupt_disable();
        assert_interrupts_disabled();
        while(cpu::guard.exchange(1)){}
        /////
    }
    thread::impl *next_impl = ready_queue.front();
    cpu::self()->impl_ptr->thread_impl_ptr = next_impl;
    ready_queue.pop();
    setcontext(next_impl->ctx_ptr);
}

