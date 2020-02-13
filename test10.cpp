#include <iostream>
#include <cstdlib>
#include "thread.h"
#include "mutex.h"

mutex m1;
mutex m2;
void printer()
{
    m2.lock();
    printf("Locked m2\n");
    m1.lock();
}

void scheduler()
{
    thread t1((thread_startfunc_t)printer, nullptr);
    m1.lock();
    printf("Locked m1\n");
    thread::yield();
    m2.lock();
    printf("sdfsdfs");
}

int main()
{
    cpu::boot(1, (thread_startfunc_t)scheduler, nullptr, false, true, 0);
}