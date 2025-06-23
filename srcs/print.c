/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 15:21:22 by lagea             #+#    #+#             */
/*   Updated: 2025/06/23 19:43:11 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

void print_ping_info(t_ping *ping)
{
	char buf[BUF_LEN] = {0};
	size_t len = 0;
	
	len = snprintf(buf, BUF_LEN, "PING %s (%s): 56 data bytes",
		ping->target_hostname ? ping->target_hostname : get_ip_string((t_sockaddr *)&ping->addr),
		 get_ip_string((t_sockaddr *)&ping->addr));
		
	if (g_data->arg->verbose){
		size_t pid = getpid();
		len += snprintf(buf + len, BUF_LEN - len, ", id 0x%lX = %d", pid, (int)pid);
	}
	snprintf(buf + len, 2, "\n");

	_(STDOUT_FILENO, buf);
}

void print_global_stats(t_ping *ping, t_ping_stats *stats)
{
	char buf[BUF_LEN] = {0};
	size_t len = 0;
	char *hostname = ping->target_hostname ? ping->target_hostname : get_ip_string((t_sockaddr *)&ping->addr);

	len = snprintf(buf, BUF_LEN, "--- %s ping statistics ---\n", hostname);
	len += snprintf(buf + len, BUF_LEN - len, "%d packets transmitted, %d packets received, %.0f%% packet loss\n",
		stats->packets_sent, stats->packets_received,
		(double)stats->packets_lost / stats->packets_sent * 100.0);
	
	double avg_rtt = rtt_avg_calculate(ping);
	double stddev_rtt = stddev_calculate(ping, avg_rtt);
	if (stats->packets_received > 0) {
		len += snprintf(buf + len, BUF_LEN - len, "round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
			stats->min_rtt, avg_rtt, stats->max_rtt, stddev_rtt);
	}
	
	_(STDOUT_FILENO, buf);
}


void print_ping_stats(t_ping *ping)
{
	char buf[BUF_LEN] = {0};

	snprintf(buf, BUF_LEN, "%zu bytes from %s: icmp_seq=%ld ttl=%u time=%.3f ms\n",
		ping->packet.bytes_read - ping->packet.iph_len, 
		get_ip_string((t_sockaddr *)&ping->addr),
		ping->ping_count - 1, ping->ttl, ping->rtt[ping->ping_count - 1]);

	_(STDOUT_FILENO, buf);
}

void print_ttl_exceeded(t_ping *ping, char *ip_add)
{
	char buf[BUF_LEN] = {0};
	size_t len = 0;
	
	size_t bytes_len = ping->packet.bytes_read - ping->packet.iph_len;
	len = snprintf(buf, BUF_LEN, "%ld bytes from %s: Time to live exceeded\n", bytes_len, ip_add);
	
	if (g_data->arg->verbose){
		t_iphdr *ip_outer = get_outer_ip_header(ping->packet.recv_buffer);
		t_icmphdr *icmp_outer = get_outer_icmp_header(ping->packet.recv_buffer);
		t_icmphdr *icmp_inner = get_inner_icmp_header(ping->packet.recv_buffer);
		
		fill_IP_header_dump(ip_outer, buf, &len, (unsigned char *)ping->packet.recv_buffer);
		fill_IP_header(ip_outer, buf, &len);
		fill_ICMP_header(icmp_inner, icmp_outer, buf, &len, bytes_len);
	}	

	_(STDOUT_FILENO, buf);
}