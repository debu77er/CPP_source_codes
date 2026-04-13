#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // for sleep

// compilation: g++ -o semaphore semaphore.cpp -lpthread


// Shared semaphore
sem_t semaphore;

void* worker_thread(void* arg) {
    std::cout << "Worker thread started, doing some work...\n";
    sleep(2); // Simulate work with sleep
    std::cout << "Worker thread finished work, posting to semaphore.\n";

    // Signal the semaphore
    sem_post(&semaphore);
    return nullptr;
}

void* dependent_thread(void* arg) {
    std::cout << "Dependent thread waiting for semaphore...\n";

    // Wait on the semaphore
    sem_wait(&semaphore);

    std::cout << "Dependent thread detected semaphore post, proceeding.\n";

    return nullptr;
}

int main() {
    // Initialize semaphore with value 0
    if (sem_init(&semaphore, 0, 0) != 0) {
        perror("sem_init failed");
        return 1;
    }

    pthread_t thread1, thread2;

    // Create threads
    if (pthread_create(&thread1, nullptr, worker_thread, nullptr) != 0) {
        perror("Failed to create worker thread");
        sem_destroy(&semaphore);
        return 1;
    }

    if (pthread_create(&thread2, nullptr, dependent_thread, nullptr) != 0) {
        perror("Failed to create dependent thread");
        pthread_cancel(thread1);
        sem_destroy(&semaphore);
        return 1;
    }

    // Wait for threads to finish
    pthread_join(thread1, nullptr);
    pthread_join(thread2, nullptr);

    // Destroy semaphore
    sem_destroy(&semaphore);

    std::cout << "Main thread finished.\n";

    return 0;
}
