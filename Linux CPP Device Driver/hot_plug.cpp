// g++ -o hot hot_plug.cpp -lusb-1.0 && sudo ./hot && rm hot

#include <iostream>
#include <libusb-1.0/libusb.h>

libusb_context *context = nullptr;
libusb_device_handle *device_handle = nullptr;

int hotplug_callback(libusb_context *ctx, libusb_device *device,
                     libusb_hotplug_event event, void *user_data) {
    struct libusb_device_descriptor descriptor;
    libusb_get_device_descriptor(device, &descriptor);

    if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
        std::cout << "Device connected:\n";
        std::cout << "\tVendor ID: " << std::hex << descriptor.idVendor << std::endl;
        std::cout << "\tProduct ID: " << std::hex << descriptor.idProduct << std::endl;

        // Attempt to open the device
        int result = libusb_open(device, &device_handle);
        if (result < 0) {
            std::cerr << "Failed to open device: " << libusb_strerror((libusb_error)result) << std::endl;
        } else {
            std::cout << "Device opened successfully" << std::endl;
        }

    } else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
        std::cout << "Device disconnected\n";

        // Close the device if it was opened
        if (device_handle) {
            libusb_close(device_handle);
            device_handle = nullptr;
            std::cout << "Device handle closed" << std::endl;
        }
    }

    return 0;
}

void open_device(uint16_t vid, uint16_t pid) {
    int result;

    try {
        // Initialize libusb
        result = libusb_init(&context);
        if (result < 0) {
            throw std::runtime_error("Failed to initialize libusb: " + std::string(libusb_strerror((libusb_error)result)));
        }

        // Register hotplug callback
        if (libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
            libusb_hotplug_register_callback(context, 
                                             (libusb_hotplug_event)(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),
                                             LIBUSB_HOTPLUG_ENUMERATE, 
                                             vid, pid, 
                                             LIBUSB_HOTPLUG_MATCH_ANY, 
                                             hotplug_callback, 
                                             nullptr, 
                                             nullptr);
        } else {
            throw std::runtime_error("Hotplug capability is not supported on this system.");
        }

        // Handle events
        while (true) {
            libusb_handle_events(context);
        }

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        if (device_handle) libusb_close(device_handle);
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
