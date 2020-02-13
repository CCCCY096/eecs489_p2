#include <iostream>
#include <cstdlib>
#include "thread.h"
mutex m1;
void printer(thread* n)
{
    m1.unlock();
    std::cout << "first finished " << std::endl;
}

void scheduler()
{
    m1.lock();
    thread t1((thread_startfunc_t)printer, (void *)&t1);
    thread::yield();
    std::cout << " printer finished" << std::endl;
}

int main()
{
    cpu::boot(1, (thread_startfunc_t)scheduler, nullptr, false, false, 0);
}