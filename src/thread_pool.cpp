#include "../include/thread_pool.h"
#include <vector>
#include <thread>
#include <mutex>
#include <unistd.h>

thread_pool::thread_pool(int _min) 
        : shutdown(false), busyNum(0), aliveNum(_min), exitNum(0) 
{
    taskQ = new task_queue;
    minNum = _min;
    std::unique_lock<std::mutex> locker(pool_mtx, std::defer_lock);
    locker.lock();
    // auto t = std::thread(manager,this);
    // auto id = t.get_id();
    threadIDs.push_back(std::thread(manager,this));
    aliveNum ++;
    // managerID = std::thread(&manager, this);
    make_thread(minNum);      
    locker.unlock();
}

thread_pool::~thread_pool() {
    shutdown = true;
    for (int i  = 0; i < threadIDs.size(); ++ i) {
        // notEmpty.notify_all();
        notEmpty.notify_one();
        if(threadIDs[i].joinable()){
            threadIDs[i].join(); // 等待任务结束， 前提：线程一定会执行完
        }     
    }
    // if (managerID.joinable()) {
    //     managerID.join();
    // }
    return;
}

void* thread_pool::worker(void* arg) {
    thread_pool* pool = static_cast<thread_pool*>(arg);
    while (1)
    {
        std::unique_lock<std::mutex> locker(pool->pool_mtx);
        pool->notEmpty.wait(locker, [&]()->bool{ return pool->get_taskQ_size() || pool->if_shutdown();});
        if (pool->if_shutdown()) {
            pool->aliveNum --;
            locker.unlock();
            pool->notEmpty.notify_one();
            return nullptr;
        }
        if (pool->exitNum) {
            auto t = std::this_thread::get_id();
            for (auto i = pool->threadIDs.begin(); i != pool->threadIDs.end(); i ++) {
                if (i->get_id() == t) {
                    pool->threadIDs.erase(i, i + 1);
                }
            }
            pool->exitNum --;
            pool->aliveNum --;
            return nullptr;
        }
        auto curTask = pool->taskQ->takeTask();
        pool->busyNum ++;

        locker.unlock();


        curTask.function(curTask.arg);


        locker.lock();
        pool->busyNum --;
        locker.unlock();
    }
}

void thread_pool::add_task(Task task) {
    if (shutdown) return ;
    std::unique_lock<std::mutex> locker(pool_mtx, std::defer_lock);
    locker.lock();
    taskQ->add_task(task);
    locker.unlock();
    notEmpty.notify_one();
}

void* thread_pool::manager(void* arg) {
    thread_pool* pool = static_cast<thread_pool*>(arg);
    while (1) {
        sleep(1);
        std::unique_lock<std::mutex> locker(pool->pool_mtx, std::defer_lock);
        locker.lock();
        if (pool->if_shutdown()) {
            pool->aliveNum --;
            locker.unlock();
            pool->notEmpty.notify_one();
            return nullptr;
        }
        
        int busy = pool->get_busyNum(), alive = pool->get_aliveNum(), taskNum = pool->get_taskQ_size();
        if (taskNum > 100) pool->make_thread(taskNum / 3);
        if (taskNum == 0) pool->kill_worker(10);

        // -------------------
        // 测试性能，max task queue size and max aliveNum
        pool->max_alive_num = pool->max_alive_num > alive ? pool->max_alive_num : alive;
        pool->max_task_queue_size = pool->max_task_queue_size > taskNum ? pool->max_task_queue_size : taskNum;
        // -------------------

        locker.unlock();
    }
}

void thread_pool::make_thread(int n) {
    // 不需要加锁
    if (aliveNum + n > 8000) {
        n = 8000 - aliveNum;
    }
    aliveNum += n;
    for (int i = 0; i < n; ++ i) {
        threadIDs.push_back(std::thread(&worker, this));
        // threadIDs.back().detach();
    }  
}

void thread_pool::kill_worker(int n) {
    exitNum += n;
    exitNum = exitNum > aliveNum - 1 ? aliveNum - 1 : exitNum;
}
