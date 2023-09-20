
#ifndef FT_PING_H
# define FT_PING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/time.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/ip.h>


#define PACKET_SIZE 64
#define IP_HEADER_SIZE 20


struct packet {
    struct icmphdr header;
    char payload[PACKET_SIZE - sizeof(struct icmphdr)];
};


unsigned short calculate_checksum(void *buf, int length);
void handle_signal(int signo);
void resolve_dns(const char *hostname, struct sockaddr_in *dest);
void setup_socket();
void prepare_packet(struct packet *pkt);
void send_packet(struct packet *pkt, struct sockaddr_in *dest, struct timeval *tv_send);
void receive_and_print_packet(struct packet *pkt, struct sockaddr_in *r_addr, struct timeval *tv_receive, struct timeval *tv_send);


int sockfd;


#endif /* FT_PING_H */
