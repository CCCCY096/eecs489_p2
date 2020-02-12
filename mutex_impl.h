#ifndef MUTEX_IMPL_H_
#define MUTEX_IMPL_H_
#include "mutex.h"
#include "thread_impl.h"
#include <queue>
#include "utility.h"
class mutex::impl
{
public:
    std::queue<thread::impl *> m_waiting;
    statusType status = FREE;
    void release(){
        status = FREE;
        if(!m_waiting.empty())
        {
            ready_queue.push(m_waiting.front());
            m_waiting.pop();
            status = BUSY;
        }
    }
    void acquire(){
        if(status == FREE)
            status = BUSY;
        else
        {
            m_waiting.push(cpu::self()->impl_ptr->thread_impl_ptr);
            switch_helper();
        }
    }
};

#endif