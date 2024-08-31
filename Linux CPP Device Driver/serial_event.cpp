// to run this
// g++ -o serial_event serial_event.cpp -lusb-1.0 -pthread


#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <sys/select.h>
#include <thread>
#include <chrono>

void configure_serial_port(int fd) {
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        std::cerr << "Error getting serial port attributes: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set baud rate
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    // Set 8N1 (8 data bits, no parity, 1 stop bit)
    tty.c_cflag &= ~PARENB; // No parity
    tty.c_cflag &= ~CSTOPB; // 1 stop bit
    tty.c_cflag &= ~CSIZE;  // Mask character size bits
    tty.c_cflag |= CS8;     // 8 data bits

    // Disable hardware flow control
    tty.c_cflag &= ~CRTSCTS;

    // Set local mode and enable receiver
    tty.c_cflag |= CREAD | CLOCAL;

    // Apply the settings
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "Error setting serial port attributes: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
}

void write_hello(int fd) {
    const char *msg = "hello";
    while (true) {
        if (write(fd, msg, strlen(msg)) != static_cast<ssize_t>(strlen(msg))) {
            std::cerr << "Error writing to serial port: " << std::strerror(errno) << std::endl;
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));  
    }
}

void read_from_serial_port(int fd, void (*callback)(const char*)) {
    fd_set read_fds;
    struct timeval timeout;
    char buf[256];
    ssize_t n;

    while (true) {
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        timeout.tv_sec = 1;  // Set timeout to 1 second
        timeout.tv_usec = 0;

        int activity = select(fd + 1, &read_fds, nullptr, nullptr, &timeout);

        if (activity < 0) {
            std::cerr << "Error in select(): " << std::strerror(errno) << std::endl;
            break;
        }

        if (FD_ISSET(fd, &read_fds)) {
            n = read(fd, buf, sizeof(buf) - 1);
            if (n < 0) {
                std::cerr << "Error reading from serial port: " << std::strerror(errno) << std::endl;
                break;
            }

            buf[n] = '\0'; // Null-terminate the string
            callback(buf); // Invoke the callback function
        }
    }
}

void message_callback(const char *message) {
    std::cout << "Received message: " << message << std::endl;
}

void open_and_listen_serial_port(const std::string &device_path) {
    int fd = open(device_path.c_str(), O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);
    if (fd < 0) {
        std::cerr << "Error opening serial port: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    // Configure the serial port
    configure_serial_port(fd);

    // Start writing "hello" every second
    std::thread writer(write_hello, fd);

    // Start reading from the serial port with the callback
    read_from_serial_port(fd, message_callback);

    // Join the writer thread (this will block until the writer thread finishes)
    writer.join();

    close(fd);
}

int main() {
    // Replace with the actual device path, e.g., /dev/ttyACM0
    std::string device_path = "/dev/ttyACM0";

    open_and_listen_serial_port(device_path);

    return 0;
}
