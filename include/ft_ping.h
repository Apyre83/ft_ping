
#ifndef FT_PING_H
# define FT_PING_H


/* Includes */
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


/* Defines */
#define PACKET_SIZE 64
#define IP_HEADER_SIZE 20


/* Structures */
struct packet {
    struct icmphdr	header;
    char			payload[PACKET_SIZE - sizeof(struct icmphdr)];
};


struct info {
	int				sockfd;
	unsigned int	transmitted, received;
	double			min_time, max_time, total_time, total_time_squared;
	char			*hostname;
	struct in_addr	dest;
};

struct options {
	int	verbose;
};


/* Global variables */
extern struct info	g_info;


/* Main */
void			handle_signal(int signo);
void			send_packet(struct packet *pkt, struct sockaddr_in *dest, struct timeval *tv_send);
void			receive_and_print_packet(struct packet *pkt, struct sockaddr_in *r_addr, struct timeval *tv_receive, struct timeval *tv_send);

/* Utils */
void			*ft_memcpy(void *dest, const void *src, size_t n);
void			*ft_memset(void *s, int c, size_t n);
int				ft_strcmp(const char *s1, const char *s2);
void			exit_with_gai_error(const char *msg, int error_code);
void			exit_with_error(const char *msg);
unsigned short	calculate_checksum(void *buf, int length);
double			sqrt(double x);

/* Inits */
void			resolve_dns(const char *hostname, struct sockaddr_in *dest);
void			setup_socket(void);
void			prepare_packet(struct packet *pkt);

/* Print */
void			print_start_message(const char *hostname, struct sockaddr_in *dest, struct options *opts);
void			print_statistics(void);

/* Parsing */
void			parse_options(int argc, char *argv[], struct options *opts);




#endif /* FT_PING_H */
