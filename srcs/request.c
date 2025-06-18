/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 17:34:06 by lagea             #+#    #+#             */
/*   Updated: 2025/06/18 15:14:52 by lagea            ###   ########.fr       */
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
    struct icmp *icmph = (struct icmp*)buf;

    memset(buf, 0, sizeof(*icmph) + payload_len);

    icmph->icmp_type = ICMP_ECHO;
    icmph->icmp_code = 0;
    icmph->icmp_cksum = 0;

    icmph->icmp_id = htons((uint16_t)getpid() & 0xFFFF);
    icmph->icmp_seq = htons(count++ & 0xFFFF);

    char *payload = (char *)buf + sizeof(struct icmp);
	build_payload(payload, payload_len);

    size_t icmp_len = sizeof(*icmph) + payload_len;
    icmph->icmp_cksum = checksum(buf, icmp_len);
}

int check_response_header(char *buf, int count)
{
    struct icmp *icmph = (struct icmp *)buf;

    if (icmph->icmp_type != ICMP_ECHOREPLY) {
        fprintf(stderr, "Received non-echo reply packet: type %d\n", icmph->icmp_type);
        return -1;
    }

    if (icmph->icmp_code != 0) {
        fprintf(stderr, "Received packet with non-zero code: %d\n", icmph->icmp_code);
        return -1;
    }

    if (icmph->icmp_id != htons(getpid() & 0xFFFF)) {
        fprintf(stderr, "Received packet with invalid identifier: %d\n", ntohs(icmph->icmp_id));
        return -1;
    }
    
    if (ntohs(icmph->icmp_seq) != count) {
        fprintf(stderr, "Received packet with invalid sequence number: %d\n", ntohs(icmph->icmp_seq));
        return -1;
    }
    return 0;
}
