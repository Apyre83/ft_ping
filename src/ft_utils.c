
#include "ft_ping.h"


void exit_with_error(const char *msg) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

void exit_with_gai_error(const char *msg, int error_code) {
    fprintf(stderr, "%s: %s\n", msg, gai_strerror(error_code));
    exit(1);
}

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t	i;

	if (!dest && !src)
		return (dest);
	i = 0;
	while (i < n)
	{
		((char *) dest)[i] = ((char *) src)[i];
		i++;
	}
	return (dest);
}


void	*ft_memset(void *s, int c, size_t n)
{
	int	i;

	i = 0;
	while ((size_t)i < n)
		((char *) s)[i++] = c;
	return (s);
}

// Ft_strcmp
int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (((unsigned char *) s1)[i] && ((unsigned char *) s2)[i]
		&& ((unsigned char *) s1)[i] == ((unsigned char *) s2)[i])
		i++;
	return (((unsigned char *) s1)[i] - ((unsigned char *) s2)[i]);
}



double fabs(double x) {
    if (x < 0) {
        return -x;
    }
    return x;
}


double sqrt(double x) {
    if (x < 0) {
        printf("Nombre négatif\n");
        return -1;
    }
	if (x == 0) {
		return 0;
	}

    double epsilon = 1e-10;
    int max_iter = 1000;

    double guess = x;

    for (int i = 0; i < max_iter; ++i) {
        guess = 0.5 * (guess + x / guess);

        if (fabs(guess * guess - x) < epsilon) {
            return guess;
        }
    }

    return guess;
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
