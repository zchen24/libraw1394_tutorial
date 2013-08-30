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
  * @brief: Tutorial 3: Asynchronous Cient (Read/Write)
  *
  *     This tutorial shows how to use asynchronous quedlet/block read/write.
  *         - to test
  *             - use a firewire cable to connect to another computer (pc_server)
  *             - run 2_arm_server on (pc_server)
  *             - run 3_async_client
  *         - this example includes
  *             - quadlet read
  *             - quadlet write
  *             - block read
  *             - block write
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


int main(int argc, char** argv)
{
    int rc; /**< return code */
    int port = 0;  /*!< fw handle port number */
    int nodeid = 0;   /*!< arm server node id */

    // parse command line (port number)
    opterr = 0;  // getopt no err output
    const char short_options[] = "p:n:";
    int next_opt;
    do {
        next_opt = getopt(argc, argv, short_options);
        switch(next_opt)
        {
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
    // Start tutorial 3 async client
    // ----------------------------------------------------------------------------

    // setup server nodeid
    nodeid_t server_nodeid;
    // get bus id and set server nodeid
    server_nodeid = (raw1394_get_local_id(handle) & 0xffc0) + nodeid;

    // hard coded nodeaddr, should be same as the address in 2_arm_server
    const nodeaddr_t arm_start_addr = 0xffffff000000;

    // quadlet write
    quadlet_t data_write = 0x5678;  // data to write to server
    rc = raw1394_write(handle, server_nodeid, arm_start_addr, 4, &data_write);
    if (rc) {
        std::cerr << "****Error: failed to write quadlet, errno = "
                  << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    } else {
        std::cout << "Write quadlet " << std::hex << data_write
                  << " to " << arm_start_addr << std::endl;
    }

    // quadlet read, now read the value back
    quadlet_t data_read = 0x0000;  // init to 0x0000
    rc = raw1394_write(handle, server_nodeid, arm_start_addr, 4, &data_read);
    if (rc) {
        std::cerr << "****Error: failed to read quadlet, errno = "
                  << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    } else {
        std::cout << "Read quadlet " << std::hex << data_read
                  << " from " << arm_start_addr << std::endl;
    }

    // block write
    const size_t data_block_write_size = 2;
    quadlet_t data_block_write_buffer[2] = {0x11223344, 0x44332211};
    rc = raw1394_write(handle, server_nodeid, arm_start_addr,
                       data_block_write_size * 4,
                       data_block_write_buffer);
    if (rc) {
        std::cerr << "****Error: failed to write block, errno = "
                  << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    } else {
        std::cout << "Write block: " << std::hex;
        for (size_t i = 0; i < data_block_write_size; i++) {
            std::cout << " " << data_block_write_buffer[i] << std::endl;
        }
        std::cout << std::endl;
    }

    // block read
    quadlet_t data_block_read_buffer[2] = {0x11223344, 0x44332211};
    rc = raw1394_read(handle, server_nodeid, arm_start_addr,
                      data_block_write_size * 4, data_block_read_buffer);
    if (rc) {
        std::cerr << "****Error: failed to read block, errno = "
                  << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    } else {
        std::cout << "Read block: " << std::hex;
        for (size_t i = 0; i < data_block_write_size; i++) {
            std::cout << " " << data_block_read_buffer[i] << std::endl;
        }
        std::cout << std::endl;
    }


    // FIX ME:
    //   add lock tranaction in the future

    // clean up & exit
    raw1394_destroy_handle(handle);

    return EXIT_SUCCESS;
}







