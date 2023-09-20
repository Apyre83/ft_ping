
#include "ft_ping.h"

void	parse_options(int argc, char *argv[], struct options *opts, char **hostname) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s [options] <hostname>\nTry %s -? for more information ", argv[0], argv[0]);
		exit(1);
	}

	int	has_hostname = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            opts->verbose = 1;
        }
		else if (strcmp(argv[i], "-?") == 0) {
            printf("Usage: %s [options] <hostname>\n", argv[0]);
            printf("Options:\n");
            printf("  -v    Verbose mode\n");
            printf("  -?    Show this help message\n");
            exit(0);
        }
		else {
			if (has_hostname) {
				fprintf(stderr, "For this project, only one hostname is allowed.\n");
				exit(1);
			}
			*hostname = argv[i];
        }
    }
}