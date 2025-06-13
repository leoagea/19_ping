/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 17:34:06 by lagea             #+#    #+#             */
/*   Updated: 2025/06/13 16:41:26 by lagea            ###   ########.fr       */
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
    // printf("PID: %d\n", pid);

    icmph->icmp_id = htons(pid & 0xFFFF);
    icmph->icmp_seq = htons(count);

	// Build the timestamped payload
	build_payload(buf + sizeof(struct icmp), payload_len);

    // 5. Compute and insert checksum over the whole ICMP packet
    size_t icmp_len = sizeof(*icmph) + payload_len;
    icmph->icmp_cksum = htons(checksum(buf, icmp_len));
}

int check_response_header(char *buf, int count)
{
    // struct icmp *icmph = (struct icmp*)buf;
    struct iphdr  *ip  = (struct iphdr *)buf;
    size_t iphl = ip->ihl * 4; 
    struct icmp *icmph = (void *)(buf + iphl);

    // uint16_t id_net = icmph->icmp_id;          /* value as it came off wire   */
    // uint16_t id_host = ntohs(id_net);          /* convert exactly **once**    */
    // printf("wire=0x%04x  host=%u\n", id_net, id_host);
    
    // Check if the type is ICMP_ECHOREPLY (0)
    if (icmph->icmp_type != ICMP_ECHOREPLY) {
        fprintf(stderr, "Received non-echo reply packet: type %d\n", icmph->icmp_type);
        return -1; // Not an Echo Reply
    }

    // Check if the code is 0
    if (icmph->icmp_code != 0) {
        fprintf(stderr, "Received packet with non-zero code: %d\n", icmph->icmp_code);
        return -1; // Non-zero code
    }

    // printf("sentID=%u recvID=%u\n",
    //    (unsigned)(getpid() & 0xFFFF),
    //    (unsigned)id);
    uint16_t id = icmph->icmp_id;
    // printf("ICMP ID: %d\n", ntohs(id));
    uint16_t want = (uint16_t)getpid() & 0xFFFF;
    // printf("wantID=  %d\n", ntohs(want));
    // printf("id: %04x\n", id);
    // printf("want: %04x\n", want);
    if (id != want) {
        fprintf(stderr, "Received packet with mismatched identifier: %d\n", ntohs(icmph->icmp_id));
        return -1; // Identifier does not match
    }
    
    if (ntohs(icmph->icmp_seq) != count) {
        fprintf(stderr, "Received packet with invalid sequence number: %d\n", ntohs(icmph->icmp_seq));
        return -1; // Invalid sequence number
    }
    return 0; // Valid Echo Reply
}

void rtt_calculate(t_ping_stats *stats, char *buf)
{
    struct timeval send_ts;
    struct timeval recv_ts;
    struct timeval result;
    
    double rtt;

    // Extract the send timestamp from the payload
    memcpy(&send_ts, buf, sizeof(send_ts));

    // Get the current time as the receive timestamp
    gettimeofday(&recv_ts, NULL);

    // Calculate RTT in seconds
    timeval_sub(&recv_ts, &send_ts, &result);
    rtt =  (result.tv_sec + result.tv_usec) / 1000000.0;

    stats->packets_received++;
    
    if (stats->packets_received == 1 || rtt < stats->min_rtt) {
        stats->min_rtt = rtt;
    }
    
    if (rtt > stats->max_rtt) {
        stats->max_rtt = rtt;
    }
    
    stats->avg_rtt += (rtt - stats->avg_rtt) / stats->packets_received;

    printf("RTT: %.3f ms\n", rtt * 1000);
}