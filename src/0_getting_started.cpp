
// Author: Zihan Chen
// Date: 2013-08-24


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
  * Tutorial 0: Getting started
  *
  *     This tutorial covers 3 topics:
  *         - check libraw1394 version
  *         - create handle
  *         - get list of nodes
  *
  * Reference:
  *   - http://www.dennedy.org/libraw1394/id2640614.html
  */


int main(int argc, char** argv)
{
    int rc; /**< return code */

    // ----- check libraw1394 verison ----
    const char* libraw1394_version = raw1394_get_libversion();
    std::cout << "lib version = " << libraw1394_version << std::endl;

    // ----- Get handle and set port for the handle -------

    // create handle
    raw1394handle_t handle = raw1394_new_handle();
    if (handle == NULL) {
        std::cerr << "**** Error: could not create 1394 handle " << strerror(errno) << std::endl;
        return -1;
    }

    // get port information
    int numPorts;
    const int maxport = 4;  // max 4 ports
    raw1394_portinfo portInfo[maxport];  // place to save port info
    numPorts = raw1394_get_port_info(handle, portInfo, maxport);

    // display port info


    // let user to choose which port to use
    rc = raw1394_set_port(handle, 0);
    if (rc) {
        std::cerr << "**** Error: failed to set port " << strerror(errno) << std::endl;
    }

    // WARNING: from now on, get/set port should NOT be used on this handle

    // sample use of the handle
    quadlet_t data;
    nodeid_t localId = raw1394_get_local_id(handle);
    rc = raw1394_read(handle, localId,
                      CSR_REGISTER_BASE + CSR_CONFIG_ROM, 4, &data);
    if (rc) {
        std::cerr << "**** Error: failed to read quadlet " << strerror(errno) << std::endl;
    } else {
        std::cout << "Config rom 0 = " << std::hex << data << std::endl;
    }


    // broadcast
    nodeid_t broadcastId = 0xffc2;
    rc = raw1394_write(handle, broadcastId, 0, 4, &data);
    if (rc) {
        std::cerr << "**** Error: failed to write broadcast " << strerror(errno) << std::endl;
    }

    // clean up & exit
    raw1394_destroy_handle(handle);

    return EXIT_SUCCESS;
}












