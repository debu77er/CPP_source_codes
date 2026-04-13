#include <iostream>
#include <thread>
#include <atomic>
#include <vector>

// g++ -o spinlock spinlock.cpp -lpthread


// Simple spinlock implementation
class SpinLock {
public:
    SpinLock() : lock_flag(ATOMIC_FLAG_INIT) {}

    void lock() {
        // Busy-wait until we can set the flag to true
        while (lock_flag.test_and_set(std::memory_order_acquire)) {
            // Optionally, you can add pause instructions here for better performance
            // asm volatile("pause");
        }
    }

    void unlock() {
        lock_flag.clear(std::memory_order_release);
    }

private:
    std::atomic_flag lock_flag;
};

int main() {
    const int num_threads = 8;
    const int increments_per_thread = 100000;
    int shared_counter = 0;

    SpinLock spinlock;

    // Vector to hold thread objects
    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < increments_per_thread; ++j) {
                spinlock.lock();
                // Critical section
                ++shared_counter;
                spinlock.unlock();
            }
        });
    }

    // Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Final counter value: " << shared_counter << std::endl;
    std::cout << "Expected value: " << num_threads * increments_per_thread << std::endl;

    return 0;
}

