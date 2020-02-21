#ifndef UTILITY_H_
#define UTILITY_H_

#include <queue>
#include <atomic>
#include <iostream>
#include "thread.h"
#include "thread_impl.h"

// var used to create unique id for each thread
extern uint32_t id_auto_incr;
// RAII used to provide mutual exclusion between CPUs
// and disable interrupts
class raii_interrupt
{
public:
    raii_interrupt()
    {
        assert_interrupts_enabled();
        cpu::interrupt_disable();
        while (cpu::guard.exchange(1))
        {
        }
    }
    ~raii_interrupt()
    {
        cpu::guard.store(0);
        assert_interrupts_disabled();
        cpu::interrupt_enable();
    }
};
enum statusType
{
    FREE,
    BUSY
};
extern std::queue<thread::impl *> ready_queue;
extern std::queue<ucontext_t *> finished_queue;
extern std::queue<cpu *> sleep_queue;
// Wrap user func
void wrapper(thread_startfunc_t user_func, void *user_arg);
// Make context for user_func
thread::impl *context_init(thread_startfunc_t wrap_func, thread_startfunc_t user_func, void *user_arg);
// Make currnet CPU switch to next (if any) thread
void switch_helper(ucontext_t *curr_ctx_ptr = nullptr);
// If there is no available threads in ready queue, then the current CPU will
// make context for this function and suspends there
void suspend_helper();
// Wake up one (if any) suspending thread
void morning_call();

#endif