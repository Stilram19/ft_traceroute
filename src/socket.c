#include "socket.h"
#include "ft_traceroute.h"
#include "macros.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

extern traceroute_state_t state;

int createMethodSocket(int *sock_fd) {
    if (!sock_fd) {
        return (SOCKET_ERROR);
    }

    switch (state.method) {
        case TCP_TRACEROUTE:
            *sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
            break;
        case ICMP_TRACEROUTE:
            *sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
            break;
        case DCCP_TRACEROUTE:
            *sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_DCCP);
            break;
        default:
            *sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    }

    if (*sock_fd < 0) {
        return (SOCKET_ERROR);
    }

    // setting the IP_HDRINCL option to construct the IP header manually
    int one = 1;
    if (setsockopt(*sock_fd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        return (SOCKET_ERROR);
    }

    return (SOCKET_OK);
}

int createICMPRecvSocket(int *sock_fd) {
    if (!sock_fd) {
        return (SOCKET_ERROR);
    }

    *sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    if (*sock_fd < 0) {
        return (SOCKET_ERROR);
    }

    return (SOCKET_OK);
}

int addressLookup(char *local_address, struct in_addr *saddr, int *port) {
    if (!local_address || !port || !saddr) {
        return (SOCKET_ERROR);
    }
    struct sockaddr_in dest_addr = {};

    dest_addr.sin_addr = state.daddr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(state.dport);

    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (fd < 0) {
        return (SOCKET_ERROR);
    }

    // the kernel does routing table lookup (depending on the destination) and binds the socket to the chosen local interface...
    if (connect(fd, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        close(fd);
        return (SOCKET_ERROR);
    }

    struct sockaddr_in local_addr;

    // getsockname will read the socket struct (already filled by the kernel in connect call) and fills 'local_address'
    socklen_t len = sizeof(local_addr);

    if (getsockname(fd, (struct sockaddr *)&local_addr, &len) < 0) {
        close(fd);
        return (SOCKET_ERROR);
    }

    *saddr = local_addr.sin_addr;

    char *address_str = inet_ntoa(local_addr.sin_addr);
    strncpy(local_address, address_str, MAX_IPV4_ADDR_LEN);
    *port = local_addr.sin_port;
    close(fd);

    return (SOCKET_OK);
}
