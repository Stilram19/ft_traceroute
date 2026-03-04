#include "ft_traceroute.h"
#include "macros.h"
#include "parsing.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

traceroute_state_t state;

int main(int argc, char **argv) {
    if (argc == 1) {
        print_help(argv[0]);
        exit(EXIT_SUCCESS);
    }

    state.program_name = argv[0];

    // options parsing 
    int host_index = parse_options(argc, argv);

    if (host_index >= argc) {
        errorLogger("Specify \"host\" missing argument.", TRACEROUTE_EX_USAGE);
    }

    // hostname/ip parsing
    char *input = argv[host_index];
    char display_address[MAX_IPV4_ADDR_LEN + 1] = {};
    struct in_addr addr;

    if (parse_input_address(input, &addr, display_address) == PARSE_ERROR) {
        char msg[MAX_ERROR_MSG_SIZE];
        snprintf(msg, MAX_ERROR_MSG_SIZE, "%s: Name or service not known\nCannot handle \"host\" cmdline arg `%s' on position 1 (argc %d)", argv[1], argv[1], host_index);
        errorLogger(msg, TRACEROUTE_EX_USAGE);
    }

    struct sockaddr_in saddr = {};

    saddr.sin_family = AF_INET;
    saddr.sin_port = 0;
    saddr.sin_addr = addr;
    state.hostname = input;
    state.display_address = display_address;
    state.dest_addr = saddr;

    // packet_len parsing
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
    state.display_address = display_address;
    state.dest_addr = saddr;

    printf("%s\n", display_address);
    return (0);
}
