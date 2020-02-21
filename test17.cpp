#include <iostream>
#include <cstdlib>
#include "thread.h"

mutex m;

void printer()
{
    thread::yield();
    m.lock();
    printf("I'm really tired\n");
    m.unlock();
}

void scheduler()
{
    for (int i = 0; i < 1000000; ++i)
    {
        thread t0((thread_startfunc_t)printer, nullptr);
        thread t1((thread_startfunc_t)printer, nullptr);
        thread t2((thread_startfunc_t)printer, nullptr);
        thread t3((thread_startfunc_t)printer, nullptr);
        thread t4((thread_startfunc_t)printer, nullptr);
        thread t5((thread_startfunc_t)printer, nullptr);
        thread t6((thread_startfunc_t)printer, nullptr);
        thread t7((thread_startfunc_t)printer, nullptr);
        thread t8((thread_startfunc_t)printer, nullptr);
    }
    
}

int main()
{
    cpu::boot(1, (thread_startfunc_t)scheduler, nullptr, true, true, 0);
}