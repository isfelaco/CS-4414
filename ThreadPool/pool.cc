#include "pool.h"

Task::Task() {
}

Task::~Task() {
}

ThreadPool::ThreadPool(int num_threads) {
}

// Submit a task with a particular name.
void ThreadPool::SubmitTask(const std::string &name, Task* task) {
}

// Wait for a task by name, if it hasn't been waited for yet. Only returns after the task is completed.
void ThreadPool::WaitForTask(const std::string &name) {
}

// Stop all threads. All tasks must have been waited for before calling this.
// You may assume that SubmitTask() is not called after this is called.
void ThreadPool::Stop() {
}
