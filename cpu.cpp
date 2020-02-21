#include <cassert>
#include <stdexcept>
#include "cpu.h"
#include "cpu_impl.h"
#include "utility.h"

/* Timer handler */
void timer_handler()
{
    thread::yield();
}

/* IPI handler */
void ipi_handler()
{
    // assert(!ready_queue.empty());
    return;
}


/* Initialize a cpu */
void cpu::init(thread_startfunc_t user_func, void *user_arg)
{
    // To protect the following critical section
    while (cpu::guard.exchange(1)) {}
    assert_interrupts_disabled();
    // Connect handlers
    interrupt_vector_table[TIMER] = timer_handler;
    interrupt_vector_table[IPI] = ipi_handler;
    try
    {
        // Make cpu remember which thread it runs on
        cpu::self()->impl_ptr = new cpu::impl;
    }
    catch (std::bad_alloc &)
    {
        throw;
    }
    // If cpu has a thread to run on, then make context and swap to it
    if (user_func)
    {
        assert_interrupts_disabled();
        thread::impl *start_impl = context_init((thread_startfunc_t)wrapper, user_func, user_arg);
        cpu::self()->impl_ptr->thread_impl_ptr = start_impl;
        assert_interrupts_disabled();
        setcontext(start_impl->ctx_ptr);
    }
    // Else if it doesn't, then it should wait for available thread in ready queue
    else
    {
        // Tool thread runs suspend_helper functions: cpu suspends on it until there is available threads to run
        thread::impl *tool_thread = context_init((thread_startfunc_t)suspend_helper, (thread_startfunc_t) nullptr, (void *)nullptr);
        cpu::self()->impl_ptr->thread_impl_ptr = tool_thread;
        setcontext(tool_thread->ctx_ptr);
    }
}
