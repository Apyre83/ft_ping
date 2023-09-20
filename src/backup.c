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

// Inclure les en-têtes de votre libft ici, si nécessaire

// Utilisez des constantes pour une meilleure lisibilité
#define PACKET_SIZE 64
#define IP_HEADER_SIZE 20  // Taille standard de l'en-tête IP

// Structure du paquet ICMP
struct packet {
    struct icmphdr header;
    char payload[PACKET_SIZE - sizeof(struct icmphdr)];
};

// Fonctions
unsigned short calculate_checksum(void *buf, int length);
void handle_signal(int signo);

// Variables globales
int sockfd;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <hostname>\n", argv[0]);
        exit(1);
    }

    signal(SIGINT, handle_signal);

    // Initialisation
    struct sockaddr_in dest;
    struct packet pkt;
    struct timeval tv_send, tv_receive;

    // Résolution DNS
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    if (getaddrinfo(argv[1], NULL, &hints, &res) != 0) {
        perror("getaddrinfo");
        exit(1);
    }

    if (res == NULL) {
        printf("Host not found.\n");
        exit(1);
    }

    // Initialisation du socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }
	int	ttl_val = 64;
	if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl_val, sizeof(ttl_val)) != 0) {
		perror("setsockopt");
		exit(1);
	}

    dest.sin_family = AF_INET;
    dest.sin_port = 0;
    dest.sin_addr = ((struct sockaddr_in*)res->ai_addr)->sin_addr;

    // Préparation du paquet ICMP
    memset(&pkt, 0, sizeof(pkt));
    pkt.header.type = ICMP_ECHO;
    pkt.header.un.echo.id = getpid() & 0xFFFF;

    // Boucle principale
    while (1) {
        pkt.header.un.echo.sequence++;
        pkt.header.checksum = 0;
        pkt.header.checksum = calculate_checksum(&pkt, sizeof(pkt));

        gettimeofday(&tv_send, NULL);

        if (sendto(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr*)&dest, sizeof(dest)) <= 0) {
            perror("sendto");
            exit(1);
        }



        struct sockaddr_in r_addr;
        socklen_t len = sizeof(r_addr);

		char recv_buffer[PACKET_SIZE + IP_HEADER_SIZE];  // Ajoutez de l'espace pour l'en-tête IP

		struct msghdr msg;
		struct iovec iov[1];

		memset(&msg, 0, sizeof(msg));
		memset(&iov, 0, sizeof(iov));

		iov[0].iov_base = recv_buffer;
		iov[0].iov_len = sizeof(recv_buffer);
		msg.msg_iov = iov;
		msg.msg_iovlen = 1;
		msg.msg_name = &r_addr;
		msg.msg_namelen = len;

		ssize_t bytes_received = recvmsg(sockfd, &msg, 0);

		if (bytes_received <= 0) {
			perror("recvmsg");
			continue;
		}

		gettimeofday(&tv_receive, NULL);

		// Extraire le TTL de l'en-tête IP
		struct ip *ip_header = (struct ip *)recv_buffer;
		int ttl = ip_header->ip_ttl;


		double rtt = ((tv_receive.tv_sec - tv_send.tv_sec) * 1000000 +
					  (tv_receive.tv_usec - tv_send.tv_usec)) / 1000.0;

		printf("%zd bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
			   bytes_received - IP_HEADER_SIZE, // Soustrayez la taille de l'en-tête IP pour obtenir la taille du paquet ICMP
			   inet_ntoa(r_addr.sin_addr),
			   pkt.header.un.echo.sequence,  // Notez que nous utilisons ntohs ici pour convertir en ordre d'octets de l'hôte
			   ttl,
			   rtt);





        usleep(1000000); // 1 seconde
    }

    freeaddrinfo(res);
    close(sockfd);
    return 0;
}

unsigned short calculate_checksum(void *buf, int length) {
    unsigned short *data = buf;
    unsigned int sum = 0;

    for (; length > 1; length -= 2) {
        sum += *data++;
    }

    if (length == 1) {
        sum += *(unsigned char*)data;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    return ~sum;
}

void handle_signal(int signo) {
    if (signo == SIGINT) {
        printf("\nExiting.\n");
        close(sockfd);
        exit(0);
    }
}

