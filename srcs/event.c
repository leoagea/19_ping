/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   event.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 16:03:52 by lagea             #+#    #+#             */
/*   Updated: 2025/06/13 16:10:53 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

static int send_ping(t_ping *ping, t_ping_stats *stats)
{
	(void)	stats; // stats is not used in this function, but could be used for logging or statistics
	char buf[MAX_PAYLOAD_SIZE + sizeof(struct icmp)];
	struct sockaddr_in dest_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = ping->target_ip
    };
	
	build_echo_request(&buf[0], MAX_PAYLOAD_SIZE, ping->ping_count);
	
	if (sendto(ping->sockfd, buf, sizeof(struct icmp) + MAX_PAYLOAD_SIZE, 0,
			(struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
				perror("sendto failed");
				return -1; // Indicate failure to send
	}
	printf("Packet sent to %s\n", ping->target_hostname ? ping->target_hostname : inet_ntoa(dest_addr.sin_addr));
	return 0; // Placeholder for actual implementation
}

static int receive_ping(t_ping *ping, t_ping_stats *stats)
{
	(void)stats; // stats is not used in this function, but could be used for logging or statistics
	(void)ping; // ping is not used in this function, but could be used for receiving the ping
	char buf[RECV_BUFFER_SIZE];

	while (recv(ping->sockfd, buf, sizeof(buf), 0) < 0) {
		if (errno == EINTR) {
			// Interrupted by a signal, continue receiving
			continue;
		} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
			// No data available, return to the event loop
			return 0; // No data received
		} else {
			perror("recv failed");
			return -1; // Indicate failure to receive
		}
	}
	if (check_response_header(buf, ping->ping_count - 1) == -1) {
		fprintf(stderr, "Received invalid ICMP packet\n");
		return -1; // Invalid packet received
	}
	rtt_calculate(stats, buf + sizeof(struct icmp));
	// Implementation of receiving an ICMP Echo Reply
	// This function should read the ICMP packet from the socket and process it
	// For simplicity, we assume it returns 0 on success
	return 0; // Placeholder for actual implementation
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

	if (safety_check(ping, stats) < 0) {
		return -1; // Safety check failed
	}
		
	ping->nfds = ping->sockfd + 1;
	gettimeofday(&stats->last_ping_time, NULL);

	while (ping->ping_count < PING_DEFAULT_COUNT){
		
		FD_ZERO(&read_fds);
		FD_SET(ping->sockfd, &read_fds);

		gettimeofday(&now, NULL);
		
		// Calculate next ping time
        next_ping = stats->last_ping_time;
        next_ping.tv_sec += ping->ping_interval;

        // Calculate timeout
		timeval_sub(&next_ping, &now, &tv);
		
		if (tv.tv_sec < 0 || (tv.tv_sec == 0 && tv.tv_usec < 0)) {
            tv.tv_sec = 0;
            tv.tv_usec = 0;
        }
		
		int ready = select(ping->nfds, &read_fds, NULL, NULL, &tv);
		gettimeofday(&now, NULL); // Update current time after select
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
				stats->last_ping_time = now; // Reset last ping time after timeout
				ping->ping_count++;
				continue; // Skip to next iteration
			}
		}
		
		if (ready && FD_ISSET(ping->sockfd, &read_fds)) {
			// Handle incoming Echo Reply
			if (receive_ping(ping, stats) < 0) {
				perror("Failed to receive ping");
				return -1;
			}
		}
		
		if (timeval_cmp(&now, &next_ping) >= 0) {
			// Send new Echo Request
			if (send_ping(ping, stats) < 0) {
				// TODO handle error
				// For exemple, log the error or increment a failure counter
				perror("Failed to send ping");
				return -1;
			}
			stats->packets_sent++;
			gettimeofday(&now, NULL);
			stats->last_ping_time = now; // Update last ping time
			ping->ping_count++;
			
			next_ping = now;
			next_ping.tv_sec += ping->ping_interval; // Schedule next ping
		}
	}
	return 0;
}