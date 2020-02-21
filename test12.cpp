#include <iostream>
#include <cstdlib>
#include "thread.h"
//wrapper(thread) ends first
int finished = 0;
int first = true;
mutex m;
void printer()
{
    std::cout << "first finished " << std::endl;
    cv c1;
    while(!finished)
        c1.wait(m);
    thread::yield();
    if(first){
        thread t1((thread_startfunc_t)printer, nullptr);
        first = false;
        t1.join();
    }
}

void scheduler()
{   
    m.lock();
    thread t1((thread_startfunc_t)printer, nullptr);
    t1.join();
    thread::yield();
    finished = 1;
    std::cout << " printer finished" << std::endl;
}

int main()
{
    cpu::boot(1, (thread_startfunc_t)scheduler, nullptr, false, false, 0);
}