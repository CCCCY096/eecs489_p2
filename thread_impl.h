#ifndef THREAD_IMPL_H_
#define THREAD_IMPL_H_

#include <queue>
#include "thread.h"
#include "ucontext.h"

class thread::impl
{
public:
    ucontext_t *ctx_ptr;
    std::queue<thread::impl *> join_queue;
    // If thread obj OR user func finished
    bool thread_done = false;
    // If the user_func has finished
    bool user_func_finished = false;
    // thread ID
    uint32_t tid;
};

#endif