/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 15:21:22 by lagea             #+#    #+#             */
/*   Updated: 2025/06/19 13:38:22 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

void print_ping_info(t_ping *ping)
{
	char *buf = calloc(256, sizeof(char));
	if  (!buf) {
		print_error("Memory allocation failed for ping info buffer.");
		return;
	}
	
	int len = snprintf(buf, 256, "PING %s (%s): 56 data bytes",
		ping->target_hostname ? ping->target_hostname : inet_ntoa(*(struct in_addr *)&ping->target_ip),
		inet_ntoa(*(struct in_addr *)&ping->target_ip));
		
	if (g_data->arg->verbose){
		size_t pid = getpid();
		len += snprintf(buf + len, 256 - len, ", id 0x%lX = %d", pid, (int)pid);
	}
	snprintf(buf + len, 2, "\n");
	fprintf(stdout, "%s", buf);

	free(buf);
}

void print_global_stats(t_ping *ping, t_ping_stats *stats)
{
	char *hostname = ping->target_hostname ? ping->target_hostname : inet_ntoa(*(struct in_addr *)&ping->target_ip);
	
	fprintf(stdout, "--- %s ping statistics ---\n", hostname);
	fprintf(stdout, "%d packets transmitted, %d packets received, %.0f%% packet loss\n",
		stats->packets_sent, stats->packets_received,
		(double)stats->packets_lost / stats->packets_sent * 100.0);
	
	double avg_rtt = rtt_avg_calculate(ping);
	double stddev_rtt = stddev_calculate(ping, avg_rtt);
	if (stats->packets_received > 0) {
		fprintf(stdout, "round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
			stats->min_rtt, avg_rtt, stats->max_rtt, stddev_rtt);
	}
}


void print_ping_stats(t_ping *ping, int ttl)
{
	fprintf(stdout, "64 bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
		inet_ntoa(*(struct in_addr *)&ping->target_ip),
		ping->ping_count - 1, ttl, ping->rtt[ping->ping_count - 1]);
}