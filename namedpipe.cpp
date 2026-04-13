#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <atomic>

// g++ -o namedpipe namedpipe.cpp -lpthread

const char* fifo_path = "/tmp/my_named_pipe";

std::atomic<bool> running(true);

void writer() {
    int fd = open(fifo_path, O_WRONLY);
    if (fd == -1) {
        std::cerr << "Failed to open FIFO for writing";
        return;
    }

    std::string input;
    while (running) {
        std::cout << "Enter message to send (or 'exit' to quit): ";
        std::getline(std::cin, input);
        if (input == "exit") {
            running = false;
            break;
        }
        input += "\n"; // add newline for clarity
        ssize_t bytes_written = write(fd, input.c_str(), input.size());
        if (bytes_written == -1) {
            std::cerr << "Write error";
            break;
        }
    }

    close(fd);
}

void reader() {
    int fd = open(fifo_path, O_RDONLY);
    if (fd == -1) {
        std::cerr << "Failed to open FIFO for reading";
        return;
    }

    char buffer[1024];
    while (running) {
        ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            std::cout << "Received: " << buffer;
        } else if (bytes_read == 0) {
            // EOF, wait for writer to reopen
            usleep(100000); // sleep 100 ms
        } else {
            std::cerr << "Read error";
            break;
        }
    }

    close(fd);
}

int main() {
    // Create FIFO if it doesn't exist
    if (access(fifo_path, F_OK) == -1) {
        if (mkfifo(fifo_path, 0666) == -1) {
            std::cerr << "Failed to create FIFO";
            return 1;
        }
        std::cout << "FIFO created at " << fifo_path << "\n";
    } else {
        std::cout << "FIFO already exists at " << fifo_path << "\n";
    }

    // Launch reader and writer threads
    std::thread reader_thread(reader);
    std::thread writer_thread(writer);

    // Wait for writer thread to finish
    writer_thread.join();

    // Once writer is done, set running to false and close reader
    running = false;
    // Since reader might be blocked on read, we can open FIFO write end to unblock
    int dummy_fd = open(fifo_path, O_WRONLY);
    if (dummy_fd != -1) {
        close(dummy_fd);
    }
    reader_thread.join();

    // Cleanup: remove FIFO
    unlink(fifo_path);

    std::cout << "Exiting.\n";
    return 0;
}
