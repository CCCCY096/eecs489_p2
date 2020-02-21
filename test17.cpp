#include <iostream>
#include <cstdlib>
#include "thread.h"

mutex m;

void printer2(){m.lock();m.unlock();}


void printer()
{
    thread::yield();
    thread t0((thread_startfunc_t)printer2, nullptr);
    thread t1((thread_startfunc_t)printer2, nullptr);
    m.lock();
    thread::yield();
    thread t2((thread_startfunc_t)printer2, nullptr);
    thread t3((thread_startfunc_t)printer2, nullptr);
    printf("I'm really tired\n");
    m.unlock();
    t1.join();
    t3.join();
    printf("I'm really done\n");
}

void scheduler()
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

int main()
{
    cpu::boot(1, (thread_startfunc_t)scheduler, nullptr, true, true, 0);
}