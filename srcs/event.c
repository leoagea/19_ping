/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   event.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 16:03:52 by lagea             #+#    #+#             */
/*   Updated: 2025/06/11 16:37:01 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

static int send_ping(t_ping *ping, t_ping_stats *stats)
{
	(void)stats; // stats is not used in this function, but could be used for logging or statistics
	(void)ping; // ping is not used in this function, but could be used for sending the ping
	
	// Implementation of sending an ICMP Echo Request
	// This function should fill the ICMP packet and send it through the socket
	// For simplicity, we assume it returns 0 on success
	return 0; // Placeholder for actual implementation
}

static int receive_ping(t_ping *ping, t_ping_stats *stats)
{
	(void)stats; // stats is not used in this function, but could be used for logging or statistics
	(void)ping; // ping is not used in this function, but could be used for receiving the ping

	// Implementation of receiving an ICMP Echo Reply
	// This function should read the ICMP packet from the socket and process it
	// For simplicity, we assume it returns 0 on success
	return 0; // Placeholder for actual implementation
}

int event_loop(t_ping *ping, t_ping_stats *stats)
{
	fd_set read_fds;
	struct timeval tv;

	while (ping->ping_count < PING_DEFAULT_COUNT){
		
		FD_ZERO(&read_fds);
		FD_SET(ping->sockfd, &read_fds);

		stats->next_ping_time = stats->last_ping_time + ping->ping_interval;
		double now = time(NULL);
		double timeout = MAX(0, stats->next_ping_time - now);
		
		tv.tv_sec = (long)timeout;
		tv.tv_usec = (long)((timeout - tv.tv_sec) * 1000000);
		
		int ready = select(ping->sockfd + 1, &read_fds, NULL, NULL, &tv);
		if (ready < 0) {
			perror("select failed");
			return -1;
		}

		now = time(NULL);
		
		if (ready && FD_ISSET(ping->sockfd, &read_fds)) {
			// Handle incoming Echo Reply
			if (receive_ping(ping, stats) < 0) {
				perror("Failed to receive ping");
				return -1;
			}
			stats->packets_received++;
		}
		
		if (now >= stats->next_ping_time) {
			// Send new Echo Request
			if (send_ping(ping, stats) < 0) {
				perror("Failed to send ping");
				return -1;
			}
			stats->packets_sent++;
			stats->last_ping_time = now;
			ping->ping_count++;
		}
	}
	return 0;
}