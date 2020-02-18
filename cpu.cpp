#include "cpu.h"
#include "cpu_impl.h"
#include "utility.h"
#include <cassert>
#include <stdexcept>
void timer_handler()
{
    thread::yield();
}

void ipi_handler()
{
    // assert(!ready_queue.empty());
    return;
}

void cpu::init(thread_startfunc_t user_func, void *user_arg)
{
    //////////////////
    while (cpu::guard.exchange(1))
    {
    }
    //////////////////

    assert_interrupts_disabled();
    interrupt_vector_table[TIMER] = timer_handler;
    interrupt_vector_table[IPI] = ipi_handler;
    try
    {
        cpu::self()->impl_ptr = new cpu::impl;
    }
    catch (std::bad_alloc &)
    {
        throw;
    }
    if (user_func)
    {
        assert_interrupts_disabled();
        thread::impl *start_impl = context_init((thread_startfunc_t)wrapper, user_func, user_arg);
        cpu::self()->impl_ptr->thread_impl_ptr = start_impl;
        //interrupt TBD
        assert_interrupts_disabled();
        setcontext(start_impl->ctx_ptr);
    }
    else
    {
        thread::impl *tool_thread = context_init((thread_startfunc_t)suspend_helper, (thread_startfunc_t) nullptr, (void *)nullptr);
        cpu::self()->impl_ptr->thread_impl_ptr = tool_thread;
        setcontext(tool_thread->ctx_ptr);
    }
}
