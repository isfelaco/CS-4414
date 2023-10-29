#ifndef POOL_H_
#include <string>
#include <pthread.h>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <string>
#include <iostream>
using namespace std;

class Task {
public:
    Task();
    virtual ~Task();

    virtual void Run() = 0;  // implemented by subclass
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
    static void* Worker(void* arg);

    int num_threads;
    bool stop;

    vector<pthread_t> threads;
    deque<pair<string, Task*>> task_queue;
    map<string, Task*> completed_tasks;

    pthread_mutex_t queue_mutex;
    pthread_mutex_t completion_mutex;

    pthread_cond_t condition;
    pthread_cond_t completion_condition;
};
#endif
