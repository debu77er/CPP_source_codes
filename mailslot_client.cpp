#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cerrno>

const char* FIFO_PATH = "/tmp/mailslot_fifo";

int main() {
    // Open the FIFO for writing
    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    std::string message;
    std::cout << "Enter messages to send. Type 'exit' to quit." << std::endl;

    while (true) {
        std::cout << "Message: ";
        std::getline(std::cin, message);
        if (message == "exit") {
            break;
        }

        ssize_t bytesWritten = write(fd, message.c_str(), message.size());
        if (bytesWritten == -1) {
            perror("write");
            break;
        }
    }

    close(fd);
    return 0;
}
