/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 17:34:06 by lagea             #+#    #+#             */
/*   Updated: 2025/06/24 14:53:58 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

/**
 * Builds the payload for the ICMP Echo Request packet.
 * This function fills the payload with a timestamp
 * and a padding of 0xAA bytes.
 * The timestamp is used to calculate the round-trip time (RTT)
 * when the Echo Reply is received.
 * @param payload     Pointer to the payload buffer.
 * @param payload_len Length of the payload to be filled.
 * @return None
 */
static void build_payload(char *payload, int payload_len)
{
	struct timeval send_ts;

	gettimeofday(&send_ts, NULL);

	memcpy(payload, &send_ts, sizeof(send_ts));

	memset(payload + sizeof(send_ts), 0xAA, payload_len - sizeof(send_ts));
}

/**
 * Builds an ICMP Echo Request packet.
 * This function initializes the ICMP header,
 * sets the type to ICMP_ECHO,
 * sets the code to 0,
 * calculates the checksum,
 * and fills the payload with a timestamp and padding.
 * @param buf         Pointer to the buffer where the ICMP packet will be built.
 * @param payload_len Length of the payload to be included in the ICMP packet.
 * @param count       The sequence number for the ICMP Echo Request.
 * @return None
 */
void build_echo_request(char *buf, size_t payload_len, int count)
{
	struct icmphdr *icmph = (struct icmphdr *)buf;

	memset(buf, 0, sizeof(*icmph) + payload_len);

	icmph->type = ICMP_ECHO;
	icmph->code = 0;
	icmph->checksum = 0;

	icmph->un.echo.id = htons((uint16_t)getpid() & 0xFFFF);
	icmph->un.echo.sequence = htons(count++ & 0xFFFF);

	char *payload = (char *)buf + ICMP4_HEADER_SIZE;
	build_payload(payload, payload_len);

	size_t icmp_len = sizeof(*icmph) + payload_len;
	icmph->checksum = checksum(buf, icmp_len);
}

/**
 * Checks the ICMP Echo Reply header for validity.
 * This function verifies that the packet is an Echo Reply,
 * the code is zero,
 * and the identifier and sequence number match the expected values.
 * @param buf   Pointer to the buffer containing the ICMP packet.
 * @param count The sequence number to check against.
 * @return       0 if the header is valid, -1 if invalid.
 */
static int check_response_header(char *buf, int count)
{
	char pbuf[BUF_LEN] = {0};
	struct icmphdr *icmph = (struct icmphdr *)buf;

	if (icmph->type != ICMP_ECHOREPLY) {
		snprintf(pbuf, BUF_LEN, "Received non-echo reply packet: type %d\n", icmph->type);
		print_error(pbuf);
		return -1;
	}

	if (icmph->code != 0) {
		snprintf(pbuf, BUF_LEN, "Received packet with non-zero code: %d\n", icmph->code);
		print_error(pbuf);
		return -1;
	}

	if (icmph->un.echo.id != htons(getpid() & 0xFFFF)) {
		snprintf(pbuf, BUF_LEN, "Received packet with invalid identifier: %d\n", ntohs(icmph->un.echo.id));
		print_error(pbuf);
		return -1;
	}

	if (!g_data->arg->flood && ntohs(icmph->un.echo.sequence) != count) {
		snprintf(pbuf, BUF_LEN, "Received packet with invalid sequence number: %d\n",
				ntohs(icmph->un.echo.sequence));
		print_error(pbuf);
		return -1;
	}
	return 0;
}

/**
 * Handles the ICMP Echo Reply message.
 * This function checks the response header for validity,
 * calculates the round-trip time (RTT),
 * and updates the statistics.
 * It also prints the ping statistics if not in quiet mode.
 * @param ping   Pointer to the t_ping structure containing ping settings.
 * @param stats  Pointer to the t_ping_stats structure to store statistics.
 * @param buf    The received packet buffer.
 * @return       0 on success, -1 on failure.
 */
int handle_echo_reply(t_ping *ping, t_ping_stats *stats, char *buf)
{
	char		  pbuf[BUF_LEN] = {0};
	struct iphdr *ip = (struct iphdr *)buf;
	size_t		  iphl = ip->ihl * 4;

	if (check_response_header(buf + iphl, ping->ping_count - 1) == -1) {
		snprintf(pbuf, BUF_LEN, "Received invalid ICMP packet\n");
		return -1;
	}

	rtt_calculate(ping, stats, buf + iphl + ICMP4_HEADER_SIZE);

	ping->ttl = ip->ttl;

	if (!g_data->arg->quiet)
		print_ping_stats(ping);

	stats->packets_received++;

	_(STDOUT_FILENO, pbuf);
	return 0;
}