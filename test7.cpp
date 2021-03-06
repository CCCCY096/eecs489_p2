#include <iostream>
#include <cstdlib>
#include "thread.h"

void printer(thread *n)
{
    n->join();
    std::cout << "first finished " << std::endl;
}

void scheduler()
{
    thread t1((thread_startfunc_t)printer, (void *)&t1);
    std::cout << " printer finished" << std::endl;
    thread::yield();
}

int main()
{
    cpu::boot(1, (thread_startfunc_t)scheduler, nullptr, false, false, 0);
}