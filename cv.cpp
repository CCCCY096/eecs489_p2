#include "cv.h"
#include "cv_impl.h"
#include "utility.h"
#include "thread_impl.h"
#include "cpu_impl.h"
cv::cv(){
    impl_ptr = new cv::impl;
}

cv::~cv(){
    delete impl_ptr;
}

void cv::wait(mutex& m){
    // assert        
    impl_ptr->cv_waiting.push(cpu::self()->impl_ptr->thread_impl_ptr);
    m.unlock();
    switch_helper();
    m.lock();
}

void cv::signal()
{
    if(!impl_ptr->cv_waiting.empty())
    {
        ready_queue.push(impl_ptr->cv_waiting.front());
        impl_ptr->cv_waiting.pop();
    }
}

void cv::broadcast(){
    while(!impl_ptr->cv_waiting.empty()){
        ready_queue.push(impl_ptr->cv_waiting.front());
        impl_ptr->cv_waiting.pop();
    }
}