#include "thread.h"
#include "utility.h"
#include "cpu_impl.h"
thread::thread(thread_startfunc_t user_func, void *user_arg)
{
    impl_ptr = context_init(user_func, user_arg);
    ready_queue.push(impl_ptr);
}

thread::~thread() {}

void thread::join(){
    cpu* current = cpu::self();
    if (!this->impl_ptr->done)
        this->impl_ptr->join_queue.push(current->impl_ptr->thread_impl_ptr);
    else
        return;
    // Maybe refactor this? It's kind of duplicate with a part of yield
    while (ready_queue.empty())
    {
        cpu::interrupt_enable_suspend();
    }
    thread::impl *next_impl = ready_queue.front();
    ready_queue.pop();
    ucontext_t *tmp_curr_context = current->impl_ptr->thread_impl_ptr->ctx_ptr;
    cpu::self()->impl_ptr->thread_impl_ptr = next_impl;
    swapcontext(tmp_curr_context, next_impl->ctx_ptr);
}

void thread::yield()
{
    if (ready_queue.empty())
        return;
    thread::impl *next_impl = ready_queue.front();
    thread::impl *curr_impl = cpu::self()->impl_ptr->thread_impl_ptr;
    ready_queue.pop();
    ready_queue.push(curr_impl);
    cpu::self()->impl_ptr->thread_impl_ptr = next_impl;
    swapcontext(curr_impl->ctx_ptr, next_impl->ctx_ptr);
}
