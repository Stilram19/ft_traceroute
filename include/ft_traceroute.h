#ifndef FT_TRACEROUTE_H
#define FT_TRACEROUTE_H

#include "macros.h"
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
    int icmp_sock_fd; // socket used to receive ICMP error messages
    int method_sock_fd; // socket used to send probes and receive responses in methods like TCP method
    char display_address[MAX_IPV4_ADDR_LEN + 1]; // parsed IPv4 address (clean)
 
    // network addressing
    uint16_t sport;
    uint16_t dport;
    char *hostname;                     // hostname/IPv4 address input (destination host/ip)
    char local_address[MAX_IPV4_ADDR_LEN + 1];        // local interface
    struct in_addr daddr;        // destination address
 
    // icmp echo packet structure & pre-allocated and sized
    icmp_echo_t packet;        // contains header + data pointer + data length
    size_t packet_len; 
    char *program_name;

    int dont_fragment; // set to 1 if don't fragment is specified through -F or --dont-fragment

    int method;         // UDP (default), TCP (if specified through -T option)
} traceroute_state_t;

#endif
