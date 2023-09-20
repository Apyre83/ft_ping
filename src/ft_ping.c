#include "ft_ping.h"



int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <hostname>\n", argv[0]);
        exit(1);
    }

    signal(SIGINT, handle_signal);

    struct sockaddr_in dest;
    struct packet pkt;
    struct timeval tv_send, tv_receive;

    resolve_dns(argv[1], &dest);
    setup_socket();
    prepare_packet(&pkt);

    while (1) {
        send_packet(&pkt, &dest, &tv_send);
        receive_and_print_packet(&pkt, &dest, &tv_receive, &tv_send);
		usleep(1000000);
    }

    close(sockfd);
    return 0;
}

/**
 * @brief Calculates ICMP checksum
 * @param buf The buffer containing the data to checksum
 * @param length The length of the data in the buffer
 * @return The computed checksum
 */
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

/**
 * @brief Handles SIGINT signal (Ctrl+C)
 * @param signo The signal number
 */
void handle_signal(int signo) {
    if (signo == SIGINT) {
        printf("\nExiting.\n");
        close(sockfd);
        exit(0);
    }
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

    if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
        perror("getaddrinfo");
        exit(1);
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

/**
 * @brief Sets up the raw socket
 */
void setup_socket() {
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    int ttl_val = 64;
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl_val, sizeof(ttl_val)) != 0) {
        perror("setsockopt");
        exit(1);
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
 * @brief Sends an ICMP packet
 * @param pkt The packet to send
 * @param dest The destination address
 * @param tv_send The timestamp when the packet is sent
 */
void send_packet(struct packet *pkt, struct sockaddr_in *dest, struct timeval *tv_send) {
    pkt->header.un.echo.sequence++;
    pkt->header.checksum = 0;
    pkt->header.checksum = calculate_checksum(pkt, sizeof(struct packet));

    gettimeofday(tv_send, NULL);

    if (sendto(sockfd, pkt, sizeof(struct packet), 0, (struct sockaddr*)dest, sizeof(struct sockaddr_in)) <= 0) {
        perror("sendto");
        exit(1);
    }
}

/**
 * @brief Receives and prints ICMP packet details
 * @param pkt The packet to populate with received data
 * @param r_addr The source address of the received packet
 * @param tv_receive The timestamp when the packet is received
 * @param tv_send The timestamp when the packet was sent
 */
void receive_and_print_packet(struct packet *pkt, struct sockaddr_in *r_addr, struct timeval *tv_receive, struct timeval *tv_send) {
    socklen_t len = sizeof(struct sockaddr_in);
    char recv_buffer[PACKET_SIZE + IP_HEADER_SIZE];

    struct msghdr msg;
    struct iovec iov[1];

    memset(&msg, 0, sizeof(msg));
    memset(&iov, 0, sizeof(iov));

    iov[0].iov_base = recv_buffer;
    iov[0].iov_len = sizeof(recv_buffer);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_name = r_addr;
    msg.msg_namelen = len;

    ssize_t bytes_received = recvmsg(sockfd, &msg, 0);

    if (bytes_received <= 0) {
        perror("recvmsg");
        return;
    }

    gettimeofday(tv_receive, NULL);
    struct ip *ip_header = (struct ip *)recv_buffer;
    int ttl = ip_header->ip_ttl;

    double rtt = ((tv_receive->tv_sec - tv_send->tv_sec) * 1000000 + (tv_receive->tv_usec - tv_send->tv_usec)) / 1000.0;

    printf("%zd bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
           bytes_received - IP_HEADER_SIZE,
           inet_ntoa(r_addr->sin_addr),
           pkt->header.un.echo.sequence,
           ttl,
           rtt);
}
