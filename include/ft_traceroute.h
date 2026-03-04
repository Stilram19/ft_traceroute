#ifndef FT_TRACEROUTE_H
#define FT_TRACEROUTE_H

#include <netinet/in.h>
#include <stdint.h>

// ICMP echo header structure
typedef struct {
    uint8_t  type;           // 8
    uint8_t  code;           // 0
    uint16_t checksum;       // will be calculated
    uint16_t identifier;     // process ID
    uint16_t sequence;       // incrementing sequence number
} icmp_echo_header_t;

// ICMP echo request structure
typedef struct {
    icmp_echo_header_t header;
    uint8_t *data;           // pointer to data
    size_t data_len;         // data length
} icmp_echo_t;

typedef struct traceroute_state {
    int sock_fd;
    char *display_address; // parsed IPv4 address (clean)
 
    // network addressing
    struct sockaddr_in dest_addr;       // destination socket address
    char *hostname;                     // hostname/IPv4 address input
 
    // packet structure & pre-allocated and sized
    icmp_echo_t packet;        // contains header + data pointer + data length
    size_t packet_len; 
    char *program_name;
} traceroute_state_t;

#endif
