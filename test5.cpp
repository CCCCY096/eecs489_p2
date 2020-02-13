#include <iostream>
#include <cstdlib>
#include "thread.h"
#include <vector>
static int counter = 0;
void printer(int &n)
{
    int i = 0;
    while(i < 100000000)
        i++;
    printf("This is the thread i: %d\n", counter++);
}

void scheduler()
{
    std::vector<thread*> vec;
    vec.resize(15);
    for( int i = 0; i < 15; i++ ){
        vec[i] = new thread((thread_startfunc_t)printer, (void *)&counter);
    }
    std::cout << " threads generated" <<std::endl;
    for( int i = 0; i < 15; i++)
        vec[i]->join();
    std::cout<< "all finished"<<std::endl;
}

int main()
{
    cpu::boot(1, (thread_startfunc_t)scheduler, nullptr, false, true, 0);
}