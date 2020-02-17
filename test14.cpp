#include <iostream>
#include <cstdlib>
#include "thread.h"

static int counter = 0;
mutex m;
void printer(int &n)
{
    m.lock();
    printf ("p1 arrived \n");
    m.unlock();
    // int i = 0;
    // while (i < 30)
    // {
    //     m.lock();
    //     std::cout<< "thread 1: " << i++ <<std::endl;
    //     m.unlock();
    // }
    m.lock();
    printf("printer1 finished\n");
    m.unlock();
}

void printer2(int &n)
{
    m.lock();
    printf ("p2 arrived \n");
    m.unlock();
    // int i = 0;
    // while (i < 30)
    // {
    //     m.lock();
    //     std::cout<< "thread 2: " << i++ <<std::endl;
    //     m.unlock();
    // }
    m.lock();
    printf("printer2 finished\n");
    m.unlock();
}

void scheduler()
{
    std::cout<< "sch arrived "<<std::endl;
    thread t1((thread_startfunc_t)printer, (void *)&counter);
    thread t2((thread_startfunc_t)printer2, (void *)&counter);
    thread::yield();
    printf("scheduler finished\n");
}

int main()
{
    cpu::boot(1, (thread_startfunc_t)scheduler, nullptr, true, true, 0);
}