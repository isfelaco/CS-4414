#ifndef POOL_H_
#include <string>
#include <pthread.h>
#include <vector>

class Task {
public:
    Task();
    virtual ~Task();

    virtual void Run() = 0;  // implemented by subclass
};

class ThreadPool {
public:
    ThreadPool(int num_threads);
    std::vector<pthread_t> pool; // do we need this? to keep track of threads
    std::vector<Task*> tasklist;
    pthread_mutex_t task_lock; // to lock the task queue whenever we need to edit it
    pthread_cond_t task_ready; // for the threads to wait on and be signalled by


    // Submit a task with a particular name.
    void SubmitTask(const std::string &name, Task *task);
 
    // Wait for a task by name, if it hasn't been waited for yet. Only returns after the task is completed.
    void WaitForTask(const std::string &name);

    // Stop all threads. All tasks must have been waited for before calling this.
    // You may assume that SubmitTask() is not called after this is called.
    void Stop();
};
#endif
