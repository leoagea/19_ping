/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   event.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 16:03:52 by lagea             #+#    #+#             */
/*   Updated: 2025/06/18 15:16:15 by lagea            ###   ########.fr       */
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
	char buf[RECV_BUFFER_SIZE];
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	ssize_t bytes = recvfrom(ping->sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&addr, &addr_len);
	if (bytes < 0) {
		perror("recvfrom failed");
		return -1;
	}
	
	struct iphdr  *ip  = (struct iphdr *)buf;
    size_t iphl = ip->ihl * 4; 
	
	if (check_response_header(buf + iphl, ping->ping_count - 1) == -1) {
		fprintf(stderr, "Received invalid ICMP packet\n");
		return -1;
	}
	
	rtt_calculate(ping, stats, buf + iphl + sizeof(struct icmp));

	print_ping_stats(ping);
	
	stats->packets_received++;
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