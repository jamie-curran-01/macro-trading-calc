# include <stdio.h>
# include <pthread.h>
# include <unistd.h>
# include <cstdlib>

# include "network_client.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


int completed_tasks = 0;  // Shared resource to track completed tasks
const int num_threads = 3;

void* child_thread(void* arg) {
    int id = *((int*)arg);
    free(arg);  // Free the allocated memory for the id

    // Simulate some work with independent progress updates
    
    if(id == 0){
	printf("setting up networking stack ... \n");
	start_client();
	sleep(20);
    	printf("network setup, getting data \n");
    }
    else if(id == 1){
	printf("setting up parser, awaiting data from networking stack \n");
	sleep(10);
        printf("parser reading data, lets go... \n");
    }
    else if(id == 2){
	printf("gpu stack setup \n");
        sleep(40);
        printf("gpu stack cooked ready to go! \n");
    }
    else{
	printf("somethings gone wrong I think? \n");
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
