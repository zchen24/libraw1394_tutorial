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
  * @brief: Tutorial 1: Bus reset
  *
  *     This tutorial shows how to deal with bus reset:
  *         - to test plug and unplug a firewire device
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
    }

    // let user to choose which port to use
    rc = raw1394_set_port(handle, 0);
    if (rc) {
        std::cerr << "**** Error: failed to set port " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }


    // -------- Set FireWire bus reset handler --------

    // set bus reset handler
    raw1394_set_bus_reset_handler(handle, my_bus_reset_handler);


    // --------- Infinite raw1394 event loop ----------
    while (true)
    {
        raw1394_loop_iterate(handle);
    }

    // clean up & exit
    raw1394_destroy_handle(handle);

    return EXIT_SUCCESS;
}







