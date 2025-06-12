/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 17:34:06 by lagea             #+#    #+#             */
/*   Updated: 2025/06/12 18:43:57 by lagea            ###   ########.fr       */
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

    // 1) Grab the current time
    gettimeofday(&send_ts, NULL);

    // 2) Copy the struct timeval into the start of the payload
    memcpy(payload, &send_ts, sizeof(send_ts));

    // 3) Pad the remainder with 0xAA
    for (int i = sizeof(send_ts); i < payload_len; i++) {
        payload[i] = 0xAA;
    }
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

    // 1. Zero everything out (so checksum starts as 0)
    memset(buf, 0, sizeof(*icmph) + payload_len);

    // 2. Fill fixed fields
    icmph->icmp_type           = ICMP_ECHO;     // Type 8 = Echo Request
    icmph->icmp_code           = 0;              // Code 0
    icmph->icmp_cksum          = 0;              // checksum = 0 for now

    // 3. Put your PID into the identifier field
    uint16_t pid = (uint16_t)getpid();

    icmph->icmp_id = htons(pid);
    icmph->icmp_seq = htons(count);

	// Build the timestamped payload
	build_payload(buf + sizeof(struct icmp), payload_len);

    // 5. Compute and insert checksum over the whole ICMP packet
    size_t icmp_len = sizeof(*icmph) + payload_len;
    icmph->icmp_cksum = htons(checksum(buf, icmp_len));
}
