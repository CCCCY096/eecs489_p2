#include <queue>
#include <atomic>
#include "mutex.h"
#include "cpu_impl.h"
#include "utility.h"
#include "mutex_impl.h"

mutex::mutex()
{
    try
    {
        impl_ptr = new mutex::impl;
    }
    catch (std::bad_alloc &)
    {
        throw;
    }
}

mutex::~mutex()
{
    delete impl_ptr;
}

void mutex::unlock()
{
    assert_interrupts_enabled();
    raii_interrupt interrupt_disable;
    impl_ptr->release();
}

void mutex::lock()
{
    assert_interrupts_enabled();
    raii_interrupt interrupt_disable;
    impl_ptr->acquire();
}
