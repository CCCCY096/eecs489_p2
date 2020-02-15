#include "cv.h"
#include "cv_impl.h"
#include "utility.h"
#include "thread_impl.h"
#include "cpu_impl.h"
#include "mutex_impl.h"
#include <stdexcept>
cv::cv(){
    try{
        impl_ptr = new cv::impl;
    }catch(std::bad_alloc&){
        throw;
    }
}

cv::~cv(){
    delete impl_ptr;
}

void cv::wait(mutex& m){
    assert_interrupts_enabled();
    raii_interrupt interrupt_disable;
    impl_ptr->cv_waiting.push(cpu::self()->impl_ptr->thread_impl_ptr);
    m.impl_ptr->release();
    switch_helper();
    m.impl_ptr->acquire();
}

void cv::signal()
{
    assert_interrupts_enabled();
    raii_interrupt interrupt_disable;
    if(!impl_ptr->cv_waiting.empty())
    {
        ready_queue.push(impl_ptr->cv_waiting.front());
        morning_call();
        impl_ptr->cv_waiting.pop();
    }
}

void cv::broadcast(){
    assert_interrupts_enabled();
    raii_interrupt interrupt_disable;
    while(!impl_ptr->cv_waiting.empty()){
        ready_queue.push(impl_ptr->cv_waiting.front());
        morning_call();
        impl_ptr->cv_waiting.pop();
    }
}