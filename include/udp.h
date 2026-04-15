#ifndef UDP_H
#define UDP_H 

// @brief constructs udp header for traceroute probes and sets the result in the udp header field in the global state struct
void construct_udp_header(void);

// @brief updates destination port in udp header for traceroute probes and sets the result in the udp header field in the global state struct
void update_udp_header(void);

#endif
