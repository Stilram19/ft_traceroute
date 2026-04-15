#include <netinet/ip.h>
#include "ip.h"
#include "ft_traceroute.h"
#include "macros.h"
#include "utils.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern traceroute_state_t state;

static uint16_t calculate_checksum(struct ip *iph) {
    if (!iph) {
        errorLogger("fatal error: iph argument in NULL", EXIT_FAILURE);
    }
    uint32_t sum = 0;
 
    size_t size = sizeof(struct ip);
    uint8_t bytes[size];
    memcpy(bytes, iph, size);

    for (size_t i = 0; i < size; i += 2) {
        uint16_t word = (bytes[i] << 8) + bytes[i + 1];
        sum += word;
    }
 
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    // return negation of the sum (1's complement)
    return (~sum);
}

void construct_ip_header(void) {
    struct ip iph;

    iph.ip_v = 4; // ipv4
    iph.ip_hl = 5; // 20 bytes (5 * 4)
    iph.ip_tos = 0;
    iph.ip_len = htons(state.packet_len); // packet length
    iph.ip_id = htons(0); // no fragmenting
    iph.ip_off = htons(IP_DF); // don't fragment
    iph.ip_ttl = state.curr_ttl; // curr probe ttl

    // setting the protocol based on the used method
    switch (state.method) {
        case TCP_TRACEROUTE:
            iph.ip_p = IPPROTO_TCP;
            break;
        case ICMP_TRACEROUTE:
            iph.ip_p = IPPROTO_ICMP;
            break;
        case DCCP_TRACEROUTE:
            iph.ip_p = IPPROTO_DCCP;
            break;
        default:
            iph.ip_p = IPPROTO_UDP;
    }

    iph.ip_sum = 0;
    iph.ip_src = state.saddr; // source address (already in network byte order)
    iph.ip_dst = state.daddr; // destination address (already in network byte order)

    iph.ip_sum = htons(calculate_checksum(&iph));

    // set the result in state global struct
    state.iph = iph;
}

// (*) update ip header section

static uint16_t ones_complement_add(uint32_t sum) {
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    // return negation of the sum (1's complement)
    return (sum);
}

void update_ip_header(void) {
    // update checksum field
    uint16_t old_word = (state.iph.ip_ttl << 8) | state.iph.ip_p;
    uint16_t new_word = (state.curr_ttl << 8) | state.iph.ip_p;

    uint32_t sum = ~ntohs(state.iph.ip_sum) & 0xFFFF;
    sum += (~old_word & 0xFFFF);
    sum += new_word;
    sum = ones_complement_add(sum);
    state.iph.ip_sum = htons(~sum);

    // update ttl field
    state.iph.ip_ttl = state.curr_ttl;
}
