#include "life.h"
#include <pthread.h>
#include <vector>
using namespace std;

// prototype for struct to be passed as arg to the thread function func
struct ThreadInfo {
    int Thread_id;
    int steps;
    LifeBoard* state;
    LifeBoard* next_state;
    pthread_barrier_t* barrier;
};

void* func(void* arg) {
    /* 
    prototype for the function that tells each thread what it needs to do
    arg can be a STRUCT with all the variables a thread has (Thread_id, steps, states, barrier, etc)
    */

    ThreadInfo* info = static_cast<ThreadInfo*>(arg);

    // Extract the thread-specific information
    int thread_id = info->Thread_id;
    int steps = info->steps;
    LifeBoard* state = info->state;
    LifeBoard* next_state = info->next_state;
    pthread_barrier_t* barrier = info->barrier;

    /*
    assign a chunk of state to a thread based on a predefined thread id
    Note: cells at the edge of the grid are always dead (0)
    Count live neighbours and Update next state is the same as single thread
    Wait behind barrier until other threads finish their job for this step
    swap
    */

   /* have each thread work on a different part of the board? 
    divide the number of items in the grid by number of threads, with some leftover (one thread takes the uneven amt?)
    each thread calculates its amount and adds it to the next_state 
    all threads need to wait on barrier?
    barrier will allow them all to begin 
    */

    pthread_barrier_wait(barrier);
    pthread_exit(NULL);
}

void simulate_life_parallel(int threads, LifeBoard &state, int steps) {
    /* YOUR CODE HERE
    initialize next state board (done)
    initialize barrier (done)
    create thread vector and initialize each thread using pthread_create() (done)
    join() all threads using pthread_join (done)
    */

    // next_state as pointer to a LifeBoard
    LifeBoard next_state{state.width(), state.height()};
    
    // initialize barrier
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, threads);

    // make vector of threads
    vector<pthread_t> threadList(threads);
    vector<ThreadInfo> threadInfoList(threads);

    for (int i = 0; i < threads; i++) {
        threadInfoList[i].Thread_id = i;
        threadInfoList[i].steps = steps;
        threadInfoList[i].state = &state;
        threadInfoList[i].next_state = &next_state;
        threadInfoList[i].barrier = &barrier;

        pthread_create(&threadList[i], NULL, &func, &threadInfoList[i]);
    }

    // wait for all threads to finish
    for (int i = 0; i < threads; i++) {
        pthread_join(threadList[i], NULL);
    }

    pthread_barrier_destroy(&barrier);

    // swap state and next_state ?

    // allocate threads to each contiguous part of the board? (divide?)
    // have them all calculate their areas in the func function and then wait
    // free barrier and swap?
}
