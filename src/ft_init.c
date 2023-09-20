
#include "ft_ping.h"


/**
 * @brief Sets up the raw socket
 */
void setup_socket() {
    g_info.sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (g_info.sockfd < 0) {
        exit_with_error("socket");
    }

    int ttl_val = 64;
    if (setsockopt(g_info.sockfd, IPPROTO_IP, IP_TTL, &ttl_val, sizeof(ttl_val)) != 0) {
        exit_with_error("setsockopt");
    }
}

/**
 * @brief Prepares the ICMP packet for sending
 * @param pkt The packet to populate
 */
void prepare_packet(struct packet *pkt) {
    memset(pkt, 0, sizeof(struct packet));
    pkt->header.type = ICMP_ECHO;
    pkt->header.un.echo.id = getpid() & 0xFFFF;
}

/**
 * @brief Receives and prints ICMP packet details
 * @param pkt The packet to populate with received data
 * @param r_addr The source address of received packet
 * @param tv_receive The timestamp when the packet is received
 * @param tv_send The timestamp when the packet was sent
 */
void resolve_dns(const char *hostname, struct sockaddr_in *dest) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    int error = getaddrinfo(hostname, NULL, &hints, &res);
    if (error != 0) {
        exit_with_gai_error("getaddrinfo", error);
    }

    if (res == NULL) {
        printf("Host not found.\n");
        exit(1);
    }

    dest->sin_family = AF_INET;
    dest->sin_port = 0;
    dest->sin_addr = ((struct sockaddr_in*)res->ai_addr)->sin_addr;

    freeaddrinfo(res);
}
