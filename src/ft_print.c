
#include "ft_ping.h"

void	print_start_message(const char *hostname, struct sockaddr_in *dest, struct options *opts) {
	if (opts->verbose) {
		pid_t pid = getpid();
		printf("PING %s (%s): 56 data bytes, id 0x%x = %d\n", hostname, inet_ntoa(dest->sin_addr), pid, pid);
	}
	else {
		printf("PING %s (%s): 56 data bytes\n", hostname, inet_ntoa(dest->sin_addr));
	}
}

void	print_statistics(void) {
    printf("\n--- ping statistics ---\n");
    printf("%u packets transmitted, %u packets received, %.1f%% packet loss\n",
           g_info.transmitted, g_info.received, 100.0 * (g_info.transmitted - g_info.received) / g_info.transmitted);
    if (g_info.received > 0) {
        double avg_time = g_info.total_time / g_info.received;
        double stddev = sqrt((g_info.total_time_squared / g_info.received) - (avg_time * avg_time));

        printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
               g_info.min_time, avg_time, g_info.max_time, stddev);
    }
}
