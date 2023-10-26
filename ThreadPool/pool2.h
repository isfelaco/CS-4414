#ifndef POOL_H_
#include <string>
#include <pthread.h>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <map>
#include <algorithm>
#include <iostream>
using namespace std;

class Task {
public:
    Task();
    virtual ~Task();

    virtual void Run() = 0;  // implemented by subclass
};

class MyTask : public Task {
public:
    MyTask(const string& name, Task* task, pthread_cond_t* task_condition)
        : task(task), name(name), completed(false), task_condition(task_condition) {}

    bool IsCompleted() const {
        return completed;
    }

    string getName() const {
        return name;
    }

    Task* getTask() {
        return task;
    }

    void Run() override {
        task->Run();
        MarkCompleted();
    }
private:
    Task* task;
    const string name;
    bool completed = false;
    pthread_cond_t* task_condition;
    
    void MarkCompleted() {
        completed = true;
        pthread_cond_signal(task_condition);
    }
};

class ThreadPool {
public:
    ThreadPool(int num_threads);

    // Submit a task with a particular name.
    void SubmitTask(const string &name, Task *task);
 
    // Wait for a task by name, if it hasn't been waited for yet. Only returns after the task is completed.
    void WaitForTask(const string &name);

    // Stop all threads. All tasks must have been waited for before calling this.
    // You may assume that SubmitTask() is not called after this is called.
    void Stop();
private:
    int num_threads;
    bool stop;
    vector<pthread_t> threads;
    queue<MyTask *> task_queue;
    pthread_mutex_t task_mutex;
    pthread_cond_t task_condition;
    static void* Worker(void* arg);
};
#endif
