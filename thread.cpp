#include "thread.h"
#include "utility.h"
thread::thread(thread_startfunc_t user_func, void *user_arg)
{
    impl_ptr = context_init(user_func, user_arg);
    ready_queue.push(impl_ptr);
}

thread::~thread() {}