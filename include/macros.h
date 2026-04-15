#ifndef MACROS_H
#define MACROS_H

#define PARSE_ERROR -1

#define PARSE_OK 0

#define FALSE 0
#define TRUE 1

#define SOCKET_ERROR -1 
#define SOCKET_OK 0

#define MAX_IPV4_ADDR_LEN 15
#define MAX_ERROR_MSG_SIZE 256
#define MAX_SEQUENCE 65535
#define MAX_PACKET_LEN 65535
#define MAX_PORT_NUMBER 65535

#define PROD 1

// defaults
#define DEFAULT_REPLY_TIMEOUT 5 // 5 seconds
#define DEFAULT_PACKET_LEN 60 // 60 bytes by default for ipv4 (we're only supporting ipv4)
#define DEFAULT_NUM_OF_PROBS 3 // 3 probs sent to each TTL setting
#define DEFAULT_MAX_TTL 30
#define DEFAULT_DEST_PORT 33434 // default destination base port for probes
#define DEFAULT_SRC_PORT 33434 // default source ports (using only raw sockets in this implementation we need to choose a default source port)
#define DEFAULT_TCP_PORT 80
#define DEFAULT_FIXED_UDP_PORT 53

// minimum packet length per method
#define MIN_TCP_PACKET_LEN   40   // 20 IP + 20 TCP
#define MIN_DCCP_PACKET_LEN  36   // 20 IP + 16 DCCP request
#define MIN_ICMP_PACKET_LEN  28   // 20 IP + 8 ICMP
#define MIN_UDP_PACKET_LEN   28   // 20 IP + 8 UDP

// exit status codes
#define TRACEROUTE_EX_USAGE 2


// traceroute methods
#define DEFAULT_UDP_TRACEROUTE 0 // UDP method
#define FIXED_UDP_TRACEROUTE 1 // UDP method (with particular ports for tracerouting instead of incrementing in each prob, default port is 53)
#define TCP_TRACEROUTE 2 // TCP method
#define ICMP_TRACEROUTE 3 // ICMP method
#define DCCP_TRACEROUTE 4 // DCCP method

#endif
