#include "thread_pool.h"

thread_pool::thread_pool(int _max, int _min) {
    taskQ = new task_queue;
    minNum = _min;
    maxNum = _max;
    busyNum = 0; 
    aliveNum = _min;     
    exitNum = 0;  

    // 根据线程最大上限分配内存
    threadIDs = new std::thread[maxNum];
    if (threadIDs == nullptr) {
        std::cout << "thread_pool(): new thread[maxNum] error" << std::endl;
    }
}
