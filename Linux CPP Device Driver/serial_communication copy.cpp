// To tun this 
// g++ -o serial_communication serial_communication.cpp -lusb-1.0 && serial_communication

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <libusb-1.0/libusb.h>

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

void open_and_communicate_with_serial_port(const std::string &device_path) {
    int fd = open(device_path.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        std::cerr << "Error opening serial port: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    // Configure the serial port
    configure_serial_port(fd);

    // Write "hello" to the serial port
    const char *msg = "hello";
    if (write(fd, msg, strlen(msg)) != static_cast<ssize_t>(strlen(msg))) {
        std::cerr << "Error writing to serial port: " << std::strerror(errno) << std::endl;
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Read the response
    char buf[256];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n < 0) {
        std::cerr << "Error reading from serial port: " << std::strerror(errno) << std::endl;
        close(fd);
        exit(EXIT_FAILURE);
    }

    buf[n] = '\0'; // Null-terminate the string
    std::cout << "Response: " << buf << std::endl;

    close(fd);
}

int main() {
    // Replace with the actual device path, e.g., /dev/ttyACM0
    std::string device_path = "/dev/ttyACM0";

    open_and_communicate_with_serial_port(device_path);

    return 0;
}
