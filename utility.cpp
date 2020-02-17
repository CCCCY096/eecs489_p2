// TODO:
// Delete the third argument in wrapper()
// Mutex/cv private member variables?
// while -> if?
// ready_queue.push change it to helper function?
// 2 raii? one for guard, one for interrupt?

#include "utility.h"
#include <cassert>
#include "cpu_impl.h"
std::queue<thread::impl *> ready_queue;
std::queue<ucontext_t *> finished_queue;
std::queue<cpu *> sleep_queue;
uint32_t id_auto_incr = 1;
void release_memory()
{
    assert_interrupts_disabled();
    while (!finished_queue.empty()){
        // Delete its stack and context
        ucontext_t* to_be_deleted = finished_queue.front();
        finished_queue.pop();
        delete[] (char*)to_be_deleted->uc_stack.ss_sp;
        delete to_be_deleted;
    }
    assert_interrupts_disabled();
}

void wrapper(thread_startfunc_t user_func, void *user_arg, thread::impl *curr_impl)
{
    assert_interrupts_disabled();
    release_memory();
    cpu::guard.store(0);
    cpu::interrupt_enable();
    user_func(user_arg);
    assert_interrupts_enabled();
    raii_interrupt interrupt_disable;
    // If no available user functions in ready_queue, suspend

    // Pick the first available user function:
    // 1. push current context to finished queue
    // 2. switch to next context
    // Before delete this thread, release all threads that are waiting for it
    curr_impl->thread_finished = true;
    while (!cpu::self()->impl_ptr->thread_impl_ptr->join_queue.empty())
    {
        ready_queue.push(cpu::self()->impl_ptr->thread_impl_ptr->join_queue.front());
        morning_call();
        cpu::self()->impl_ptr->thread_impl_ptr->join_queue.pop();
    }
    if(!curr_impl->done) 
        curr_impl->done = true;
    else {
        assert(curr_impl->join_queue.empty());
        curr_impl->ctx_ptr = nullptr;
        delete curr_impl;
        curr_impl = nullptr;
    }
    finished_queue.push(curr_impl->ctx_ptr);
    assert_interrupts_disabled();
    switch_helper();
}


thread::impl *context_init(thread_startfunc_t user_func, void *user_arg)
{
    // Release_memory
    release_memory();
    // Make context
    ucontext_t *ucontext_ptr = new ucontext_t;
    char *stack = new char[STACK_SIZE];
    ucontext_ptr->uc_stack.ss_sp = stack;
    ucontext_ptr->uc_stack.ss_size = STACK_SIZE;
    ucontext_ptr->uc_stack.ss_flags = 0;
    ucontext_ptr->uc_link = nullptr;
    getcontext(ucontext_ptr);
    // Build new thread::impl
    thread::impl *thread_impl_ptr = new thread::impl;
    thread_impl_ptr->ctx_ptr = ucontext_ptr;
    thread_impl_ptr->tid = id_auto_incr++;
    assert_interrupts_disabled();
    makecontext(ucontext_ptr, (void (*)())wrapper, 3, user_func, user_arg, thread_impl_ptr );
    assert_interrupts_disabled();
    return thread_impl_ptr;
}

void switch_helper()
{
    assert_interrupts_disabled();
    thread::impl *curr_impl = cpu::self()->impl_ptr->thread_impl_ptr; 
    if (ready_queue.empty())
    {
        thread::impl* tool_thread = context_init((thread_startfunc_t)suspend_helper, (void*)nullptr);
        cpu::self()->impl_ptr->thread_impl_ptr = tool_thread;
        assert_interrupts_disabled();
        swapcontext(curr_impl->ctx_ptr, tool_thread->ctx_ptr);
        release_memory();
    }
    else
    {
        assert_interrupts_disabled();
        // Pick next ready thread and pop it from ready_queue
        thread::impl *next_impl = ready_queue.front();
        ready_queue.pop();
        // Make sure cpu remember on what thread it is running
        cpu::self()->impl_ptr->thread_impl_ptr = next_impl;
        // swap context
        swapcontext(curr_impl->ctx_ptr, next_impl->ctx_ptr);
        assert_interrupts_disabled();
        release_memory();
    }
}

void suspend_helper()
{
    assert_interrupts_disabled();
    release_memory();
    sleep_queue.push(cpu::self());
    cpu::guard.store(0);
    cpu::interrupt_enable_suspend();
    // raii_interrupt interrupt;
    assert_interrupts_enabled();
    cpu::interrupt_disable();
    while(cpu::guard.exchange(1)){}

    thread::impl *curr_impl = cpu::self()->impl_ptr->thread_impl_ptr;
    finished_queue.push(curr_impl->ctx_ptr);
    assert(curr_impl->join_queue.empty());
    delete curr_impl;
    curr_impl = nullptr;
    // Pick next ready thread and pop it from ready_queue
    thread::impl *next_impl = ready_queue.front();
    ready_queue.pop();
    // Make sure cpu remember on what thread it is running
    cpu::self()->impl_ptr->thread_impl_ptr = next_impl;
    // set context
    setcontext(next_impl->ctx_ptr);
}

void morning_call()
{
    assert_interrupts_disabled();
    if(!sleep_queue.empty()){
        sleep_queue.front()->interrupt_send();
        sleep_queue.pop();
    }
}