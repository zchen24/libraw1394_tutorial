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
  * @brief: Tutorial 6: Isochronous Transmit
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


#define BUFFER 1000
#define MAX_PACKET 4096
#define BUF_SIZE 4096
#define BUF_HEAD 8


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


//raw1394_iso_disposition
//my_iso_recv_handler(raw1394handle_t handle,
//                    unsigned char *data,
//                    unsigned int len,
//                    unsigned char channel,
//                    unsigned char tag,
//                    unsigned char sy,
//                    unsigned int cycle,
//                    unsigned int dropped)
//{
//    std::cout << "channel = " << channel << std::endl;

//    // see raw1394_iso_disposition
//    return RAW1394_ISO_OK;
//}

//raw1394_iso_xmit_handler_t
raw1394_iso_disposition
my_iso_xmit_handler(raw1394handle_t handle,
                    unsigned char *data,
                    unsigned int *len,
                    unsigned char *tag,
                    unsigned char *sy,
                    int cycle, /* -1 if unknown */
                    unsigned int dropped)
{
    std::cout << "xmit tag = " << &tag << std::endl;
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
    // Start tutorial 6 isochronous send
    // ----------------------------------------------------------------------------

    /**
     * raw1394_iso_xmit_init - initialize isochronous transmission
     * @handle: libraw1394 handle
     * @handler: handler function for queueing packets
     * @buf_packets: number of isochronous packets to buffer
     * @max_packet_size: largest packet you need to handle, in bytes
     * (not including the isochronous header)
     * @channel: isochronous channel on which to transmit
     * @speed: speed at which to transmit
     * @irq_interval: maximum latency of wake-ups, in packets (-1 if you don't care)
     *
     * Allocates all user and kernel resources necessary for isochronous transmission.
     * Channel and bandwidth allocation at the IRM is not performed.
     *
     * Returns: 0 on success or -1 on failure (sets errno)
     **/
    size_t length;
    unsigned char channel, tag, sy;
    unsigned char buffer[BUF_SIZE + BUF_HEAD];
    channel = 5;
    tag = 6;
    sy = 7;

    // ----- transmitting end -------------
    rc = raw1394_iso_xmit_init(handle,      // 1394 handle
                               NULL,        // xmit handler
                               BUFFER,      // iso packets to buffer
                               MAX_PACKET,  // max packet size
                               channel,           // just pick 5 for fun
                               RAW1394_ISO_SPEED_400,
                               -1);         // irq_interval
    if (rc) {
        perror("raw1394_iso_xmit_init");
        exit(1);
    }
    rc = raw1394_iso_xmit_start(handle, -1, -1);
    if (rc) {
        perror("raw1394_iso_xmit_start");
        exit(1);
    }

    quadlet_t data = 0x0;
    while (true) {
        rc = raw1394_iso_xmit_write(handle,
                                    (unsigned char *)&data,
                                    4,
                                    tag,
                                    sy);
        if (rc) {
            perror("\nraw1394_iso_xmit_write");
            break;
        }
//        data++;
//        std::cout << "data = " << data << std::endl;
    }

    // stop, clean up & exit
    raw1394_iso_stop(handle);
    raw1394_iso_shutdown(handle);
    raw1394_destroy_handle(handle);

    return EXIT_SUCCESS;
}







