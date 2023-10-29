#include "pool.h"
#include <pthread.h>


Task::Task() {}

Task::~Task() {}

ThreadPool::ThreadPool(int num_threads) : num_threads(num_threads), stop(false) {
    pthread_mutex_init(&queue_mutex, nullptr);
    pthread_mutex_init(&completion_mutex, nullptr);
    pthread_cond_init(&condition, nullptr);
    pthread_cond_init(&completion_condition, nullptr);
    
    for (int i = 0; i < num_threads; ++i) {
        pthread_t thread;
        pthread_create(&thread, nullptr, Worker, this);
        threads.push_back(thread);
    }
}

void* ThreadPool::Worker(void* arg) {
    ThreadPool* pool = static_cast<ThreadPool*>(arg);

    while (true) {
        pair<string, Task*> task;

        pthread_mutex_lock(&pool->queue_mutex);
        while (pool->task_queue.empty() && !pool->stop) {
            pthread_cond_wait(&pool->condition, &pool->queue_mutex);
        }

        bool shouldStop = pool->stop;
        if (shouldStop && pool->task_queue.empty()) {
            pthread_mutex_unlock(&pool->queue_mutex);
            break;
        }

        if (!pool->task_queue.empty()) {
            task = pool->task_queue.front();
            pool->task_queue.pop_front();
        }

        pthread_mutex_unlock(&pool->queue_mutex);
        

        if (!task.second) {
            // No task available, exit the thread.
            break;
        }

        task.second->Run();

        pthread_mutex_lock(&pool->completion_mutex);
        pool->completed_tasks.insert(task);
        pthread_cond_broadcast(&pool->completion_condition);
        pthread_mutex_unlock(&pool->completion_mutex);
        
        delete task.second;
    }
    return nullptr;
}

void ThreadPool::SubmitTask(const string &name, Task* task) {
    pthread_mutex_lock(&queue_mutex);
    task_queue.push_back(make_pair(name, task));
    pthread_mutex_unlock(&queue_mutex);
    pthread_cond_signal(&condition);
}

void ThreadPool::WaitForTask(const std::string &name) {
    pthread_mutex_lock(&completion_mutex);
    // task is not in list of completed tasks and stop has not been called
    while (!completed_tasks.count(name) && !stop) {
        pthread_cond_wait(&completion_condition, &completion_mutex); // wait on the task
    }
    pthread_mutex_unlock(&completion_mutex);
}

void ThreadPool::Stop() {
    pthread_mutex_lock(&queue_mutex);
    stop = true;
    pthread_mutex_unlock(&queue_mutex);
    
    pthread_cond_broadcast(&condition);

    for (pthread_t thread : threads) {
        pthread_join(thread, nullptr);
    }

    while (!task_queue.empty()) {
        Task* thisTask = task_queue.front().second;
        delete thisTask;
        task_queue.pop_front();
    }

    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&condition);

    pthread_mutex_destroy(&completion_mutex);
    pthread_cond_destroy(&completion_condition);
}
