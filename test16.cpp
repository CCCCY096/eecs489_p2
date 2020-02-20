#include <iostream>
#include <cstdlib>
#include "thread.h"

static int counter = 0;
mutex m;
bool done = true;
cv c1;
void printer(int &n)
{
    m.lock();
    printf("p1 arrived \n");
    m.unlock();
    int i = 0;
    while (i < 30)
    {
        m.lock();
        std::cout<< "thread 1: " << i++ <<std::endl;
        while(!done)
            c1.wait(m);
        done = !done;
        m.unlock();
        c1.signal();
        thread::yield();
    }
    m.lock();
    printf("printer1 finished\n");
    m.unlock();
}

void printer2(int &n)
{
    m.lock();
    printf("p2 arrived \n");
    m.unlock();
    int i = 0;
    while (i < 30)
    {
        m.lock();
        while(!done)
            c1.wait(m);
        std::cout<< "thread 2: " << i++ <<std::endl;
        m.unlock();
        c1.signal();
        thread::yield();
    }
    m.lock();
    printf("printer2 finished\n");
    m.unlock();
}

void scheduler()
{
    std::cout << "sch arrived " << std::endl;
    thread t1((thread_startfunc_t)printer, (void *)&counter);
    thread t2((thread_startfunc_t)printer2, (void *)&counter);
    thread t3((thread_startfunc_t)printer2, (void *)&counter);
    thread t4((thread_startfunc_t)printer2, (void *)&counter);
    thread::yield();
    printf("scheduler finished\n");
}

int main()
{
    cpu::boot(1, (thread_startfunc_t)scheduler, nullptr, true, true, 0);
}