#ifndef CPU_IMPL_H_
#define CPU_IMPL_H_

#include "cpu.h"
#include "thread_impl.h"

class cpu::impl
{
public:
    // Pointer to thread::impl. Use this var to remeber which thread
    // this cpu is running on
    thread::impl *thread_impl_ptr;
};

#endif