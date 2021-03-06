#ifndef CV_IMPL_H_
#define CV_IMPL_H_

#include <queue>
#include "cv.h"
#include "thread.h"

class cv::impl
{
public:
    // the queue of all threads waiting on this cv
    std::queue<thread::impl *> cv_waiting;
};

#endif