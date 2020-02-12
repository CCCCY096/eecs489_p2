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
    switch_helper();
}

void thread::yield()
{
    if (ready_queue.empty())
        return;
    ready_queue.push(cpu::self()->impl_ptr->thread_impl_ptr);
    switch_helper();
}
