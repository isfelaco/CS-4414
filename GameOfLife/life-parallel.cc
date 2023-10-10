#include "life.h"
#include <pthread.h>
#include <vector>
#include <iostream>
using namespace std;

// prototype for struct to be passed as arg to the thread function func
struct ThreadInfo {
    int Thread_id;
    int steps;
    int num_threads;
    LifeBoard* even_state;
    LifeBoard* odd_state;
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
    int num_threads = info->num_threads;
    int steps = info->steps;
    LifeBoard* even_state = info->even_state;
    LifeBoard* odd_state = info->odd_state;
    pthread_barrier_t* barrier = info->barrier;

    int height = even_state->height();

    int rows_per_thread = (height - 2) / num_threads;
    int start_row = 1 + thread_id * rows_per_thread;
    int end_row = (thread_id == num_threads - 1) ? height - 1 : start_row + rows_per_thread;

    // Count live neighbours and Update next state is the same as single thread
    for (int step = 0; step < steps; ++step) {
        LifeBoard* current_generation = (step % 2 == 0) ? even_state : odd_state;
        LifeBoard* next_generation = (step % 2 == 0) ? odd_state : even_state;
        // Process the assigned portion of the board
        for (int y = start_row; y < end_row; ++y) {
            for (int x = 1; x < current_generation->width()-1; ++x) {
                int live_in_window = 0;
                /* For each cell, examine a 3x3 "window" of cells around it,
                 * and count the number of live (true) cells in the window. */
                for (int y_offset = -1; y_offset <= 1; ++y_offset) {
                    for (int x_offset = -1; x_offset <= 1; ++x_offset) {
                        if (current_generation->at(x + x_offset, y + y_offset)) {
                            ++live_in_window;
                        }
                    }
                }
                /* Cells with 3 live neighbors remain or become live.
                   Live cells with 2 live neighbors remain live. */
                next_generation->at(x, y) = (
                    live_in_window == 3 /* dead cell with 3 neighbors or live cell with 2 */ ||
                    (live_in_window == 4 && current_generation->at(x, y)) /* live cell with 3 neighbors */
                );
            }
        }
        pthread_barrier_wait(barrier);
    }
    pthread_exit(NULL);
}

void simulate_life_parallel(int threads, LifeBoard &state, int steps) {
    LifeBoard even_state{state.width(), state.height()};
    LifeBoard odd_state{state.width(), state.height()};

    for (int y = 0; y < state.height(); ++y) {
        for (int x = 0; x < state.width(); ++x) {
            even_state.at(x, y) = state.at(x, y);
            odd_state.at(x, y) = state.at(x, y);
        }
    }
    
    // initialize barrier
    pthread_barrier_t barrier;
    int result = pthread_barrier_init(&barrier, NULL, threads);
    if (result != 0) {
        perror("pthread_barrier_init");
    }

    // make vector of threads
    vector<pthread_t> threadList(threads);
    vector<ThreadInfo> threadInfoList(threads);
    for (int i = 0; i < threads; i++) {
        threadInfoList[i].Thread_id = i;
        threadInfoList[i].steps = steps;
        threadInfoList[i].num_threads = threads;
        threadInfoList[i].even_state = &even_state;
        threadInfoList[i].odd_state = &odd_state;
        threadInfoList[i].barrier = &barrier;

        int result = pthread_create(&threadList[i], NULL, &func, &threadInfoList[i]);
        if (result != 0) {
            perror("pthread_create");
        }
    }

    // wait for all threads to finish
    for (int i = 0; i < threads; i++) {
        void* exit_status;
        pthread_join(threadList[i], &exit_status);
        if (exit_status != 0) {
            perror("pthread_join");
        }
    }

    int destroy_result = pthread_barrier_destroy(&barrier);
    if (destroy_result != 0) {
        perror("pthread_barrier_destroy");
    }

    if (steps % 2 == 0) {
        state = even_state;
    } else {
        state = odd_state;
    }
}
