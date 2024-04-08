# pragma once
#include <iostream>
#include <queue>
#include <mutex>


// # include"../src/task_queue.cpp"

using callback = void (*)(void* arg);

class Task{
public:
    // Task() {
    //     this->function = nullptr;
    //     this->arg = nullptr;
    // }
    // Task(callback func, void* arg) {
    //     this->function = func;
    //     this->arg = arg;
    // } 
    Task(callback func = nullptr, void* arg = nullptr) : function(func), arg(arg){};
private:
    callback function;
    void* arg;
};

class task_queue
{
public:
    task_queue(/* args */);
    ~task_queue();
    // 添加任务
    void add_task(Task task);
    void add_task(callback func, void* arg);

    // 取出任务
    Task takeTask();

    // 获取当前任务队列个数
    inline int get_task_number() {
        return my_queue.size();
    }
private:
    /* data */
    std::mutex m_mtx;
    std::queue<Task> my_queue;
};


