
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <byteswap.h>
#include <bitset>
#include <stdint.h>

// libraw1394
#include <libraw1394/raw1394.h>
#include <libraw1394/csr.h>


// Declare handle here
raw1394handle_t handle;
const int maxBufferSize = 100;  // max 100 quadlets
quadlet_t readBuffer[maxBufferSize];


/* signal handler cleans up and exits the program */
void signal_handler(int sig) {
    signal(SIGINT, SIG_DFL);
    raw1394_destroy_handle(handle);
    exit(0);
}


/* bus reset handler updates the bus generation */
int reset_handler(raw1394handle_t hdl, unsigned int gen) {
    int id = raw1394_get_local_id(hdl);
    printf("Bus reset to gen %d, local id %x\n", gen, id);

    std::cout << "hello in reset handler" << std::endl;
    raw1394_update_generation(hdl, gen);
    return 0;
}


// tag handler
int my_tag_handler(raw1394handle_t handle, unsigned long tag,
                raw1394_errcode_t errcode)
{
    int err = raw1394_errcode_to_errno(errcode);
    if (err) {
        std::cerr << "failed with error: " << strerror(err) << std::endl;
    } else {
        std::cout << "completed customized tag handler = " << bswap_32(readBuffer[0]) << std::endl;
    }
}


// asynchronous read/write handler
int arm_handler(raw1394handle_t handle, unsigned long arm_tag,
                byte_t request_type, unsigned int requested_length,
                void *data)
{

    std::cout << "arm_handler called " << std::endl
              << "req len = " << requested_length << " type = " << request_type << std::endl;
}




// arm tag callback
int my_arm_req_callback(raw1394handle_t handle,
                        struct raw1394_arm_request_response *arm_req_resp,
                        unsigned int requested_length,
                        void *pcontext, byte_t request_type)
{
    std::cout << "arm_req_callback, type = " << std::dec << (int)request_type << std::endl;
//    std::cout << ""
}





int main(int argc, char** argv)
{
    // return code
    int rc;

    // ----- Set linux signal handler -----
    signal(SIGINT, signal_handler);


    // ----- Display lib version ----
    std::cout << "lib version = " << raw1394_get_libversion() << std::endl;

    // ----- Get handle and set port for the handle -------

    // create handle
    handle = raw1394_new_handle();
    if (handle == NULL) {
        std::cerr << "**** Error: could not open 1394 handle" << std::endl;
        return -1;
    }

    // set the bus reset handler
    raw1394_set_bus_reset_handler(handle, reset_handler);
    rc = raw1394_busreset_notify(handle, RAW1394_NOTIFY_OFF);

    // set the bus arm_tag handler
//    raw1394_set_arm_tag_handler(handle, arm_handler);


    // get port info & save to portinfo
    const int maxports = 4;
    raw1394_portinfo portinfo[maxports];
    int numPorts = raw1394_get_port_info(handle, portinfo, maxports);

    // display port info
    for (size_t i = 0; i < numPorts; i++) {
        std::cout << "port " << portinfo[i].name
                  << "  has " << portinfo[i].nodes << " nodes" << std::endl;
    }

    // set port to handle, port should be 0 to numPorts-1
    int port = 0;
    rc = raw1394_set_port(handle, port);
    if (rc) {
        std::cerr << "set port error: " <<  strerror(errno) << std::endl;
    }


    // ------ Get number of node --------
    int numNodes = raw1394_get_nodecount(handle);
    std::cout << numNodes << " nodes" << std::endl;

    /**
      * nodeid_t is a 16 bits value,
      *   16 = 10 + 6
      *   higher 10 bits are bus ID
      *   lower 6 bits are local node number (physical ID)
      *   physical ID are cynamic and determined during bus reset
      */
    nodeid_t localID = raw1394_get_local_id(handle);
    std::cout << "Local ID = " << std::hex << localID << std::endl;

    // get nodeid_t for the node (here node physical id = 0)
    int node = 0;
    nodeid_t targetNodeID = (localID & 0xFFC0) + node;

    // -------- get config rom info ---------
    size_t romBufferSize = 100;
    quadlet_t romBuffer[romBufferSize];
    size_t rom_size;
    unsigned char rom_version[100];
    rc = raw1394_get_config_rom(handle,
                                romBuffer,
                                romBufferSize,
                                &rom_size,
                                rom_version);

    std::cout << "rom_size = " << rom_size
              << "  rom_version = " << rom_version << std::endl;
    for (size_t i = 0; i < rom_size; i++) {
//        std::cout << std::hex << bswap_32(romBuffer[i]) << std::endl;
    }
    std::cout << std::hex << bswap_32(romBuffer[0]) << std::endl;

    // ------- Isochronous test ---------------
    nodeid_t irmNodeID = raw1394_get_irm_id(handle);
    std::cout << "irm node id = " << std::hex << irmNodeID << std::endl;


    // -------  Asynchronous read -----------

    std::cout << "----------- Asynchronous read -----------" << std::endl
              << "nodeid = " << std::hex << targetNodeID << std::endl;

    // we are ready for read
    int size = 0;  // read/write size
    for (int i = 0; i < maxBufferSize; i++) {
        readBuffer[i] = 0;
    }

    // using custom tag handler
    tag_handler_t std_tag_handler;
    std_tag_handler = raw1394_set_tag_handler(handle, my_tag_handler);

    if (raw1394_start_read(handle, targetNodeID, 0, 4, readBuffer, 0))
    {
        perror("failed");
    }
    if (raw1394_loop_iterate(handle)) {
        perror("failed");
    }

    // quadlet read using default tag_handler
    raw1394_set_tag_handler(handle, std_tag_handler);

    size = 1;
    rc = raw1394_read(handle, targetNodeID, 0, size*4, readBuffer);
    if (rc) {
        std::cerr << "quadlet read error: " <<  strerror(errno) << std::endl;
    } else {
        for (size_t i = 0; i < size; i++) {
            std::cout << "quadlet read = " << std::hex << bswap_32(readBuffer[i]) << std::endl;
        }
    }

    // block read
    size = 20;
    rc = raw1394_read(handle, targetNodeID, 0, 20, readBuffer);
    if (rc) {
        std::cerr << "block read error: " << strerror(errno) << std::endl;
    } else {
        for (size_t i = 0; i < size; i++) {
            std::cout << "block read " << std::dec << i << " = "
                      << std::hex << bswap_32(readBuffer[i]) << std::endl;
        }
    }


    // --------------------------------------------
    // ------------- ARM Handling -----------------
    const int maxArmBufferSize = 300;
    uint8_t armBuffer[maxArmBufferSize];
    size_t armSize = 10;
    int mode = RAW1394_ARM_WRITE | RAW1394_ARM_READ;
    byte_t configROM[4] = {0x01, 0x04, 0x02, 0x09};
    raw1394_arm_reqhandle reqHandle;

    char my_arm_callback_context[] = "my_arm_callback_context";


    reqHandle.arm_callback = my_arm_req_callback;
    reqHandle.pcontext = my_arm_callback_context;

    // regitster arm to handle request for CSR_CONFIG_ROM
    rc = raw1394_arm_register(handle, 0, 4, configROM,
                              (unsigned long) &reqHandle, mode, mode, mode);

    // get memory
    rc = raw1394_arm_get_buf(handle, 0, 4, armBuffer);
    if (rc) {
        std::cerr << "arm get buf: " << strerror(errno) << std::endl;
    } else {
        for (size_t i = 0; i < 4; i++) {
            std::cout << "arm buf " << std::dec << i << " = "
                      << std::hex << bswap_32(armBuffer[i]) << std::endl;
        }
    }

    while (1) {
        raw1394_loop_iterate(handle);
    }

    // clean up & exit
    raw1394_destroy_handle(handle);

    return EXIT_SUCCESS;
}












