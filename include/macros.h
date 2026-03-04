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
// #define MAX_SEQUENCE 65535
#define MAX_PACKET_LEN 65535

#define PROD 1

// defaults
#define DEFAULT_REPLY_TIMEOUT 5 // 5 seconds
#define DEFAULT_PACKET_LEN 60 // 60 bytes by default for ipv4 (we're only supporting ipv4)
#define DEFAULT_NUM_OF_PROBS 3 // 3 probs sent to each TTL setting
#define DEFAULT_MAX_HOPS 30

#define TRACEROUTE_EX_USAGE 2
#endif
