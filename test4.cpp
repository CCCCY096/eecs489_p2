#include <iostream>
#include <cstdlib>
#include "thread.h"

static int counter = 0;
void printer(int &n)
{
    int i = 0;
    while (i < 100000000)
        i++;
    printf("This is the thread i: %d\n", counter++);
    std::cout << "first finished " << std::endl;
}

void scheduler()
{
    thread t1((thread_startfunc_t)printer, (void *)&counter);
    t1.join();
    std::cout << " printer finished" << std::endl;
}

int main()
{
    cpu::boot(1, (thread_startfunc_t)scheduler, nullptr, false, true, 0);
}