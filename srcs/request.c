/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 17:34:06 by lagea             #+#    #+#             */
/*   Updated: 2025/06/19 18:41:09 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

/**
 * build_payload()
 *   payload: pointer to the first byte of your ICMP data area
 *
 * Writes:
 *   [ 0 .. sizeof(struct timeval)-1 ] = the send timestamp
 *   [ sizeof(struct timeval) .. payload_len-1 ] = 0xAA padding
 */
static void build_payload(char *payload, int payload_len)
{
    struct timeval send_ts;

    gettimeofday(&send_ts, NULL);

    memcpy(payload, &send_ts, sizeof(send_ts));

    memset(payload + sizeof(send_ts), 0xAA, payload_len - sizeof(send_ts));
}

/*
 * Function to build an ICMP Echo Request packet.
 * The packet structure is as follows:
 * - Type: 8 (Echo Request) 1 byte
 * - Code: 0 1 byte
 * - Checksum: 0 (will be calculated later) 2 bytes
 * - Identifier: 0 (can be set to a unique value) 2 bytes
 * - Sequence Number: 0 (can be set to a unique value) 2 bytes
 * - Payload: optional data (can be filled with any data, e.g., timestamp)
 */
void build_echo_request(char *buf, size_t payload_len, int count)
{
    struct icmphdr *icmph = (struct icmphdr*)buf;

    memset(buf, 0, sizeof(*icmph) + payload_len);

    icmph->type = ICMP_ECHO;
    icmph->code = 0;
    icmph->checksum = 0;

    icmph->un.echo.id = htons((uint16_t)getpid() & 0xFFFF);
    icmph->un.echo.sequence = htons(count++ & 0xFFFF);

    char *payload = (char *)buf + ICMP_HEADER_SIZE;
	build_payload(payload, payload_len);

    size_t icmp_len = sizeof(*icmph) + payload_len;
    // printf("ICMP size: %zu bytes\n", sizeof(*icmph));
    // printf("Payload size: %zu bytes\n", payload_len);
    // printf("ICMP packet size: %zu bytes\n", icmp_len);
    icmph->checksum = checksum(buf, icmp_len);
}

static int check_response_header(char *buf, int count)
{
    struct icmphdr *icmph = (struct icmphdr *)buf;

    if (icmph->type != ICMP_ECHOREPLY) {
        fprintf(stderr, "Received non-echo reply packet: type %d\n", icmph->type);
        return -1;
    }

    if (icmph->code != 0) {
        fprintf(stderr, "Received packet with non-zero code: %d\n", icmph->code);
        return -1;
    }

    if (icmph->un.echo.id != htons(getpid() & 0xFFFF)) {
        fprintf(stderr, "Received packet with invalid identifier: %d\n", ntohs(icmph->un.echo.id));
        return -1;
    }
    
    if (ntohs(icmph->un.echo.sequence) != count) {
        fprintf(stderr, "Received packet with invalid sequence number: %d\n", ntohs(icmph->un.echo.sequence));
        return -1;
    }
    return 0;
}

int handle_echo_reply(t_ping *ping, t_ping_stats *stats, char *buf)
{
    struct iphdr  *ip  = (struct iphdr *)buf;
    size_t iphl = ip->ihl * 4; 
	
	if (check_response_header(buf + iphl, ping->ping_count - 1) == -1) {
		fprintf(stderr, "Received invalid ICMP packet\n");
		return -1;
	}
	
	rtt_calculate(ping, stats, buf + iphl + ICMP_HEADER_SIZE);

    int ttl =  ip->ttl;
	print_ping_stats(ping, ttl);
	
	stats->packets_received++;
    return 0;
}