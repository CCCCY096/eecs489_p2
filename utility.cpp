// TODO:
// Delete the third argument in wrapper()
// Mutex/cv private member variables?
// while -> if?

#include "utility.h"
#include "cpu_impl.h"
std::queue<thread::impl *> ready_queue;
std::queue<thread::impl *> finished_queue;
void wrapper(thread_startfunc_t user_func, void *user_arg, thread::impl *curr_impl)
{
    cpu::interrupt_enable();
    user_func(user_arg);
    raii_interrupt interrupt_disable;
    curr_impl->done = true;
    while (!finished_queue.empty()){
        // Delete its stack and context
        thread::impl* to_be_deleted = finished_queue.front();
        finished_queue.pop();
        delete[] (char*)to_be_deleted->ctx_ptr->uc_stack.ss_sp;
        delete to_be_deleted->ctx_ptr;
        to_be_deleted->ctx_ptr = nullptr;

    }
    // If no available user functions in ready_queue, suspend

    // Pick the first available user function:
    // 1. push current context to finished queue
    // 2. switch to next context
    finished_queue.push(curr_impl);
    // Before delete this thread, release all threads that are waiting for it
    while (!cpu::self()->impl_ptr->thread_impl_ptr->join_queue.empty())
    {
        ready_queue.push(cpu::self()->impl_ptr->thread_impl_ptr->join_queue.front());
        cpu::self()->impl_ptr->thread_impl_ptr->join_queue.pop();
    }


    switch_helper();
}


thread::impl *context_init(thread_startfunc_t user_func, void *user_arg)
{
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
    makecontext(ucontext_ptr, (void (*)())wrapper, 3, user_func, user_arg, thread_impl_ptr );
    return thread_impl_ptr;
}

void switch_helper()
{
    assert_interrupts_disabled();
    while (ready_queue.empty())
    {
        cpu::interrupt_enable_suspend();
        
    }
    thread::impl *curr_impl = cpu::self()->impl_ptr->thread_impl_ptr;
    // Pick next ready thread and pop it from ready_queue
    thread::impl *next_impl = ready_queue.front();
    ready_queue.pop();
    // Make sure cpu remember on what thread it is running
    cpu::self()->impl_ptr->thread_impl_ptr = next_impl;
    // swap context
    swapcontext(curr_impl->ctx_ptr, next_impl->ctx_ptr);
    assert_interrupts_disabled();
}