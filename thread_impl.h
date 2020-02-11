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
};

#endif