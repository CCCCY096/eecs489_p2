#include <iostream>
#include <cstdlib>
#include "thread.h"
//wrapper(thread) ends first
void printer()
{
    std::cout << "first finished " << std::endl;
}

void scheduler()
{
    thread t1((thread_startfunc_t)printer , nullptr);
    t1.join();
    std::cout << " printer finished" << std::endl;
}

int main()
{
    cpu::boot(1, (thread_startfunc_t)scheduler, nullptr, false, false, 0);
}