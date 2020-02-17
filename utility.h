#ifndef UTILITY_H_
#define UTILITY_H_

#include "thread.h"
#include "thread_impl.h"
#include <queue>
#include <atomic>
#include <iostream>
class raii_interrupt
{
public:
    raii_interrupt()
    {
        assert_interrupts_enabled();
        cpu::interrupt_disable();
        while(cpu::guard.exchange(1)){}
    }
    ~raii_interrupt()
    {
        cpu::guard.store(0);
        assert_interrupts_disabled();
        cpu::interrupt_enable();
    }
};

extern std::queue<thread::impl *> ready_queue;
extern std::queue<ucontext_t *> finished_queue;
extern std::queue<cpu *> sleep_queue;
extern uint32_t id_auto_incr;
enum statusType
{
    FREE,
    BUSY
};
void wrapper(thread_startfunc_t user_func, void *user_arg, thread::impl *curr_impl);
thread::impl *context_init(thread_startfunc_t wrap_func ,thread_startfunc_t user_func, void *user_arg);
void switch_helper(ucontext_t* curr_ctx_ptr = nullptr);
void suspend_helper();
void morning_call();
#endif