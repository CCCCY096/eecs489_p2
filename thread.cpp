#include "thread.h"
#include "utility.h"
#include "cpu_impl.h"
#include <stdexcept>
thread::thread(thread_startfunc_t user_func, void *user_arg)
{
    raii_interrupt interrupt_disable;
    try{
        impl_ptr = context_init(user_func, user_arg);
    }catch(std::bad_alloc&){
        throw;
    }
    ready_queue.push(impl_ptr);
}

thread::~thread() {}

void thread::join(){
    raii_interrupt interrupt_disable;
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
    raii_interrupt interrupt_disable;
    if (ready_queue.empty())
        return;
    ready_queue.push(cpu::self()->impl_ptr->thread_impl_ptr);
    switch_helper();
}
