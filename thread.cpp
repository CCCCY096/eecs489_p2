#include "thread.h"
#include "utility.h"
#include "cpu_impl.h"
#include <stdexcept>
#include <cassert>
thread::thread(thread_startfunc_t user_func, void *user_arg)
{
    raii_interrupt interrupt_disable;
    try
    {
        impl_ptr = context_init((thread_startfunc_t)wrapper, user_func, user_arg);
    }
    catch (std::bad_alloc &)
    {
        throw;
    }
    ready_queue.push(impl_ptr);
    morning_call();
}

thread::~thread()
{
    // If the user func has NOT already finished, 
    // then we shouldn't destroy the thread obj
    if (!this->impl_ptr->thread_done)
        this->impl_ptr->thread_done = true;
    // If user func has ALREADY finished, destory the obj
    else
    {
        assert(this->impl_ptr->join_queue.empty());
        this->impl_ptr->ctx_ptr = nullptr;
        delete this->impl_ptr;
        this->impl_ptr = nullptr;
    }
}

void thread::join()
{
    assert_interrupts_enabled();
    raii_interrupt interrupt_disable;
    cpu *current = cpu::self();
    // If user func of the JOINEE thread has finished, then we should do nothing
    if (this->impl_ptr && impl_ptr->user_func_finished)
        return;
    // Else: current thread will be blocked until JOINEE thread finished
    else if (this->impl_ptr && impl_ptr->user_func_finished == false)
        this->impl_ptr->join_queue.push(current->impl_ptr->thread_impl_ptr);
    else
        return;
    switch_helper();
}

void thread::yield()
{
    assert_interrupts_enabled();
    raii_interrupt interrupt_disable;
    // If there is no available thread in ready queue, then yield() should do nothing
    if (ready_queue.empty())
        return;
    ready_queue.push(cpu::self()->impl_ptr->thread_impl_ptr);
    morning_call();
    switch_helper();
}
