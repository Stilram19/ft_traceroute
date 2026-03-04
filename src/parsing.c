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
    printf("  -h            Show this help message\n");
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

    while (opt_index < argc && argv[opt_index][0] == '-') {
        char *arg = argv[opt_index];

        if (strcmp(arg, "-h") == 0) {
            print_help(argv[0]);
            exit(EXIT_SUCCESS);
        } else {
            char msg[MAX_ERROR_MSG_SIZE];
            snprintf(msg, MAX_ERROR_MSG_SIZE, "Bad option `%s' (argc %d)", argv[opt_index], opt_index);
            errorLogger(msg, TRACEROUTE_EX_USAGE);
        }
        opt_index += 1;
    }

    return (opt_index);
}
