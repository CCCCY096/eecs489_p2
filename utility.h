#ifndef UTILITY_H_
#define UTILITY_H_

#include <queue>
#include "thread.h"
#include "thread_impl.h"
extern std::queue<thread::impl *> ready_queue;
extern std::queue<thread::impl *> finished_queue;
void wrapper(thread_startfunc_t user_func, void *user_arg, thread::impl *curr_impl);
thread::impl *context_init(thread_startfunc_t user_func, void *user_arg);

#endif