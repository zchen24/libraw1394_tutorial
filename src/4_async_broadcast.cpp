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
  * @brief: Tutorial 4: Asynchronous broadcast Write
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

void print_usage()
{
    std::cout << "Usage: 4_async_broadcast [-h] [-n server_nodeid]\n"
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
    rc = raw1394_set_port(handle, 0);
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
     * Asynchronous broadcasting:
     *     Asynchronous broadcast uses asynchronous transfer to broadcast quadlet or block
     *     write data to all other nodes on the bus. It uses a special node id 0xffff. Also,
     *     to avoid bus confliction, no ACK response is expected.
     *
     *     API: raw1394_start_write
     *         - set size to 4 to broadcast quadlet
     *         - set size > 4 to broadcast block data
     *
     *     Caveat:
     *         - firewire driver (not libraw1394) limits register address larger than
     *           CSR_REGISTER_BASE + CSR_CONFIG_END = 0xfffff0000800
     *         - broadcast speed is limited to 100 Mbps (should be able to hack)
     *         - see kernel/drivers/firewire/core-cdev.c
     *               ioctl_send_broadcast_request() for detail
     */

    // setup broadcast nodeid (should be 0xffff, otherwise will NOT work)
    const nodeid_t broadcast_node_id = 0xffff;
    const unsigned long tag = 11;  // tag is random picked, not used

    // hard coded nodeaddr, should be same as the address in 2_arm_server
    const nodeaddr_t arm_start_addr = 0xffffff000000;


    // ----- quadlet broadcast write --------
    quadlet_t data_write = 0x5678;  // data to broadcast to server
    rc = raw1394_start_write(handle,                 // fw handle
                             broadcast_node_id,      // 0xffff
                             arm_start_addr,         // > 0xfffff0000800
                             4,                      // 4 for quadlet broadcast
                             &data_write,            // data to broadcast
                             11);                    // ramdom tag number
    if (rc) {
        std::cerr << "****Error: failed to broadcast quadlet, errno = "
                  << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    } else {
        std::cout << "Broadcast quadlet " << std::hex << data_write
                  << "  addr = " << arm_start_addr << std::endl;
    }


    // ------ block broadcast write -------
    // Exactly same as quadlet broadcast, except size > 4 (8 in this example)
    quadlet_t data_block_write_buffer[2] = {0x11223344, 0x44332211};
    rc = raw1394_start_write(handle,                 // fw handle
                             broadcast_node_id,      // 0xffff
                             arm_start_addr,         // > 0xfffff0000800
                             8,                      // 8 (2 quadlets)
                             data_block_write_buffer,// data to broadcast
                             11);                    // ramdom tag number
    if (rc) {
        std::cerr << "****Error: failed to broadcast block data, errno = "
                  << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    } else {
        std::cout << "Broadcast block data " << std::hex << data_write
                  << "  addr = " << arm_start_addr << std::endl;
    }

    // clean up & exit
    raw1394_destroy_handle(handle);

    return EXIT_SUCCESS;
}







