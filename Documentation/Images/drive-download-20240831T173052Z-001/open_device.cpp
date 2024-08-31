// To tun this 
// sudo apt-get install libusb-1.0-0-dev
// sudo apt-get install g++ libusb-dev
// g++ -o open_device open_device.cpp -lusb-1.0

#include <iostream>
#include <libusb-1.0/libusb.h>

void open_device(uint16_t vid, uint16_t pid) {
    libusb_context *context = nullptr;
    libusb_device_handle *device_handle = nullptr;
    libusb_device **device_list = nullptr;
    ssize_t device_count;
    int result;

    try {
        // Initialize libusb
        result = libusb_init(&context);
        if (result < 0) {
            throw std::runtime_error("Failed to initialize libusb: " + std::string(libusb_strerror((libusb_error)result)));
        }

        // Get list of USB devices
        device_count = libusb_get_device_list(context, &device_list);
        if (device_count < 0) {
            throw std::runtime_error("Failed to get device list: " + std::string(libusb_strerror((libusb_error)device_count)));
        }

        bool device_found = false;

        // Search for the device with the specified VID and PID
        for (ssize_t i = 0; i < device_count; ++i) {
            libusb_device *device = device_list[i];
            libusb_device_descriptor descriptor;

            result = libusb_get_device_descriptor(device, &descriptor);
            if (result < 0) {
                throw std::runtime_error("Failed to get device descriptor: " + std::string(libusb_strerror((libusb_error)result)));
            }

            if (descriptor.idVendor == vid && descriptor.idProduct == pid) {
                // Open the device
                result = libusb_open(device, &device_handle);
                if (result < 0) {
                    throw std::runtime_error("Failed to open device: " + std::string(libusb_strerror((libusb_error)result)));
                }

                device_found = true;

                std::cout<< "Device details: \n" <<std::endl;
                std::cout<< "\tManufacturer: \t\t"<< descriptor.iManufacturer <<std::endl;
                std::cout<< "\tProduct: \t\t"<< descriptor.iProduct <<std::endl;
                std::cout<< "\tUSB Version: \t\t"<< descriptor.bcdUSB <<std::endl;
                std::cout<< "\tSerial NUm: \t\t"<< descriptor.iSerialNumber <<std::endl;
                break;
            }
        }

        if (!device_found) {
            throw std::runtime_error("Device with VID " + std::to_string(vid) + " and PID " + std::to_string(pid) + " not found.");
        }

        // Do something with the device handle
        std::cout << "Device opened successfully" << std::endl;

        // Close the device
        libusb_close(device_handle);
        libusb_free_device_list(device_list, 1);
        libusb_exit(context);

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        if (device_handle) libusb_close(device_handle);
        if (device_list) libusb_free_device_list(device_list, 1);
        if (context) libusb_exit(context);
    }
}

int main() {
    // Replace with your VID and PID
    uint16_t vid = 0x0483; // Example VID
    uint16_t pid = 0x5740; // Example PID

    open_device(vid, pid);

    return 0;
}

