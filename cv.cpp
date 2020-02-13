#include "cv.h"
#include "cv_impl.h"
#include "utility.h"
#include "thread_impl.h"
#include "cpu_impl.h"
#include "mutex_impl.h"
cv::cv(){
    impl_ptr = new cv::impl;
}

cv::~cv(){
    delete impl_ptr;
}

void cv::wait(mutex& m){
    raii_interrupt interrupt_disable;
    impl_ptr->cv_waiting.push(cpu::self()->impl_ptr->thread_impl_ptr);
    m.impl_ptr->release();
    switch_helper();
    m.impl_ptr->acquire();
}

void cv::signal()
{
    raii_interrupt interrupt_disable;
    if(!impl_ptr->cv_waiting.empty())
    {
        ready_queue.push(impl_ptr->cv_waiting.front());
        impl_ptr->cv_waiting.pop();
    }
}

void cv::broadcast(){
    raii_interrupt interrupt_disable;
    while(!impl_ptr->cv_waiting.empty()){
        ready_queue.push(impl_ptr->cv_waiting.front());
        impl_ptr->cv_waiting.pop();
    }
}