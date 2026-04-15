#ifndef IP_H
#define IP_H

// @brief constructs ip header for traceroute probes and sets the result in the ip header field in the global state struct
void construct_ip_header(void);

// @brief updates ttl and checksum fields in ip header in the global state struct
void update_ip_header(void);

#endif
