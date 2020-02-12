#ifndef MUTEX_IMPL_H_
#define MUTEX_IMPL_H_
#include "mutex.h"
#include "thread_impl.h"
#include <queue>
#include "utility.h"
class mutex::impl
{
public:
    std::queue<thread::impl *> m_waiting;
    statusType status = FREE;
};

#endif