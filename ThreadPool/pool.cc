#include "pool.h"

void* func(void* arg) {
    // thread should run a task if one is allotted, FIFO
    // else they should probably be sleeping/waiting
    // pseudocode? similar to consumer code (with submittask as producer?)
    /* 
    lock mutex
    while (no processes to run) {
        wait(task_ready, lock)
    }
    pop off task at front of queue to run on thread
    unlock mutex
    */

}

// what to do about run method?
// task constructor
Task::Task() {
    // define mutexes and condvars here in .h file?
    
}

// task destructor
Task::~Task() {
    // can destroy mutexes and condvars here in
}

ThreadPool::ThreadPool(int num_threads) {
    // threadpool manages worker threads (a given number!)
    // N never changes, should only call pthread_create N times
    // ^^ create threads here!
    // threads have one job: to run tasks via task::run()

    //std::vector<pthread_t> pool(num_threads); // do we need this? to keep track of threads
    //std::vector<Task> tasklist;
    //pthread_mutex_t task_lock; // to lock the task queue whenever we need to edit it
    
    int mutex_res = pthread_mutex_init(&task_lock, NULL); // initialize mutex
    if (mutex_res != 0) {
        perror("pthread_mutex_init");
    }

    for (int i = 0; i < num_threads; i++) {
        int result = pthread_create(&pool[i], NULL, &func, NULL); // create only N threads
        // edit this if we need args?
        if (result != 0) {
            perror("pthread_create");
        }

    }

    // either when the task completes or when the task is waited for, threadpool deallocates with delete task

    // define mutexes and condvars here (are in .h file)
}

// Submit a task with a particular name.
void ThreadPool::SubmitTask(const std::string &name, Task* task) {
    // add a task for the thread pool to run
    // (add task to fifo queue and let worker threads know it is available)
    // should not block (what does this mean)

    pthread_mutex_lock(&task_lock); // to make sure no address conflicts when editing the queue?

    tasklist.push_back(task);

    pthread_cond_broadcast(&task_ready); // to tell the threads to wake up to run the task
    pthread_mutex_unlock(&task_lock);

    return;

    // should return immediately after adding task to a queue
    // postcondition: task should be in the queue but does not need to be running immediately 
}

// Wait for a task by name, if it hasn't been waited for yet. Only returns after the task is completed.
void ThreadPool::WaitForTask(const std::string &name) {
    // block execution until task is done?
    // block if task is in queue! block if thread is executing task!
    // don't block if task's run() is completed, erase task from queue

    // do not waste cycles in lock check unlock loop
    // << use condition variables/signal instead? >>
    // sleep the threads that need to wait instead of putting them in a loop

    // don't return until task completes
    
    // thread has completed executing run() of task with given name
}

// Stop all threads. All tasks must have been waited for before calling this.
// You may assume that SubmitTask() is not called after this is called.
void ThreadPool::Stop() {
    // cleanly stop execution and exit all worker threads
    // do not abruptly kill workers, wait for everyone to finish
    // hint: can use boolean flag for stop variable
    // wake up all threads and wait for them to finish, use pthread_join
    // destroy lock and mutexes at the end

}
