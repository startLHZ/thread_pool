#pragma once
#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "task_queue.h"

class thread_pool 
{
public:
    // 创建线程池，构造函数
    thread_pool(int _max, int _min);

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
private:
    // 工作的线程任务函数
    void* worker(void* arg);

    // 管理者线程任务函数
    void* manager(void* arg);

    // 单个线程退出
    void sigle_thread_exit();

private:
    std::mutex pool_mtx;                 //线程池的锁
    std::condition_variable notEmpty;    //条件变量，队列不为空
    std::thread* threadIDs;              //工作的线程ID
    std::thread managerID;               //管理的线程ID
    task_queue* taskQ;                 //任务队列
    int minNum;               //最小线程数量
    int maxNum;               //最大线程数量
    int busyNum;              //在忙线程数
    int aliveNum;             //存活线程数
    int exitNum;              //需要销毁线程数

    bool shutdown;    //是不是要销毁线程池，销毁为1，不销毁为0
};