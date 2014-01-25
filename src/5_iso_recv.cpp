#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <byteswap.h>
#include <stdio.h>

// libraw1394
#include <libraw1394/raw1394.h>
#include <libraw1394/csr.h>


/**
  * @brief: Tutorial 5: Isochronous broadcast Write
  *
  *     This tutorial shows how to use asynchronous quedlet/block broadcast write.
  *         - to run this example
  *             - use a firewire cable to connect to another computer (pc_server_1)
  *             - run 2_arm_server on (pc_server_1)
  *             - (optional)
  *                - if you have one more computer (pc_server_2)
  *                - daisy-chain it into the same firewire buse
  *                - run 2_arm_server on (pc_server_2)
  *             - run 4_async_broadcast
  *         - this example includes
  *             - quadlet broadcast write
  *             - block broadcast write
  *
  *
  * @date 2013-08-30
  * @author Zihan Chen
  *
  * @ref http://www.dennedy.org/libraw1394/id2640614.html
  *
  */


// Global variable fw handle
raw1394handle_t handle;

// bus reset handler, update bus generation
int my_bus_reset_handler(raw1394handle_t h, unsigned int gen)
{
    nodeid_t id = raw1394_get_local_id(h);
    std::cout << "Reset bus to gen " << gen << " local id " << id << std::endl;

    // update handle gen value
    raw1394_update_generation(h, gen);
}


raw1394_iso_disposition
my_iso_recv_handler(raw1394handle_t handle,
                    unsigned char *data,
                    unsigned int len,
                    unsigned char channel,
                    unsigned char tag,
                    unsigned char sy,
                    unsigned int cycle,
                    unsigned int dropped)
{
    std::cout << "channel = " << channel << std::endl;

    // see raw1394_iso_disposition
    return RAW1394_ISO_OK;
}


void print_usage()
{
    std::cout << "Usage: 5_iso_server [-h] [-n server_nodeid]\n"
              << "    -h  show usage\n"
              << "    -n  specify server nodeid\n";
}


int main(int argc, char** argv)
{
    int rc; /**< return code */
    int port = 0;  /*!< fw handle port number */
    int nodeid = 0;   /*!< arm server node id */

    // parse command line (port number)
    opterr = 0;  // getopt no err output
    const char short_options[] = "hp:n:";
    int next_opt;
    do {
        next_opt = getopt(argc, argv, short_options);
        switch(next_opt)
        {
        case 'h':
            print_usage();
            return EXIT_SUCCESS;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 'n':
            nodeid = atoi(optarg);
            break;
        case '?':
            std::cerr << "Invalid argument" << std::endl;
            break;
        default:
            break;
        }
    }
    while(next_opt != -1);


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
    rc = raw1394_set_port(handle, port);
    if (rc) {
        std::cerr << "**** Error: failed to set port " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }


    // -------- Set FireWire bus reset handler --------

    // set bus reset handler
    raw1394_set_bus_reset_handler(handle, my_bus_reset_handler);



    // ----------------------------------------------------------------------------
    // Start tutorial 4 async broadcast
    // ----------------------------------------------------------------------------

    /**
     *
     */
    // ----- receving end -------------
    raw1394_iso_recv_init(handle,
                          my_iso_recv_handler,
                          100,     // buf_packets
                          10,      // max_packet_size
                          0x5,     // channel
                          RAW1394_DMA_DEFAULT,
                          -1);     // irq_interval default

    // start receiving
    raw1394_iso_recv_start(handle, -1, -1, 0);
    while (true)
    {
        raw1394_loop_iterate(handle);
    }

    // stop, clean up & exit
    raw1394_iso_stop(handle);
    raw1394_iso_shutdown(handle);
    raw1394_destroy_handle(handle);

    return EXIT_SUCCESS;
}







