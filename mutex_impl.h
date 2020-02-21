#ifndef MUTEX_IMPL_H_
#define MUTEX_IMPL_H_

#include <queue>
#include <stdexcept>
#include "mutex.h"
#include "thread_impl.h"
#include "utility.h"

class mutex::impl
{
public:
    std::queue<thread::impl *> m_waiting;
    statusType status = FREE;
    // The id of owner thread
    uint32_t owner_id = 0;
    void release()
    {
        // If the mutex is not busy or the current thread is not its owner
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
            // Assign the owner
            owner_id = cpu::self()->impl_ptr->thread_impl_ptr->tid;
        }
        else
        {
            // Add current thread to the waiting queue for this mutex
            m_waiting.push(cpu::self()->impl_ptr->thread_impl_ptr);
            switch_helper();
        }
    }
};

#endif