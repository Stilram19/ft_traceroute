#include "ft_traceroute.h"
#include "macros.h"
#include "parsing.h"
#include "socket.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

traceroute_state_t state;

int main(int argc, char **argv) {
    if (argc == 1) {
        print_help(argv[0]);
        exit(EXIT_SUCCESS);
    }

    state.program_name = argv[0];
    state.method = DEFAULT_UDP_TRACEROUTE; // setting method to UDP (default), if -T option is specified method will be set to TCP_TRACEROUTE
    state.dont_fragment = 0;
    state.sport = 0; // to be initialized to a valid value by -sp option or addressLookup
    state.dport = DEFAULT_DEST_PORT;

    // (*) options parsing

    int host_index = parse_options(argc, argv);

    if (host_index >= argc) {
        errorLogger("Specify \"host\" missing argument.", TRACEROUTE_EX_USAGE);
    }

    // (*) hostname/ip parsing

    char *input = argv[host_index];
    char display_address[MAX_IPV4_ADDR_LEN + 1] = {};
    struct in_addr addr;

    if (parse_input_address(input, &addr, display_address) == PARSE_ERROR) {
        char msg[MAX_ERROR_MSG_SIZE];
        snprintf(msg, MAX_ERROR_MSG_SIZE, "%s: Name or service not known\nCannot handle \"host\" cmdline arg `%s' on position 1 (argc %d)", argv[1], argv[1], host_index);
        errorLogger(msg, TRACEROUTE_EX_USAGE);
    }

    state.hostname = input;
    strncpy(state.display_address, display_address, MAX_IPV4_ADDR_LEN);
    state.daddr = addr;

    // (*) packet_len parsing

    int packet_len_index = host_index + 1;
    int packet_len = DEFAULT_PACKET_LEN;

    if (packet_len_index < argc) {
        // if there are more args after packet_len error
        if (packet_len_index < argc - 1) {
            char msg[MAX_ERROR_MSG_SIZE];
            snprintf(msg, MAX_ERROR_MSG_SIZE, "Extra arg `%s' (position 3, argc %d)", argv[packet_len_index + 1], packet_len_index + 1);
            errorLogger(msg, TRACEROUTE_EX_USAGE);
        }

        if (parse_packet_len(argv[packet_len_index], &packet_len) == PARSE_ERROR) {
            char msg[MAX_ERROR_MSG_SIZE];
            snprintf(msg, MAX_ERROR_MSG_SIZE, "Cannot handle \"packetlen\" cmdline arg `%s' on position 2 (argc %d)", argv[packet_len_index], packet_len_index);
            errorLogger(msg, TRACEROUTE_EX_USAGE);
        }
    }

    state.packet_len = packet_len;

    printf("%s\n", display_address);

    // (*) sockets creation

    int method_sock_fd = 0;
    int icmp_sock_fd = 0;

    if (createMethodSocket(&method_sock_fd) == SOCKET_ERROR) {
        errorLogger("socket creation failed", EXIT_FAILURE);
    }

    if (state.method != ICMP_TRACEROUTE) {
        if (createICMPRecvSocket(&icmp_sock_fd) == SOCKET_ERROR) {
            errorLogger("socket creation failed", EXIT_FAILURE);
        }
    } else {
        icmp_sock_fd = method_sock_fd; // same socket used in case of ICMP traceroute
    }

    state.method_sock_fd = method_sock_fd;
    state.icmp_sock_fd = icmp_sock_fd;

    // (*) set local address

    int sport;
    char local_address[MAX_IPV4_ADDR_LEN + 1] = {};

    if (addressLookup(local_address, &sport) == SOCKET_ERROR) {
        errorLogger("address lookup failed", EXIT_FAILURE);
    }

    strncpy(state.local_address, local_address, MAX_IPV4_ADDR_LEN);

    if (state.sport == 0) {
        state.sport = sport;
    }

    return (0);
}
