#include "../include/task_queue.h"

task_queue::task_queue() {
    
}

task_queue::~task_queue() {

}

void task_queue::add_task(Task task) {
    std::unique_lock<std::mutex> locker(this->m_mtx);
    this->my_queue.push(task);
    locker.unlock();
}

void task_queue::add_task(callback func, void* arg) {
    std::unique_lock<std::mutex> locker(this->m_mtx);
    this->my_queue.push(Task(func, arg));
    locker.unlock();
}

Task task_queue::takeTask() {
    Task getTask;
    std::unique_lock<std::mutex> locker(this->m_mtx);
    getTask = my_queue.front();
    my_queue.pop();
    locker.unlock();
    return getTask;
}