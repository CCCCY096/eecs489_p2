#include <iostream>
#include <cstdlib>
#include "thread.h"

static int counter = 0;
int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
void printer(int *n)
{
    printf("Thread %d reached\n", *n);
    thread::yield();
    printf("This is the thread i: %d\n", *n);
}

void scheduler()
{
    for (int i = 0; i < 15; i++)
    {
        thread((thread_startfunc_t)printer, (void *)&(arr[i]));
        counter++;
    }
}

int main()
{
    cpu::boot(1, (thread_startfunc_t)scheduler, nullptr, false, false, 0);
}