#define _DEFAULT_SOURCE
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <sysexits.h>
#include <stdlib.h>
#include "ft_traceroute.h"
#include "utils.h"
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "macros.h"
#include "parsing.h"

extern traceroute_state_t state;

static int is_all_digits(const char *str) {
    if (!str || *str == '\0')
        return (0);
    while (*str) {
        if (!isdigit((unsigned char)*str))
            return (0);
        str++;
    }
    return (1);
}

void print_help(char *program_name) {
    printf("Usage: %s [options] <hostname/IP>\n\n", program_name);
    printf("Options:\n");
    printf("  --help                    Read this help and exit\n");
    printf("  -F  --dont-fragment       Do not fragment packets\n");
    printf("  -T  --tcp                 Use TCP SYN for tracerouting (default port is 80)\n");
    printf("  -I  --icmp                Use ICMP ECHO for tracerouting\n");
    printf("  -U  --udp                 Use UDP to particular port for tracerouting\n"
           "                            (instead of increasing the port per each probe),\n"
           "                            default port is 53\n");
    printf("  -D  --dccp                Use DCCP Request for tracerouting\n"
           "                            (default port is 33434)\n");
    printf("  -p port                   Set the destination port to use. It is either\n"
           "                            initial udp port value for \"default\" method\n"
           "                            (incremented by each probe, default is 33434), or\n"
           "                            initial seq for \"icmp\" (incremented as well,\n"
           "                            default from 1), or some constant destination\n"
           "                            port for other methods (with default of 80 for\n"
           "                            \"tcp\", 53 for \"udp\", etc.)\n");
    printf("  -sp num                   Use source port num for outgoing packets.\n");
}

int parse_packet_len(char *arg, int *packet_len) {
    if (!arg || !packet_len || !is_all_digits(arg)) {
        return (PARSE_ERROR);
    }

    *packet_len = atoi(arg);

    // check if it's in valid range
    if (*packet_len < 0 || *packet_len > MAX_PACKET_LEN) {
        char msg[MAX_ERROR_MSG_SIZE];
        snprintf(msg, MAX_ERROR_MSG_SIZE, "too big packetlen %s specified", arg);
        errorLogger(msg, EX_USAGE);
    }

    return (PARSE_OK);
}

static int no_space(const char *arg) {
    if (arg == NULL) {
        return (0);
    }

    for (size_t i = 0; arg[i]; i++) {
        if (isspace(arg[i])) {
            return (0);
        }
    }
    return (1);
}

int parse_input_address(const char *input, struct in_addr *addr, char *display_address) {
    if (!input || !addr || !display_address) {
        return (PARSE_ERROR);
    }

    if (!no_space(input)) {
        return (PARSE_ERROR);
    }

    if (inet_aton(input, addr) != 0) {
        // handle special address (0.0.0.0)
        if (addr->s_addr == 0) {
            inet_aton("127.0.0.1", addr);
        }

        const char *parsed_address = inet_ntoa(*addr);

        // handle 255.255.255.255
        if (strcmp(parsed_address, "255.255.255.255") == 0) {
            errorLogger("ft_traceroute to 255.255.255.255 (255.255.255.255), 30 hops max, 60 byte packets\nconnect: Permission denied", EXIT_FAILURE);
        }

        size_t parsed_address_len = strlen(parsed_address);
        strncpy(display_address, parsed_address, parsed_address_len); // inet_ntoa returns the underlying static buffer that is overwritten with each subsequent call
        display_address[parsed_address_len] = '\0';
        return (PARSE_OK);
    }

    struct addrinfo hints;
    struct addrinfo *res = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    if (getaddrinfo(input, NULL, &hints, &res) == 0) {
        struct sockaddr_in *sin = (struct sockaddr_in *)(res->ai_addr);
        *addr = sin->sin_addr;

        // handle special address (0.0.0.0)
        if (addr->s_addr == 0) {
            inet_aton("127.0.0.1", addr);
        }

        const char *parsed_address = inet_ntoa(*addr);

        size_t parsed_address_len = strlen(parsed_address);
        strncpy(display_address, parsed_address, parsed_address_len); // inet_ntoa returns the underlying static buffer that is overwritten with each subsequent call
        display_address[parsed_address_len] = '\0';
        freeaddrinfo(res);
        return (PARSE_OK);
    }

    return (PARSE_ERROR);
}

int parse_options(int argc, char **argv) {
    if (!argv) {
        errorLogger("parse_options: argument pointer cannot be NULL", EXIT_FAILURE);
    }

    int opt_index = 1;
    int port_specified = 0;

    while (opt_index < argc && argv[opt_index][0] == '-') {
        char *arg = argv[opt_index];

        if (strcmp(arg, "--help") == 0) {
            print_help(argv[0]);
            exit(EXIT_SUCCESS);
        } else if (strcmp(arg, "-T") == 0 || strcmp(arg, "--tcp") == 0) {
            state.method = TCP_TRACEROUTE;
            // set port to tcp default (80) only if port wasn't already specified by -p
            if (!port_specified) {
                state.dport = DEFAULT_TCP_PORT;
            }
        } else if (strcmp(arg, "-F") == 0 || strcmp(arg, "--dont-fragment") == 0) {
            state.dont_fragment = 1;
        } else if (strcmp(arg, "-I") == 0 || strcmp(arg, "--icmp") == 0) {
            state.method = ICMP_TRACEROUTE;
        } else if (strcmp(arg, "-U") == 0 || strcmp(arg, "--udp") == 0) {
            state.method = FIXED_UDP_TRACEROUTE;
            // set port to 'fixed udp' default (53) only if port wasn't already specified by -p
            if (!port_specified) {
                state.dport = DEFAULT_FIXED_UDP_PORT;
            }
        } else if (strcmp(arg, "-D") == 0 || strcmp(arg, "--dccp") == 0) {
            state.method = DCCP_TRACEROUTE;
        } else if (strcmp(arg, "-p") == 0) {
            // Check if there's a next argument
            if (opt_index + 1 >= argc) {
                char msg[MAX_ERROR_MSG_SIZE];
                snprintf(msg, MAX_ERROR_MSG_SIZE, "Option `-p' (argc %d) requires an argument: `-p port'", opt_index);
                errorLogger(msg, TRACEROUTE_EX_USAGE);
            }
 
            char *value_str = argv[++opt_index];
 
            // check if it's all digits
            if (!is_all_digits(value_str)) {
                char msg[MAX_ERROR_MSG_SIZE];
                snprintf(msg, MAX_ERROR_MSG_SIZE, "Cannot handle `-p' option with arg `%s' (argc %d)", value_str, opt_index - 1);
                errorLogger(msg, TRACEROUTE_EX_USAGE);
            }
 
            int value = atoi(value_str);
 
            if (value < 0 || value > MAX_PORT_NUMBER) {
                errorLogger("-p: value too large", EX_USAGE);
            }
 
            port_specified = 1;
            state.dport = value;
        } else if (strcmp(arg, "-sp") == 0) {
            // check if there's a next argument
            if (opt_index + 1 >= argc) {
                char msg[MAX_ERROR_MSG_SIZE];
                snprintf(msg, MAX_ERROR_MSG_SIZE, "Option `-sp' (argc %d) requires an argument: `-sp port'", opt_index);
                errorLogger(msg, TRACEROUTE_EX_USAGE);
            }
 
            char *value_str = argv[++opt_index];
 
            // check if it's all digits
            if (!is_all_digits(value_str)) {
                char msg[MAX_ERROR_MSG_SIZE];
                snprintf(msg, MAX_ERROR_MSG_SIZE, "Cannot handle `-sp' option with arg `%s' (argc %d)", value_str, opt_index - 1);
                errorLogger(msg, TRACEROUTE_EX_USAGE);
            }
 
            int value = atoi(value_str);
 
            if (value < 0 || value > MAX_PORT_NUMBER) {
                errorLogger("-sp: value too large", EX_USAGE);
            }
 
            state.sport = value;
        } else {
            char msg[MAX_ERROR_MSG_SIZE];
            snprintf(msg, MAX_ERROR_MSG_SIZE, "Bad option `%s' (argc %d)", argv[opt_index], opt_index);
            errorLogger(msg, TRACEROUTE_EX_USAGE);
        }
        opt_index += 1;
    }

    return (opt_index);
}
