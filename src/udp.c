#include "ft_traceroute.h"
#include "udp.h"
#include <netinet/udp.h>

extern traceroute_state_t state;

void construct_udp_header(void) {
    struct udphdr udph;

    udph.uh_sport = htons(state.sport);
    udph.uh_dport = htons(state.curr_dport);
    udph.uh_ulen = htons((uint16_t)(state.packet_len - sizeof(struct ip))); // packet_len - ipheader_len(20)
    udph.uh_sum = 0; // optional for ipv4

    state.udph = udph;
}

void update_udp_header(void) {
    state.udph.uh_dport = state.curr_dport;
}
