#include "thread.h"

void scheduler(){
    mutex m1;
    m1.unlock();
    m1.lock();
}

int main()
{
    cpu::boot(1, (thread_startfunc_t)scheduler, nullptr, false, true, 0);
}