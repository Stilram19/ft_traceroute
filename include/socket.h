#ifndef SOCKET_H
#define SOCKET_H

// @brief creates the socket used to receive ICMP error messages required by traceroute to identify intermediate hops and to know the end 
// it is created for all methods except ICMP traceroute in which the method socket is also used to receive ICMP error message
// @return SOCKET_ERROR to indicate error, SOCKET_OK to indicate success
#include <netinet/in.h>
int createICMPRecvSocket(int *sock_fd);

// @brief creates the socket used by the method (raw TCP socket for TCP method, as an example)
// this socket will be used to send probes and maybe (depending on method) receive packets
// the IP_HDRINCL option is also set
// @return SOCKET_ERROR to indicate error, SOCKET_OK to indicate success
int createMethodSocket(int *sock_fd);

// @brief uses the connect/getsockname trick to get the local interface to use
int addressLookup(char *local_address, struct in_addr *saddr, int *port);

#endif
