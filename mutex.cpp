#include <queue>
#include "mutex.h"
#include "cpu_impl.h"
#include "utility.h"
#include <atomic>
#include "mutex_impl.h"
mutex::mutex(){
    impl_ptr = new mutex::impl;
}

mutex::~mutex(){
    delete impl_ptr;
}

void mutex::unlock(){
    while(cpu::guard.exchange(1)){}
    impl_ptr->status = FREE;
    if(!impl_ptr->m_waiting.empty())
    {
        ready_queue.push(impl_ptr->m_waiting.front());
        impl_ptr->m_waiting.pop();
        impl_ptr->status = BUSY;
    }
    cpu::guard.store(0);
}


void mutex::lock()
{
    while(cpu::guard.exchange(1)){}
    if (impl_ptr->status == FREE)
        impl_ptr->status = BUSY;
    else
    {
        impl_ptr->m_waiting.push(cpu::self()->impl_ptr->thread_impl_ptr);
        switch_helper();
    }
    cpu::guard.store(0);
}







