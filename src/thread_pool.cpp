#include "thread_pool.h"

thread_pool::thread_pool(int _max, int _min) {
    taskQ = new task_queue;
    minNum = _min;
    maxNum = _max;
    busyNum = 0; 
    aliveNum = _min;     
    exitNum = 0;  

    // 根据线程最大上限分配内存
    vector<std::thread> threadIDs = new std::thread(maxNum);
    if (threadIDs == nullptr) {
        std::cout << "thread_pool(): new thread[maxNum] error" << std::endl;
    }

    // *******创建线程***************
    // 根据最小线程数创建线程
    for (int i = 0; i < minNum; i ++) {
        threadIDs[i] = std::thread(worker, this);
        std::cout << threadIDs[i] << std::endl;
    }
    managerID = std::thread(manager, this);
    return ;
}

thread_pool::~thread_pool() {
    std::unique_lock<std::mutex> locker(this->pool_mtx);
    this->shutdown = true;
    notEmpty.notify_all();
    for (int i = 0; i <= this->aliveNum; i ++) {
        if (threadIDs[i] == 0) continue;
        threadIDs[i].join();
    }
}

void* thread_pool::worker(void* arg) {
    thread_pool pool = (thread_pool*) arg;

    while (1) {
        std::unique_lock(std::mutex) locker(this->pool_mtx);
        while (pool->takeTask.get_task_number() == 0) {
            pool->notEmpty.wait(locker);
            // 是否销毁线程
            if (pool->exitNum > 0) {
                pool->exitNum --;
                if (pool->aliveNum > pool->minNum) {
                    pool->aliveNum--;
                    locker.unlock();
                    return;
                }
            }
        }
        // 线程池是否关闭   
        if (pool->shutdown) {
            locker.unlock();
            return ;
        }
        pool->busyNum ++;
        Task task = pool->taskQ->takeTask();
        locker.unlock();
        task.function(task.arg);

        std::unique_lock(std::mutex) locker(this->pool_mtx);
        pool->busyNum --;
        locker.unlock();
    }
    return nullptr;
}

void* thread_pool::manager(void* arg) {

}

void thread_pool::add_task(Task task) {
    std::unique_lock<std::mutex> locker(pool_mtx);
    task_queue.push(task);
    notEmpty.notify_one();
    locker.unlock();
}

void thread_pool::sigle_thread_exit() {
    std::thread tid = this_thread();
    for (int i = 0; i < this->maxNum; i ++) {
        threadIDs[i] = 0;
        break;
    }
}
