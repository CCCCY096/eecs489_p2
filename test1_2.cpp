#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>
#include <stdlib.h>
#include "thread.h"
#include <limits>
using std::cin;
using std::cout;

std::vector<std::queue<int>> requestsCollector;
std::vector<std::queue<int>> intermediate;
mutex mqueue;
cv cv_request;
cv cv_receive;
int max_disk_queue;
int file_num;
int current_active_threads;
int current_track = 0;
int queue_size = 0;
int req_num;

int find_closest_track(void)
{
   mqueue.unlock();
   thread::yield();
   mqueue.lock();
   int i, dist = std::numeric_limits<int>::max(), id = 0;
   mqueue.unlock();
   thread::yield();
   mqueue.lock();
   for (i = 0; i < file_num; i++)
   {
      if (requestsCollector[i].empty())
         continue;
      if (std::abs(requestsCollector[i].front() - current_track) < dist)
      {
         mqueue.unlock();
         thread::yield();
         mqueue.lock();
         dist = std::abs(requestsCollector[i].front() - current_track);
         id = i;
      }
   }
   mqueue.unlock();
   thread::yield();
   mqueue.lock();
   current_track = requestsCollector[id].front();
   return id;
}

void requester(void *a)
{
   int id = *(int *)a;
   thread::yield();
   mqueue.lock();
   thread::yield();
   mqueue.unlock();
   thread::yield();
   mqueue.lock();
   thread::yield();
   while (!intermediate[id].empty())
   {
      mqueue.unlock();
      thread::yield();
      mqueue.lock();
      cv_receive.signal();
      while (queue_size >= max_disk_queue || !requestsCollector[id].empty())
         cv_request.wait(mqueue);
      mqueue.unlock();
      thread::yield();
      mqueue.lock();
      int req_track = intermediate[id].front();
      mqueue.unlock();
      cv_request.signal();
      thread::yield();
      mqueue.lock();
      cv_receive.signal();
      cout << "Request from requester : " << req_track << std::endl;
      mqueue.unlock();
      thread::yield();
      mqueue.lock();
      cv_receive.signal();
      requestsCollector[id].push(req_track);
      mqueue.unlock();
      thread::yield();
      mqueue.lock();
      cv_receive.signal();
      intermediate[id].pop();
      queue_size++;
      cv_receive.signal();
      cv_receive.signal();
   }
   req_num--;
   if (!req_num)
      cv_receive.signal();
   mqueue.unlock();
   delete (int *)a;
   return;
}

void receiver(void *a)
{
   mqueue.lock();
   thread::yield();
   mqueue.unlock();
   thread::yield();
   mqueue.lock();
   while (current_active_threads)
   {
      mqueue.unlock();
      thread::yield();
      mqueue.lock();
      while (queue_size < max_disk_queue)
         cv_receive.wait(mqueue);
      int rid = find_closest_track();
      mqueue.unlock();
      thread::yield();
      mqueue.lock();
      cout << " Serve track: " << requestsCollector[rid].front() << std::endl;
      requestsCollector[rid].pop();
      mqueue.unlock();
      thread::yield();
      mqueue.lock();
      if (requestsCollector[rid].empty() && intermediate[rid].empty())
      {
         mqueue.unlock();
         thread::yield();
         mqueue.lock();
         current_active_threads--;
         if (current_active_threads < max_disk_queue)
            max_disk_queue = current_active_threads;
      }
      queue_size--;
      mqueue.unlock();
      thread::yield();
      mqueue.lock();
      cv_request.broadcast();
   }
   mqueue.unlock();
   return;
}

void scheduler(void *a)
{
   for (int i = 0; i < file_num; i++)
   {
      int *duty_dum = new int;
      *duty_dum = i;
      thread(requester, (void *)duty_dum);
   }
   thread(receiver, nullptr);
   return;
}

int main(int argc, char *argv[])
{
   max_disk_queue = 3;
   file_num = 5;
   req_num = file_num;
   current_active_threads = file_num;
   requestsCollector.resize(file_num);
   std::queue<int> qtmp;
   qtmp.push(785);
   qtmp.push(53);
   intermediate.push_back(qtmp);
   qtmp = {};
   qtmp.push(350);
   qtmp.push(914);
   intermediate.push_back(qtmp);
   qtmp = {};
   qtmp.push(827);
   qtmp.push(567);
   intermediate.push_back(qtmp);
   qtmp = {};
   qtmp.push(302);
   qtmp.push(230);
   intermediate.push_back(qtmp);
   qtmp = {};
   qtmp.push(631);
   qtmp.push(11);
   intermediate.push_back(qtmp);
   if (max_disk_queue > file_num)
      max_disk_queue = file_num;
   cpu::boot(1, (thread_startfunc_t)scheduler, nullptr, false, false, 0);
   return 0;
}