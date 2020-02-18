#ifndef MUTEX_IMPL_H_
#define MUTEX_IMPL_H_
#include "mutex.h"
#include "thread_impl.h"
#include <queue>
#include "utility.h"
#include <stdexcept>
class mutex::impl
{
public:
    std::queue<thread::impl *> m_waiting;
    statusType status = FREE;
    uint32_t owner_id = 0;
    void release()
    {
        if (status != BUSY || owner_id != cpu::self()->impl_ptr->thread_impl_ptr->tid)
            throw std::runtime_error("Sincerely apologize that we dont support your advanced codeing style :(");
        status = FREE;
        if (!m_waiting.empty())
        {
            ready_queue.push(m_waiting.front());
            morning_call();
            owner_id = m_waiting.front()->tid;
            m_waiting.pop();
            status = BUSY;
        }
    }
    void acquire()
    {
        if (status == FREE)
        {
            status = BUSY;
            owner_id = cpu::self()->impl_ptr->thread_impl_ptr->tid;
        }
        else
        {
            m_waiting.push(cpu::self()->impl_ptr->thread_impl_ptr);
            switch_helper();
        }
    }
};

#endif