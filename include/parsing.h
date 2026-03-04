#ifndef PARSING_H
#define PARSING_H

#include <netinet/in.h>

// @brief parses the input IPv4 (address / hostname);
// * initializes the struct pointed to by 'addr'
// * copies the parsed input address into the pointer pointed to by display_address
// * '*display_address' must point an allocated block of memory with at least (MAX_IPV4_ADDR_LEN + 1) bytes
// @return PARSE_ERROR if input is neither an IPv4 address nor a hostname
int parse_input_address(const char *input, struct in_addr *addr, char *display_address);

// @brief parses supported options
// @return on success, the index of the first non-option argument (typically the host); exits on error and does not return
int parse_options(int argc, char **argv);

void print_help(char *program_name);

// @brief parses the packet_len argument, and sets the value in parse_packet_len
// @return PARSE_ERROR to indicate error and PARSE_OK to indicate success
int parse_packet_len(char *arg, int *packet_len);

#endif
