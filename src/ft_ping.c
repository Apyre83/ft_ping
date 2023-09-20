#include "ft_ping.h"


struct info g_info = {0, 0, 0, 1e6, 0, 0, 0};


int main(int argc, char *argv[]) {
	struct options	opts = {0};
	char		*hostname;
	parse_options(argc, argv, &opts, &hostname);

    
    signal(SIGINT, handle_signal);

    struct sockaddr_in dest;
    struct packet pkt;
    struct timeval tv_send, tv_receive;

    resolve_dns(hostname, &dest);
	print_start_message(hostname, &dest, &opts);
    setup_socket();
    prepare_packet(&pkt);

    while (1) {
        send_packet(&pkt, &dest, &tv_send);
        receive_and_print_packet(&pkt, &dest, &tv_receive, &tv_send);
        usleep(1000000);
    }

    close(g_info.sockfd);
    return 0;
}


/**
 * @brief Handles SIGINT signal (Ctrl+C)
 * @param signo The signal number
 */
void handle_signal(int signo) {
    if (signo == SIGINT) {
        print_statistics();
		close(g_info.sockfd);
        exit(0);
    }
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

    if (sendto(g_info.sockfd, pkt, sizeof(struct packet), 0, (struct sockaddr*)dest, sizeof(struct sockaddr_in)) <= 0) {
        exit_with_error("sendto");
    }
	g_info.transmitted++;
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

    ssize_t bytes_received = recvmsg(g_info.sockfd, &msg, 0);

    if (bytes_received <= 0) {
        exit_with_error("recvmsg");
    }

    gettimeofday(tv_receive, NULL);
    struct ip *ip_header = (struct ip *)recv_buffer;
    int ttl = ip_header->ip_ttl;

	g_info.received++;
    double rtt = ((tv_receive->tv_sec - tv_send->tv_sec) * 1000000 + (tv_receive->tv_usec - tv_send->tv_usec)) / 1000.0;
	g_info.total_time += rtt;
	if (rtt < g_info.min_time) g_info.min_time = rtt;
	if (rtt > g_info.max_time) g_info.max_time = rtt;
	g_info.total_time_squared += rtt * rtt;


    printf("%zd bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
           bytes_received - IP_HEADER_SIZE,
           inet_ntoa(r_addr->sin_addr),
           pkt->header.un.echo.sequence,
           ttl,
           rtt);
}
