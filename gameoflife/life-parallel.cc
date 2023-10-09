#include "life.h"
#include <pthread.h>
#include <vector>
using namespace std;

// prototype for struct to be passed as arg to the thread function func
struct {
    int Thread_id;
    int steps;
    LifeBoard state;
    LifeBoard next_state;
    pthread_barrier_t barrier;

} threadInfo;

void* func(void* arg) {
    // prototype for the function that tells each thread what it needs to do
    // arg can be a STRUCT with all the variables a thread has (Thread_id, steps, states, barrier, etc)

   /* have each thread work on a different part of the board? 
    divide the number of items in the grid by number of threads, with some leftover (one thread takes the uneven amt?)
    each thread calculates its amount and adds it to the next_state 
    all threads need to wait on barrier?
    barrier will allow them all to begin 
    */
    pthread_exit(NULL);
}

void simulate_life_parallel(int threads, LifeBoard &state, int steps) {
    /* YOUR CODE HERE
    initialize next state board (done?)
    initialize barrier (done?)
    create thread vector and initialize each thread
    using pthread_create()
    join() all threads using pthread_join
     */
    LifeBoard next_state{state.width(), state.height()}; // initialize next state board?
    pthread_barrier_t barrier;
    // initialize barrier
    pthread_barrier_init(&barrier, NULL, threads); // not sure if i should use & or *  or not? this at least compiles

 

    // make vector of threads and init each one?
    vector<pthread_t> threadList;
    for (int i = 0; i < threads; i++) {
        pthread_t ithread;
        pthread_create(&ithread, NULL, &func, NULL); // initializing each thread?
        threadList.push_back(ithread); // will this do anything?
        // how to pass struct as arg?
        // how to get thread id?
    }

    // allocate threads to each contiguous part of the board? (divide?)
    // have them all calculate their areas in the func function and then wait
    // free barrier and swap?
}
