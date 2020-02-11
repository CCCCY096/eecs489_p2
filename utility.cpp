#include "utility.h"
#include "cpu_impl.h"
#include <iostream>
std::queue<thread::impl *> ready_queue;
std::queue<thread::impl *> finished_queue;
void wrapper(thread_startfunc_t user_func, void *user_arg, thread::impl *curr_impl)
{
    user_func(user_arg);
    while (!finished_queue.empty()){
        // Delete its stack and context
        thread::impl* to_be_deleted = finished_queue.front();
        finished_queue.pop();
        delete[] (char*)to_be_deleted->ctx_ptr->uc_stack.ss_sp;
        delete to_be_deleted->ctx_ptr;
        // Before delete this thread, release all threads that are waiting for it
        while (!to_be_deleted->join_queue.empty())
        {
            ready_queue.push(to_be_deleted->join_queue.front());
            to_be_deleted->join_queue.pop();
        }
        delete to_be_deleted;
    }
    // If no available user functions in ready_queue, suspend
    while (ready_queue.empty())
    {
        std::cout <<"i really want to suspend" <<std::endl;
        cpu::interrupt_enable_suspend();
    }
    // Pick the first available user function:
    // 1. push current context to finished queue
    // 2. switch to next context
    finished_queue.push(curr_impl);
    thread::impl *next_impl = ready_queue.front();
    ready_queue.pop();
    cpu::self()->impl_ptr->thread_impl_ptr = next_impl;
    swapcontext(curr_impl->ctx_ptr, next_impl->ctx_ptr);
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