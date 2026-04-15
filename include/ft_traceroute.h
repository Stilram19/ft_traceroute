#ifndef FT_TRACEROUTE_H
#define FT_TRACEROUTE_H

#include "macros.h"
#include <netinet/ip.h>
#include <netinet/udp.h>
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

typedef struct probe {
    int ttl;
    int probe_id;

    uint16_t src_port;
    uint16_t dst_port; // for UDP

    uint32_t seq;   // for ICMP/TCP

    struct timeval sent_time;

    int answered;
    char ip_str[INET_ADDRSTRLEN];
} probe_t;

typedef struct traceroute_state {
    int icmp_sock_fd; // socket used to receive ICMP error messages
    int method_sock_fd; // socket used to send probes and receive responses in methods like TCP method
    char display_address[MAX_IPV4_ADDR_LEN + 1]; // parsed IPv4 address (clean)
 
    // network addressing
    uint16_t sport; // source port (in host byte order)
    uint16_t curr_dport; // current destination port for UDP method, and current sequence number for ICMP method
    char *hostname;                     // hostname/IPv4 address input (destination host/ip)
    char local_address[MAX_IPV4_ADDR_LEN + 1];        // local interface
    struct in_addr daddr;        // destination address (in network byte order)
    struct in_addr saddr;        // source address (in network byte order)
 
    uint16_t packet_len;
    char *program_name;

    uint8_t curr_ttl;
    size_t max_ttl;
    size_t probes_per_ttl;

    int method;         // UDP (default), TCP (if specified through -T option)
    struct ip iph; // ip header
    struct udphdr udph;

    int is_iph_constructed; // is ip header already constructed and just needs update for each new probe
    int is_protoh_constructed; // is proto header already constructed and just needs update for each new probe

    void *packet; // points to the next packet's memory (include ipheader + protoheader + payload)
} traceroute_state_t;

// @brief traceroute loop, start sending probes and tracerouting
void start(void);

#endif
