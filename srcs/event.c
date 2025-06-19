/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   event.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 16:03:52 by lagea             #+#    #+#             */
/*   Updated: 2025/06/19 15:39:09 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

static int send_ping(t_ping *ping, t_ping_stats *stats)
{
	(void)	stats;
	char buf[MAX_PAYLOAD_SIZE + sizeof(struct icmp)];
	struct sockaddr_in dest_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = ping->target_ip
    };
	
	build_echo_request(&buf[0], MAX_PAYLOAD_SIZE, ping->ping_count);
	
	if (sendto(ping->sockfd, buf, sizeof(struct icmp) + MAX_PAYLOAD_SIZE, 0,
			(struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
				perror("sendto failed");
				return -1;
	}
	
	return 0;
}

static int receive_ping(t_ping *ping, t_ping_stats *stats)
{
	char *buf = ping->packet.recv_buffer;
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	ping->packet.bytes_read = recvfrom(ping->sockfd, ping->packet.recv_buffer, sizeof(ping->packet.recv_buffer), 0, (struct sockaddr *)&addr, &addr_len);
	if (ping->packet.bytes_read < 0) {
		perror("recvfrom failed");
		return -1;
	}
	
	ping->packet.iph_len = ((struct iphdr *)ping->packet.recv_buffer)->ihl * 4;
	struct icmphdr *ih = (void *)(buf + ping->packet.iph_len);
    ping->packet.icmp_header = ih;

	if (ih->type == ICMP_ECHOREPLY)
		handle_echo_reply(ping, stats, ping->packet.recv_buffer);
	else if (ih->type == ICMP_DEST_UNREACH){
		fprintf(stderr, "Destination unreachable: %s\n", inet_ntoa(addr.sin_addr));
		stats->packets_lost++;
	}
	else if (ih->type == ICMP_TIME_EXCEEDED){
		print_ttl_exceeded(ping, inet_ntoa(addr.sin_addr));
		stats->packets_lost++;
	}
	else if (ih->type != ICMP_ECHO){
		fprintf(stderr, "Received unexpected ICMP type: %d\n", ih->type);
		stats->packets_lost++;
	}
	
	return 0;
}

static int safety_check(t_ping *ping, t_ping_stats *stats)
{
	if (!ping || !stats) {
		print_error("Ping or stats structure is NULL.");
		return -1;
	}
	if (!ping->is_valid) {
		print_error("Ping structure is not valid.");
		return -1;
	}
	
	if (!ping->target_hostname && !ping->target_ip) {
		print_error("Target hostname or IP is not set.");
		return -1;
	}
	
	if (ping->sockfd < 0) {
		print_error("Socket is not valid.");
		return -1;
	}

	return 0;
}

int event_loop(t_ping *ping, t_ping_stats *stats)
{
	fd_set read_fds;
	t_timeval tv, now, next_ping;

	if (safety_check(ping, stats) < 0)
		return -1;
		
	ping->nfds = ping->sockfd + 1;
	gettimeofday(&stats->last_ping_time, NULL);

	print_ping_info(ping);
	
	while (stats->packets_lost + stats->packets_received < PING_DEFAULT_COUNT){
		
		FD_ZERO(&read_fds);
		FD_SET(ping->sockfd, &read_fds);

		gettimeofday(&now, NULL);
		
        next_ping = stats->last_ping_time;
        next_ping.tv_sec += ping->ping_interval;

		timeval_sub(&next_ping, &now, &tv);
		
		if (tv.tv_sec < 0 || (tv.tv_sec == 0 && tv.tv_usec < 0)) {
            tv.tv_sec = 0;
            tv.tv_usec = 0;
        }
		
		int ready = select(ping->nfds, &read_fds, NULL, NULL, &tv);
		gettimeofday(&now, NULL);
		if (ready < 0) {
			if (errno == EINTR) {
				// Interrupted by a signal, continue the loop
				continue;
			}
			perror("select failed");
			return -1;
		}
		
		if (ready == 0) {
			// Timeout occurred, no data available
			t_timeval diff;
			timeval_sub(&now, &stats->last_ping_time, &diff);
			
			if (ping->ping_timeout > 0 && diff.tv_sec >= ping->ping_timeout) {
				stats->packets_lost++;
				stats->last_ping_time = now;
				ping->ping_count++;
				continue;
			}
		}
		
		if (ready && FD_ISSET(ping->sockfd, &read_fds)) {
			if (receive_ping(ping, stats) < 0) {
				perror("Failed to receive ping");
				return -1;
			}
		}
		
		if (timeval_cmp(&now, &next_ping) >= 0) {
			if (send_ping(ping, stats) < 0) {
				perror("Failed to send ping");
				return -1;
			}
			stats->packets_sent++;
			gettimeofday(&now, NULL);
			stats->last_ping_time = now;
			ping->ping_count++;
			
			next_ping = now;
			next_ping.tv_sec += ping->ping_interval;
		}
	}
	print_global_stats(ping, stats);

	return 0;
}