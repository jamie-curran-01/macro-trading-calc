# include <stdio.h>
# include <pthread.h>
# include <unistd.h>
#include <cstdlib>



pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


int completed_tasks = 0;  // Shared resource to track completed tasks
const int num_threads = 50;

void* child_thread(void* arg) {
    int id = *((int*)arg);
    free(arg);  // Free the allocated memory for the id

    // Simulate some work with independent progress updates
    for (int i = 0; i < 3; ++i) { // Assuming each thread does 3 units of work
        printf("Thread %d, workload %d\n", id, i + 1);
        sleep(1); // Simulate work taking time
    }

    // Lock the mutex before modifying shared resources
    pthread_mutex_lock(&mutex);
    completed_tasks++;
    if (completed_tasks == num_threads) {
        pthread_cond_signal(&cond);  // Signal the main thread that all tasks are completed
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    pthread_t threads[num_threads];

    // Start child threads
    for (int i = 0; i < num_threads; ++i) {
        int* id = (int*)malloc(sizeof(int));
        *id = i;
        pthread_create(&threads[i], NULL, child_thread, (void*)id);
    }

    // Main loop: wait for all child threads to complete
    pthread_mutex_lock(&mutex);
    while (completed_tasks < num_threads) {
        // Put the main thread to sleep until a child thread signals completion
        pthread_cond_wait(&cond, &mutex);
    }
    printf("All tasks completed\n");
    pthread_mutex_unlock(&mutex);

    // Clean up
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
