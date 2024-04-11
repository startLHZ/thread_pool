#include "../include/task_queue.h"
#include "../include/thread_pool.h"
#include <unistd.h>

void taskFunc(void* arg)
{
    int num = *(int*)arg;
    printf("thread %ld is working, number = %d\n", pthread_self(), num);
    // sleep(1);
}

int main()
{
    // 创建线程池
    thread_pool* pool = new thread_pool(3);
    for (int i = 0; i < 100000; ++i)
    {
        int* num = (int*)malloc(sizeof(int));
        *num = i + 100;
        Task task;
        task.function = taskFunc;
        task.arg = num;
        pool->add_task(task);
    }
    sleep(3);
    int max_queue = pool->max_task_queue_size;
    int max_alive_thread = pool->max_alive_num;
    // std::cout << "max taskQ.size(): " << pool->max_task_queue_size << std::endl;
    // std::cout << "max alive thread: " << pool->max_alive_num << std::endl;
    delete pool;
    std::cout << "maxTaskQ: " << max_queue << "\n" << "max alive thread: " << max_alive_thread <<\
    std::endl;

    return 0;
}