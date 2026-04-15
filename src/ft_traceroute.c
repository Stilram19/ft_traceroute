#include "ft_traceroute.h"
#include "ip.h"
#include "macros.h"
#include "udp.h"
#include <stdint.h>
#include <string.h>

extern traceroute_state_t state;

static void build_ip_header(void) {
    construct_ip_header(); 
}

static void build_proto_header(void) {

}

static void serialize_packet(void) {

}

static void construct_next_probe(void) {
    // construct/update ip header and method proto header
    if (state.curr_ttl == 1) {
        construct_ip_header();

        switch (state.method) {
            case TCP_TRACEROUTE:
                // construct_tcp_header();
                break;
            case ICMP_TRACEROUTE:
                // construct_icmp_header();
                break;
            case DCCP_TRACEROUTE:
                // construct_dccp_header();
                break;
            default:
                construct_udp_header();
        }
    } else {
        update_ip_header();

        switch (state.method) {
            case TCP_TRACEROUTE:
                // update_tcp_header();
                break;
            case ICMP_TRACEROUTE:
                // update_icmp_header();
                break;
            case DCCP_TRACEROUTE:
                // update_dccp_header();
                break;
            default:
                update_udp_header();
        }
    }

    // copy headers to packet memory & add padding bytes

    void *proto_hdr;
    size_t proto_hdr_len = 0;

    switch (state.method) {
        case TCP_TRACEROUTE:
            // proto_hdr = state.tcph;
            // proto_hdr_len = 20
            break;
        case ICMP_TRACEROUTE:
            // proto_hdr = state.icmph;
            // proto_hdr_len = 8 
            break;
        case DCCP_TRACEROUTE:
            // proto_hdr = state.dccph;
            // proto_hdr_len = 16 
            break;
        default:
            proto_hdr = &state.udph;
            proto_hdr_len = 8;
    }

    uint8_t *packet = state.packet;

    memcpy(packet, &state.iph, sizeof(state.iph)); // copy ip header
    memcpy(packet + sizeof(state.iph), proto_hdr, proto_hdr_len); // copy proto header

    // padding bytes
    for (uint16_t i = sizeof(state.iph) + proto_hdr_len; i < state.packet_len; i++) {
        packet[i] = 0x00;
    }
}

static void send_next_probe(void) {

}

void start(void) {
    for (size_t ttl = 1; ttl <= state.max_ttl; ttl++) {
        state.curr_ttl = ttl;
        for (size_t probe = 0; probe < state.probes_per_ttl; probe++) {
            construct_next_probe();
            send_next_probe();
        }
    }
}
