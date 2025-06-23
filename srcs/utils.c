/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 14:16:40 by lagea             #+#    #+#             */
/*   Updated: 2025/06/23 19:42:26 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

void print_error(const char *msg)
{
    char buf[1024] = {0};
    
    snprintf(buf, sizeof(buf), "ping: %s", msg);
	_(STDERR_FILENO, buf);
}

void usage(void)
{
	print_error("ping: missing host operand");
	print_error("Try 'ping --help' for more information.");
}

void checkTarget(t_ping *ping, const char *target)
{
    if (!ping || !target) {
        print_error("Invalid ping or target.");
        ping->is_valid = false;
        return;
    }

    struct addrinfo hints;
    struct addrinfo *result = NULL;
    int status;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_flags = AI_CANONNAME;

    status = getaddrinfo(target, NULL, &hints, &result);
    if (status != 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "%s: %s\n", 
                target, gai_strerror(status));
        print_error(error_msg);
        ping->is_valid = false;
        return;
    }

    struct addrinfo *rp;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        if (rp->ai_family == AF_INET) {
            // Copy the address to our sockaddr_storage
            memcpy(&ping->addr, rp->ai_addr, rp->ai_addrlen);
            break;
        }
        else{
            print_error("Only IPv4 addresses are supported");
            ping->is_valid = false;
            freeaddrinfo(result);
            return;
        }
    }

    if (rp == NULL) {
        print_error("No suitable address found");
        ping->is_valid = false;
        freeaddrinfo(result);
        return;
    }

    // Check if target was a hostname (not an IP address)
    struct sockaddr_in sa4;
    bool is_ipv4 = (inet_pton(AF_INET, target, &sa4.sin_addr) == 1);
    
    if (!is_ipv4) {
        ping->target_hostname = strdup(target);
        if (!ping->target_hostname) {
            print_error("Memory allocation failed for hostname");
            ping->is_valid = false;
            freeaddrinfo(result);
            return;
        }
    } else {
        ping->target_hostname = NULL;
    }

    ping->target_ip = ((struct sockaddr_in *)&ping->addr)->sin_addr.s_addr;

    ping->is_valid = true;
    freeaddrinfo(result);
}

void timeval_add(const struct timeval *a, const struct timeval *b, struct timeval *result)
{
    if (!a || !b || !result)
        return;
        
    result->tv_sec = a->tv_sec + b->tv_sec;
    result->tv_usec = a->tv_usec + b->tv_usec;
    if (result->tv_usec >= 1000000) {
        result->tv_sec++;
        result->tv_usec -= 1000000;
    }
}

void timeval_sub(const struct timeval *a, const struct timeval *b, struct timeval *result)
{
    if (!a || !b || !result)
        return;
        
    result->tv_sec = a->tv_sec - b->tv_sec;
    result->tv_usec = a->tv_usec - b->tv_usec;
    if (result->tv_usec < 0) {
        result->tv_sec--;
        result->tv_usec += 1000000;
    }
}

int timeval_cmp(const struct timeval *a, const struct timeval *b)
{
    if (!a || !b)
        return 0;
        
    if (a->tv_sec < b->tv_sec)
        return -1;
    if (a->tv_sec > b->tv_sec)
        return 1;
    if (a->tv_usec < b->tv_usec)
        return -1;
    if (a->tv_usec > b->tv_usec)
        return 1;
    return 0;
}

uint16_t checksum(void *buf, size_t len)
{
    uint32_t sum = 0;
    uint16_t *data = buf;

    // Sum up 16-bit words
    while (len > 1) {
        sum += *data++;
        len  -= 2;
    }

    // If there's a leftover byte, pad with zero to form a 16-bit word
    if (len == 1) {
        uint8_t  tail[2] = { *(uint8_t*)data, 0 };
        sum += *(uint16_t*)tail;
    }

    // Fold any carries from the top 16 bits into the lower 16 bits
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    // One's-complement and return
    return (uint16_t)~sum;
}

void signal_handler(int sig)
{
    if (sig == SIGINT){
		t_ping *ping = &g_data->ping[g_data->ping_index];
		t_ping_stats *stats = &g_data->stats[g_data->ping_index];
		print_global_stats(ping, stats);
		exit(EXIT_SUCCESS);
	}
}

void help(void)
{
    fprintf(stdout, "Usage: ping [options] <host>\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -h, --help        Show this help message and exit\n");
    fprintf(stdout, "  -v, --verbose     Enable verbose output\n");
}

void exit_failure(const char *msg)
{
    _(STDERR_FILENO, msg);
    exit(EXIT_FAILURE);
}

t_icmphdr *get_inner_icmp_header(char *buf)
{
    /* 1. Skip the OUTER IPv4 header */
    struct iphdr *ip_outer = (struct iphdr *)buf;
    size_t outer_ip_len    = ip_outer->ihl * 4;

    uint8_t *pointer = (uint8_t *)buf + outer_ip_len;

    /* 2. Skip the OUTER ICMP Time-Exceeded header (always 8 bytes) */
    pointer += 8;

    /* 3. p now points at the QUOTED inner IPv4 header */
    struct iphdr *ip_inner = (struct iphdr *)pointer;
    size_t inner_ip_len    = ip_inner->ihl * 4;
    pointer += inner_ip_len;

    /* 4. p now points at the QUOTED inner ICMP Echo header */
    struct icmphdr *icmp_inner = (struct icmphdr *)pointer;

    return icmp_inner;
}

t_icmphdr *get_outer_icmp_header(char *buf)
{
    /* 1. Skip the OUTER IPv4 header */
	struct iphdr *ip_outer = (struct iphdr *)buf;
	size_t outer_ip_len    = ip_outer->ihl * 4;
    
	uint8_t *pointer = (uint8_t *)buf + outer_ip_len;
    
	/* 2. Skip the OUTER ICMP Time-Exceeded header (always 8 bytes) */
	struct icmphdr *icmp_outer = (struct icmphdr *)pointer;

    return icmp_outer;
}

t_iphdr *get_outer_ip_header(char *buf)
{
    return (struct iphdr *)buf;
}

// Helper function to get IP string from sockaddr_storage
char* get_ip_string(const t_sockaddr *addr)
{
    static char ip_str[INET6_ADDRSTRLEN];
    
    struct sockaddr_in *addr4 = (struct sockaddr_in*)addr;
    inet_ntop(AF_INET, &addr4->sin_addr, ip_str, INET_ADDRSTRLEN);
    
    return ip_str;
}
