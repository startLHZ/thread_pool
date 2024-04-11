#pragma once
#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <unordered_map>
#include "task_queue.h"

class thread_pool 
{
public:
    // 创建线程池，构造函数
    thread_pool(int _min);

    // 销毁线程池，析构函数
    ~thread_pool();

    // 给线程池添加任务
    void add_task(Task task);

    // 获取线程池中工作的线程数
    inline int get_busyNum() {
        return this->busyNum;
    }

    // 获取线程池中活着的线程数
    inline int get_aliveNum() {
        return this->aliveNum;
    }

    inline bool if_shutdown() {
        return this->shutdown;
    }

    inline int get_taskQ_size() {
        return taskQ->get_task_number();
    }

    void make_thread(int n);
    
    void kill_worker(int n);

private:
    // 工作的线程任务函数
    static void* worker(void* arg);

    // 管理者线程任务函数
    static void* manager(void* arg);

private:
    std::condition_variable notEmpty;    //条件变量，队列不为空
    std::vector<std::thread> threadIDs;  //工作的线程ID
    // std::unordered_map<std::thread::id, std::thread> threadIDs;
    std::thread managerID;               //管理的线程ID
    task_queue* taskQ;                   //任务队列
    int minNum;               //最小线程数量
    int maxNum;               //最大线程数量
    int busyNum;              //在忙线程数
    int aliveNum;             //存活线程数
    int exitNum;              //需要销毁线程数

    bool shutdown;    //是不是要销毁线程池，销毁为1，不销毁为0
public:
    std::mutex pool_mtx;                 //线程池的锁
    // // 性能指标
    int max_alive_num = 0;
    int max_task_queue_size = 0;
};