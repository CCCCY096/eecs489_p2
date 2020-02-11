#include <iostream>
#include <cstdlib>
#include "thread.h"

static int counter = 0;
void printer(int &n)
{
    std::cout << "Scheduler reached\n";
    printf("This is the thread i: %d\n", counter++);
}

void scheduler()
{
    for (int i = 0; i < 15; i++)
        thread((thread_startfunc_t)printer, (void *)&counter);
}

int main()
{
    cpu::boot(1, (thread_startfunc_t)scheduler, nullptr, false, true, 0);
}