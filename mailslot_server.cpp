#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>

const char* FIFO_PATH = "/tmp/mailslot_fifo";

int main() {
    // Create the FIFO (named pipe) if it doesn't exist
    if (mkfifo(FIFO_PATH, 0666) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo");
            return 1;
        }
    }

    std::cout << "Mailslot server started. Waiting for messages..." << std::endl;

    while (true) {
        // Open the FIFO for reading
        int fd = open(FIFO_PATH, O_RDONLY);
        if (fd == -1) {
            perror("open");
            break;
        }

        // Read messages from the FIFO
        char buffer[1024];
        ssize_t bytesRead;
        while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';  // Null-terminate the string
            std::cout << "Received message: " << buffer << std::endl;
        }

        if (bytesRead == -1) {
            perror("read");
        }

        close(fd);
        // Loop back to wait for next message
    }

    // Cleanup
    unlink(FIFO_PATH);
    return 0;
}
