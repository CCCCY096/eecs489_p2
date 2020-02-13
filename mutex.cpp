#include <queue>
#include "mutex.h"
#include "cpu_impl.h"
#include "utility.h"
#include <atomic>
#include "mutex_impl.h"
mutex::mutex(){
     try{
        impl_ptr = new mutex::impl;
    }catch(std::bad_alloc&){
        throw;
    }
}

mutex::~mutex(){
    delete impl_ptr;
}

void mutex::unlock(){
    raii_interrupt interrupt_disable;
    impl_ptr->release();
}


void mutex::lock()
{
    raii_interrupt interrupt_disable;
    impl_ptr->acquire();
}







