#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <byteswap.h>

// libraw1394
#include <libraw1394/raw1394.h>
#include <libraw1394/csr.h>


/**
  * @brief: Tutorial 2: arm server
  *
  *     This tutorial shows how to register arm to handle incoming firewire request:
  *         - arm: Address Range Mapping
  *         - 1 register arm buffer for read/write access
  *         - 2 read first 4 quadlet (should be initial value)
  *         - 3 write value to frist 4 quadlets
  *         - 4 read back for verification
  *     NOTE: this program will be used in the next tutorial as server program.
  *
  * @date 2013-08-30
  * @author Zihan Chen
  *
  * @ref http://www.dennedy.org/libraw1394/id2640614.html
  *
  */


// Global variable fw handle
raw1394handle_t handle;


/* signal handler cleans up and exits the program */
void signal_handler(int sig) {
    signal(SIGINT, SIG_DFL);
    raw1394_destroy_handle(handle);
    exit(0);
}


// bus reset handler, update bus generation
int my_bus_reset_handler(raw1394handle_t h, unsigned int gen)
{
    nodeid_t id = raw1394_get_local_id(h);
    std::cout << "Reset bus to gen " << gen << " local id " << id << std::endl;

    // update handle gen value
    raw1394_update_generation(h, gen);
}


// arm tag callback
int my_arm_req_callback(raw1394handle_t handle,
                        struct raw1394_arm_request_response *arm_req_resp,
                        unsigned int requested_length,
                        void *pcontext, byte_t request_type)
{
    std::cout << "arm_req_callback, type = " << std::dec << (int)request_type << std::endl;
}


int main(int argc, char** argv)
{
    int rc; /**< return code */
    int port = 0;  /*!< fw handle port number */

    // parse command line (port number)
    opterr = 0;  // getopt no err output
    const char short_options[] = "p:";
    int next_opt;
    do {
        next_opt = getopt(argc, argv, short_options);
        switch(next_opt)
        {
        case 'p':
            port = atoi(optarg);
            break;
        case '?':
            std::cerr << "Invalid argument" << std::endl;
            break;
        default:
            break;
        }
    }
    while(next_opt != -1);


    // Setup signal handler to exit on Ctrl-C
    signal(SIGINT, signal_handler);


    // ----- Get handle and set port for the handle -------
    // create handle
    handle = raw1394_new_handle();
    if (handle == NULL) {
        std::cerr << "**** Error: could not create 1394 handle " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }

    // get port number & sanity check
    int numPorts = raw1394_get_port_info(handle, NULL, 0);
    if (port < 0 || port >= numPorts) {
        std::cerr << "Invalid port number" << std::endl;
        return EXIT_FAILURE;
    }

    // let user to choose which port to use
    rc = raw1394_set_port(handle, 0);
    if (rc) {
        std::cerr << "**** Error: failed to set port " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }


    // -------- Set FireWire bus reset handler --------
    // set bus reset handler
    bus_reset_handler_t old_bus_reset_handler;
    old_bus_reset_handler = raw1394_set_bus_reset_handler(handle, my_bus_reset_handler);



    // ----------------------------------------------------------------------------
    // Start tutorial 2 arm server
    // ----------------------------------------------------------------------------


    // -------- Register arm register to handle arm request --------
    const nodeaddr_t arm_start_addr = 0xffffff000000;  // arm start address
    const size_t arm_length = 4;  // arm length to register

    // arm initial buffer
    byte_t arm_init_buffer[arm_length];
    memset(arm_init_buffer, 0x02, arm_length);  // set inital value to all 0x02

    // setup arm request handle
    raw1394_arm_reqhandle arm_reqhandle;
    char my_arm_callback_context[] = "my_arm_callback_context";
    arm_reqhandle.pcontext = my_arm_callback_context;
    arm_reqhandle.arm_callback = my_arm_req_callback;

    int access_mode = RAW1394_ARM_WRITE|RAW1394_ARM_READ;   // allow read and write transaction

    rc = raw1394_arm_register(handle,  // fw handle
                              arm_start_addr, // arm start address
			      arm_length * 4, // arm_length quadlet * 4 to bytes
			      arm_init_buffer,  // arm init buffer value
                              (octlet_t) &arm_reqhandle,  // arm request handler
                              access_mode,   // access permission
                              access_mode,   // client handler will be notified
                              0);            // client handler will need to handle these transactions

    if (rc) {
        std::cerr << "**** Error: failed to setup arm register, error " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }

    // -------- Read from this arm buffer ---------
    // NOTE: this is a simple way to test if
    const size_t arm_read_size = 4;
    byte_t arm_read_buffer[arm_read_size];

    rc = raw1394_arm_get_buf(handle, arm_start_addr, arm_read_size, arm_read_buffer);
    if (rc) {
        std::cerr << "**** Error: failed to read arm register, error " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    } else {
        // if success then print value
        std::cout << "ARM buffer read value: \n";
        for (size_t i = 0; i < arm_read_size; i++) {
            std::cout << std::hex << " " << (int)arm_read_buffer[i];
        }
        std::cout << std::endl;
    }

    // -------- Set value to this arm buffer ---------
    const size_t arm_write_size = 4;
    byte_t arm_write_buffer[arm_write_size] = {0x11, 0x22, 0x33, 0x44};
    rc = raw1394_arm_set_buf(handle, arm_start_addr, arm_write_size, arm_write_buffer);
    if (rc) {
        std::cerr << "**** Error: failed to set arm register, error " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }


    // --------- Now read the value back from buffer ------
    rc = raw1394_arm_get_buf(handle, arm_start_addr, arm_read_size, arm_read_buffer);
    if (rc) {
        std::cerr << "**** Error: failed to read arm register, error " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    } else {
        // if success then print value
        std::cout << "ARM buffer read value: \n";
        for (size_t i = 0; i < arm_read_size; i++) {
            std::cout << std::hex << " " << (int)arm_read_buffer[i];
        }
        std::cout << std::endl;
    }


    // --------- Infinite raw1394 event loop ----------

    std::cout << "--------- Now start arm server -----------" << std::endl;
    std::cout << " node id = " << raw1394_get_local_id(handle) << std::endl;

    while (true)
    {
        raw1394_loop_iterate(handle);
    }

    // clean up & exit
    raw1394_destroy_handle(handle);

    return EXIT_SUCCESS;
}




