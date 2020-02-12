#ifndef UTILITY_H_
#define UTILITY_H_

#include "thread.h"
#include "thread_impl.h"
#include <queue>

class raii_interrupt
{
    raii_interrupt()
    {
        cpu::interrupt_enable();
    }
    ~raii_interrupt()
    {
        cpu::interrupt_disable();
    }
};

extern std::queue<thread::impl *> ready_queue;
extern std::queue<thread::impl *> finished_queue;
enum statusType
{
    FREE,
    BUSY
};
void wrapper(thread_startfunc_t user_func, void *user_arg, thread::impl *curr_impl);
thread::impl *context_init(thread_startfunc_t user_func, void *user_arg);
void switch_helper();
#endif