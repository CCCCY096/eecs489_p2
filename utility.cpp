// TODO:
// Delete the third argument in wrapper()
// Mutex/cv private member variables?
// while -> if?
// ready_queue.push change it to helper function?
// 2 raii? one for guard, one for interrupt?

#include "utility.h"
#include <cassert>
#include "cpu_impl.h"
#include <cassert>

// Initialize global variables
std::queue<thread::impl *> ready_queue;
std::queue<ucontext_t *> finished_queue;
std::queue<cpu *> sleep_queue;
uint32_t id_auto_incr = 1;

void release_memory()
{
    assert_interrupts_disabled();
    while (!finished_queue.empty())
    {
        // Delete its stack and context
        ucontext_t *to_be_deleted = finished_queue.front();
        finished_queue.pop();
        delete[](char *) to_be_deleted->uc_stack.ss_sp;
        delete to_be_deleted;
    }
    assert_interrupts_disabled();
}

void wrapper(thread_startfunc_t user_func, void *user_arg)
{
    /* ----- Execute user function ------------- */
    assert_interrupts_disabled();
    release_memory();
    cpu::guard.store(0);
    cpu::interrupt_enable();
    user_func(user_arg);
    assert_interrupts_enabled();
    /* ----- user function finished ------------ */

    raii_interrupt interrupt_disable;
    thread::impl *curr_impl = cpu::self()->impl_ptr->thread_impl_ptr;
    // Mark current thread as uesr_func_finished
    curr_impl->user_func_finished = true;
    // Before delete this thread, release all threads that are waiting for it
    while (!cpu::self()->impl_ptr->thread_impl_ptr->join_queue.empty())
    {
        ready_queue.push(cpu::self()->impl_ptr->thread_impl_ptr->join_queue.front());
        morning_call();
        cpu::self()->impl_ptr->thread_impl_ptr->join_queue.pop();
    }
    // useless_ctx: trivial var to pass to switch helper
    // switch helper then will know that it doesn't need to store the current context
    ucontext_t *useless_ctx = curr_impl->ctx_ptr;
    finished_queue.push(curr_impl->ctx_ptr);
    // If thread obj has not finished, then we should not destroy the obj
    if (!curr_impl->thread_done)
        curr_impl->thread_done = true;
    // Else destroy the obj
    else
    {
        assert(curr_impl->join_queue.empty());
        curr_impl->ctx_ptr = nullptr;
        delete curr_impl;
        curr_impl = nullptr;
    }
    assert_interrupts_disabled();
    switch_helper(useless_ctx);
}

thread::impl *context_init(thread_startfunc_t wrap_func, thread_startfunc_t user_func, void *user_arg)
{
    // Make context
    ucontext_t *ucontext_ptr = new ucontext_t;
    char *stack = new char[STACK_SIZE];
    ucontext_ptr->uc_stack.ss_sp = stack;
    ucontext_ptr->uc_stack.ss_size = STACK_SIZE;
    ucontext_ptr->uc_stack.ss_flags = 0;
    ucontext_ptr->uc_link = nullptr;
    getcontext(ucontext_ptr);
    // Build new thread::impl
    thread::impl *thread_impl_ptr = new thread::impl;
    thread_impl_ptr->ctx_ptr = ucontext_ptr;
    thread_impl_ptr->tid = id_auto_incr++;
    assert_interrupts_disabled();
    // 2 choice to make context for: wrapper or suspend_helper
    if (user_func)
        makecontext(ucontext_ptr, (void (*)())wrap_func, 3, user_func, user_arg);
    else
        makecontext(ucontext_ptr, (void (*)())wrap_func, 0);
    assert_interrupts_disabled();
    return thread_impl_ptr;
}

void switch_helper(ucontext_t *curr_ctx_ptr)
{
    assert_interrupts_disabled();
    // Trivial procedures to decide which context to store
    ucontext_t *tmp_ctx_ptr;
    if (curr_ctx_ptr)
    {
        tmp_ctx_ptr = curr_ctx_ptr;
    }
    else
    {
        tmp_ctx_ptr = cpu::self()->impl_ptr->thread_impl_ptr->ctx_ptr;
    }
    // If there is no available func in ready_queue, then we should make context for suspend_helper and suspends there
    if (ready_queue.empty())
    {
        thread::impl *tool_thread = context_init((thread_startfunc_t)suspend_helper, (thread_startfunc_t) nullptr, (void *)nullptr);
        cpu::self()->impl_ptr->thread_impl_ptr = tool_thread;
        assert_interrupts_disabled();
        swapcontext(tmp_ctx_ptr, tool_thread->ctx_ptr);
        release_memory();
    }
    // Else, then we should pick next available thread and swap to there
    else
    {
        assert_interrupts_disabled();
        // Pick next ready thread and pop it from ready_queue
        thread::impl *next_impl = ready_queue.front();
        ready_queue.pop();
        // Make sure cpu remember on what thread it is running
        cpu::self()->impl_ptr->thread_impl_ptr = next_impl;
        // swap context
        swapcontext(tmp_ctx_ptr, next_impl->ctx_ptr);
        assert_interrupts_disabled();
        release_memory();
    }
}

void suspend_helper()
{
    assert_interrupts_disabled();
    // If ready_queue is empty, then suspends here
    while (ready_queue.empty())
    {
        assert_interrupts_disabled();
        release_memory();
        sleep_queue.push(cpu::self());
        cpu::guard.store(0);
        cpu::interrupt_enable_suspend();
        assert_interrupts_enabled();
        cpu::interrupt_disable();
        while (cpu::guard.exchange(1)) {}
    }
    thread::impl *curr_impl = cpu::self()->impl_ptr->thread_impl_ptr;
    // Now that the suspend_helper has done its job, add its context to finished_queue
    finished_queue.push(curr_impl->ctx_ptr);
    assert(curr_impl->join_queue.empty());
    delete curr_impl;
    curr_impl = nullptr;
    // Pick next ready thread and pop it from ready_queue
    thread::impl *next_impl = ready_queue.front();
    ready_queue.pop();
    // Make sure cpu remember on what thread it is running
    cpu::self()->impl_ptr->thread_impl_ptr = next_impl;
    assert_interrupts_disabled();
    // set context
    setcontext(next_impl->ctx_ptr);
}

void morning_call()
{
    assert_interrupts_disabled();
    assert(!ready_queue.empty());
    if (!sleep_queue.empty())
    {
        sleep_queue.front()->interrupt_send();
        sleep_queue.pop();
    }
}