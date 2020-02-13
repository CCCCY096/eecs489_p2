#ifndef CPU_IMPL_H_
#define CPU_IMPL_H_

#include "cpu.h"
#include "thread_impl.h"

class cpu::impl
{
public:
    thread::impl *thread_impl_ptr;
};

#endif