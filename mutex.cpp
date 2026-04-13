#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

// g++ -o mutex mutex.cpp -lpthread


// Shared resource
int counter = 0;

// Mutex to protect access to shared resource
std::mutex counter_mutex;

// Function to be run by each thread
void incrementCounter(int iterations) {
    for (int i = 0; i < iterations; ++i) {
        // Lock the mutex before accessing the shared resource
        std::lock_guard<std::mutex> lock(counter_mutex);
        ++counter;
        // Mutex is automatically released when lock goes out of scope
    }
}

int main() {
    const int num_threads = 4;       // Number of threads
    const int iterations = 1000;     // Number of increments per thread

    // Create a vector to hold thread objects
    std::vector<std::thread> threads;

    // Launch threads
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(incrementCounter, iterations);
    }

    // Join threads
    for (auto& t : threads) {
        t.join();
    }

    // Output the result
    std::cout << "Final counter value: " << counter << std::endl;

    return 0;
}
