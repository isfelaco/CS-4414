#include "life.h"
#include <pthread.h>
#include <vector>
using namespace std;

// prototype for struct to be passed as arg to the thread function func
struct ThreadInfo {
    int Thread_id;
    int steps;
    LifeBoard state;
    LifeBoard next_state;
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
    LifeBoard* state = &(info->state);
    LifeBoard* next_state = &(info->next_state);
    pthread_barrier_t* barrier = info->barrier;

    // Wait behind barrier until other threads finish their job for this step
    int num_threads = pthread_barrier_wait(barrier);

    // assign a chunk of state to a thread based on a predefined thread id
    int rows_per_thread = state->height() / num_threads;
    int start_row = thread_id * rows_per_thread;
    int end_row = (thread_id == num_threads - 1) ? state->height() : start_row + rows_per_thread;

    // Count live neighbours and Update next state is the same as single thread
    for (int generation = 0; generation < steps; generation++) {
        // Process the assigned portion of the board
        for (int x = start_row; x < end_row; x++) {
            for (int y = 0; y < state->width(); y++) {
                int live_in_window = 0;
                /* For each cell, examine a 3x3 "window" of cells around it,
                 * and count the number of live (true) cells in the window. */
                for (int y_offset = -1; y_offset <= 1; ++y_offset) {
                    for (int x_offset = -1; x_offset <= 1; ++x_offset) {
                        if (state->at(x + x_offset, y + y_offset)) {
                            ++live_in_window;
                        }
                    }
                }
                /* Cells with 3 live neighbors remain or become live.
                   Live cells with 2 live neighbors remain live. */
                next_state->at(x, y) = (
                    live_in_window == 3 /* dead cell with 3 neighbors or live cell with 2 */ ||
                    (live_in_window == 4 && state->at(x, y)) /* live cell with 3 neighbors */
                );
            }
        }
        swap(state, next_state);
    }

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
        threadInfoList[i].state = state;
        threadInfoList[i].next_state = next_state;
        threadInfoList[i].barrier = &barrier;

        pthread_create(&threadList[i], NULL, &func, &threadInfoList[i]);
    }

    // wait for all threads to finish
    for (int i = 0; i < threads; i++) {
        pthread_join(threadList[i], NULL);
    }

    pthread_barrier_destroy(&barrier);
}
