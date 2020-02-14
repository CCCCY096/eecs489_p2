#include <iostream>
#include <cstdlib>
#include "thread.h"

static int counter = 0;
mutex m;
void printer(int &n)
{
    int i = 0;
    while (i < 300)
    {
        m.lock();
        std::cout<< "thread 1: " << i++ <<std::endl;
        m.unlock();
    }
    m.lock();
    printf("printer1 finished\n");
    m.unlock();
}

void printer2(int &n)
{
    int i = 0;
    while (i < 300)
    {
        m.lock();
        std::cout<< "thread 2: " << i++ <<std::endl;
        m.unlock();
    }
    m.lock();
    printf("printer2 finished\n");
    m.unlock();
}

void scheduler()
{
    thread t1((thread_startfunc_t)printer, (void *)&counter);
    thread t2((thread_startfunc_t)printer2, (void *)&counter);
    printf("scheduler finished\n");
}

int main()
{
    cpu::boot(2, (thread_startfunc_t)scheduler, nullptr, false, false, 0);
}