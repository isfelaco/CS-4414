#include "pool.h"


/*
Implement a thread pool, which sets up a fixed number of threads to service incoming tasks.
Each of these threads should take tasks from a queue and run them, waiting for new tasks
whenever the queue is empty. Your thread pool will start tasks in the order they are
submitted and also support waiting for a particular task submitted to thread pool to complete.
Each task in the thread pool will be identified by a name.
*/

Task::Task() {
    /*
        define mutex or condvars
        you can have destructor to destroy them
    */
}

Task::~Task() {
}

ThreadPool::ThreadPool(int num_threads) : stop(false) {
    /* 
        num_threads calls to pthread_create
        each thread runs Tasks via Task:Run()
        define mutex or condvars
        create threads
            what args does the worker function need?
    */

     // Initialize synchronization primitives
    pthread_mutex_init(&task_mutex, nullptr);
    pthread_cond_init(&task_condition, nullptr);

    // Create worker threads
    for (int i = 0; i < num_threads; ++i) {
        pthread_t thread;
        pthread_create(&thread, NULL, Worker, this);
        threads.push_back(thread);
    }
}

void* ThreadPool::Worker(void* arg) {
    /*
        if there is no task available, wait
        take a task off the queue
        run task, then notify waiting threads
        always check if Stop() has been called
    */
    ThreadPool* pool = static_cast<ThreadPool*>(arg);

    while (true) {
        pthread_mutex_lock(&pool->task_mutex);

        while (pool->task_queue.empty() && !pool->stop) {
            pthread_cond_wait(&pool->task_condition, &pool->task_mutex);
        }

        if (pool->stop) {
            pthread_mutex_unlock(&pool->task_mutex);
            pthread_exit(NULL);
        }

        MyTask* thisTask = pool->task_queue.front();
        pool->task_queue.pop();
        pthread_mutex_unlock(&pool->task_mutex);

        if (thisTask->getTask()) {
            thisTask->getTask()->Run();
            delete thisTask->getTask();
        }
    }

    return NULL;
}

// Submit a task with a particular name.
void ThreadPool::SubmitTask(const string &name, Task* task) {
    /* 
        add task for the thread pool to run
            add the task to your FIFO queue
            let worker threads know work is available
        should not block
        postcondition: the Task is in the queue
            the task is not necessarily running before return
    */
    pthread_mutex_lock(&task_mutex);
    MyTask* myTask = new MyTask(name, task, &task_condition);
    task_queue.push(myTask);
    pthread_mutex_unlock(&task_mutex);
    pthread_cond_signal(&task_condition);
}

// Wait for a task by name, if it hasn't been waited for yet. Only returns after the task is completed.
void ThreadPool::WaitForTask(const string &name) {
    /* 
        block execution until named task finishes
            task still in queue? block
            thread executing Task? block
            Task's Run() completed? do not block
                erase the task from queue
        do not waste cycles in a lock-check-unlock loop
        postcondition: a thread has completed executing the Run() function of Task named by name
    */
    pthread_mutex_lock(&task_mutex);

    queue<MyTask *> temp_queue;

    // Search for the task in the queue and mark it for waiting
    while (!task_queue.empty()) {
        MyTask* thisTask = task_queue.front();
        if (thisTask->getName() == name ) {
            task_queue.pop();
            while (!thisTask->IsCompleted()) {
                cout << "HERE" << endl;
                // Wait for the task's completion using a conditional variable
                pthread_cond_wait(&task_condition, &task_mutex);
                cout << "HERE2" << endl;

            }
            break;
        } else {
            temp_queue.push(thisTask);
        }
    }

    task_queue = temp_queue; // replace with task removed queue

    pthread_mutex_unlock(&task_mutex);
}

// Stop all threads. All tasks must have been waited for before calling this.
// You may assume that SubmitTask() is not called after this is called.
void ThreadPool::Stop() {
    /*
        cleanly cease execution, exit all worker threads
        do no abruptly kill your workers
        hint: you can use a boolean flag for stop variable
        wake up all the threads and wait for them to finish
            use pthread_join
        do not forget to destroy lock and mutexes at the end
    */
   // Set the stop flag to notify worker threads to exit
    stop = true;
    
    // Signal all threads to wake up
    pthread_cond_broadcast(&task_condition);
    
    // Join all worker threads to wait for them to finish
    for (pthread_t thread : threads) {
        pthread_join(thread, NULL);
    }
    
    // Ensure all tasks have been executed and deleted
    while (!task_queue.empty()) {
        delete task_queue.front()->getTask();
        delete task_queue.front();
        task_queue.pop();
    }
    
    // Destroy synchronization primitives
    pthread_mutex_destroy(&task_mutex);
    pthread_cond_destroy(&task_condition);
}
