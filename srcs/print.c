/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 15:21:22 by lagea             #+#    #+#             */
/*   Updated: 2025/06/26 14:42:36 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

/**
 * Prints the initial ping information, including the target hostname or IP address.
 * This function formats the output to include the target hostname or IP address,
 * the number of data bytes being sent, and optionally the process ID if verbose mode is enabled.
 * @param ping Pointer to the t_ping structure containing ping settings.
 * @return None
 */
void print_ping_info(t_ping *ping)
{
	char   buf[BUF_LEN] = {0};
	size_t len = 0;

	len = snprintf(buf, BUF_LEN, "PING %s (%s): 56 data bytes",
				   ping->target_hostname ? ping->target_hostname
										 : get_ip_string((t_sockaddr *)&ping->addr),
				   get_ip_string((t_sockaddr *)&ping->addr));

	if (g_data->arg->verbose) {
		size_t pid = getpid();
		len += snprintf(buf + len, BUF_LEN - len, ", id 0x%lX = %d", pid, (int)pid);
	}
	snprintf(buf + len, 2, "\n");

	_(STDOUT_FILENO, buf);
}

/**
 * Prints the global statistics of the ping operation.
 * This function formats the output to include the total number of packets sent,
 * received, and lost, as well as the round-trip time (RTT) statistics.
 * @param ping   Pointer to the t_ping structure containing ping settings.
 * @param stats  Pointer to the t_ping_stats structure containing statistics.
 * @return None
 */
void print_global_stats(t_ping *ping, t_ping_stats *stats)
{
	char   buf[BUF_LEN] = {0};
	size_t len = 0;
	float percent_lost = 0;
	char  *hostname =
		 ping->target_hostname ? ping->target_hostname : get_ip_string((t_sockaddr *)&ping->addr);

	len = snprintf(buf, BUF_LEN, "--- %s ping statistics ---\n", hostname);

	if (stats->packets_received == 0 && stats->packets_sent == 0) {
		percent_lost = 0;
	}
	else if (stats->packets_received == 0) {
		percent_lost = 100;
	} else {
		percent_lost = (stats->packets_lost * 100) / stats->packets_sent;
	}

	len += snprintf(buf + len, BUF_LEN - len,
					"%d packets transmitted, %d packets received, %.0f%% packet loss\n",
					stats->packets_sent, stats->packets_received,
					(double)percent_lost);

	double avg_rtt = rtt_avg_calculate(ping);
	double stddev_rtt = stddev_calculate(ping, avg_rtt);
	if (stats->packets_received > 0) {
		len += snprintf(buf + len, BUF_LEN - len,
						"round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", stats->min_rtt,
						avg_rtt, stats->max_rtt, stddev_rtt);
	}

	_(STDOUT_FILENO, buf);
}
/**
 * Prints the statistics of a single ping response.
 * This function formats the output to include the number of bytes received,
 * the IP address from which the response was received, the sequence number,
 * the Time to Live (TTL), and the round-trip time (RTT).
 * @param ping Pointer to the t_ping structure containing ping settings.
 * @return None
 */
void print_ping_stats(t_ping *ping)
{
	char buf[BUF_LEN] = {0};

	snprintf(buf, BUF_LEN, "%zu bytes from %s: icmp_seq=%ld ttl=%u time=%.3f ms\n",
			 ping->packet.bytes_read - ping->packet.iph_len,
			 get_ip_string((t_sockaddr *)&ping->addr), ping->ping_count - 1, ping->ttl,
			 ping->rtt[ping->ping_count - 1]);

	_(STDOUT_FILENO, buf);
}

/**
 * Prints a message indicating that the Time to Live (TTL) has been exceeded.
 * This function is called when an ICMP Time-Exceeded message is received.
 * It formats the output to include the number of bytes received and the IP address
 * from which the message was received.
 * @param ping   Pointer to the t_ping structure containing ping settings.
 * @param ip_add Pointer to the IP address string from which the Time-Exceeded message was
 * @return None
 */
void print_ttl_exceeded(t_ping *ping, char *ip_add)
{
	char   buf[BUF_LEN] = {0};
	size_t len = 0;

	size_t bytes_len = ping->packet.bytes_read - ping->packet.iph_len;
	len = snprintf(buf, BUF_LEN, "%ld bytes from %s: Time to live exceeded\n", bytes_len, ip_add);

	if (g_data->arg->verbose) {
		t_iphdr	  *ip_outer = get_outer_ip_header(ping->packet.recv_buffer);
		t_icmphdr *icmp_outer = get_outer_icmp_header(ping->packet.recv_buffer);
		t_icmphdr *icmp_inner = get_inner_icmp_header(ping->packet.recv_buffer);

		fill_IP_header_dump(ip_outer, buf, &len, (unsigned char *)ping->packet.recv_buffer);
		fill_IP_header(ip_outer, buf, &len);
		fill_ICMP_header(icmp_inner, icmp_outer, buf, &len, bytes_len);
	}

	_(STDOUT_FILENO, buf);
}