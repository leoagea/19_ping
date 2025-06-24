/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   event.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 16:03:52 by lagea             #+#    #+#             */
/*   Updated: 2025/06/24 14:55:39 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

/**
 * Sends an ICMP Echo Request packet to the target IP address.
 * It builds the packet with the appropriate header and payload,
 * and sends it using the socket file descriptor.
 * @param ping   Pointer to the t_ping structure containing ping settings.
 * @return       0 on success, -1 on failure.
 */
static int send_ping(t_ping *ping)
{
	char			   buf[MAX_PAYLOAD_SIZE + ICMP4_HEADER_SIZE];
	struct sockaddr_in dest_addr = {.sin_family = AF_INET, .sin_addr.s_addr = ping->target_ip};

	build_echo_request(&buf[0], MAX_PAYLOAD_SIZE, ping->ping_count);

	if (sendto(ping->sockfd, buf, ICMP4_HEADER_SIZE + MAX_PAYLOAD_SIZE, 0,
			   (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
		perror("sendto failed");
		return -1;
	}

	return 0;
}

/**
 * Handles the ICMP Echo Reply message.
 * This function processes the received packet,
 * It also prints the ping statistics if not in quiet mode.
 * @param ping   Pointer to the t_ping structure containing ping settings.
 * @param stats  Pointer to the t_ping_stats structure to store statistics.
 * @param buf    The received packet buffer.
 * @return       0 on success, -1 on failure.
 */
static int receive_ping(t_ping *ping, t_ping_stats *stats)
{
	char			  *buf = ping->packet.recv_buffer;
	char			   pbuf[BUF_LEN] = {0};
	struct sockaddr_in addr;
	socklen_t		   addr_len = sizeof(addr);

	ping->packet.bytes_read =
		recvfrom(ping->sockfd, ping->packet.recv_buffer, sizeof(ping->packet.recv_buffer), 0,
				 (struct sockaddr *)&addr, &addr_len);

	if (ping->packet.bytes_read < 0) {
		perror("recvfrom failed");
		return -1;
	}

	ping->packet.iph_len = ((struct iphdr *)ping->packet.recv_buffer)->ihl * 4;
	struct icmphdr *ih = (void *)(buf + ping->packet.iph_len);

	if (ih->type == ICMP_ECHOREPLY)
		handle_echo_reply(ping, stats, ping->packet.recv_buffer);
	else if (ih->type == ICMP_DEST_UNREACH) {
		snprintf(pbuf, BUF_LEN, "Destination unreachable: %s\n", inet_ntoa(addr.sin_addr));
		stats->packets_lost++;
	} else if (ih->type == ICMP_TIME_EXCEEDED) {
		print_ttl_exceeded(ping, inet_ntoa(addr.sin_addr));
		stats->packets_lost++;
	} else if (ih->type != ICMP_ECHO) {
		snprintf(pbuf, BUF_LEN, "Received unexpected ICMP type: %d\n", ih->type);
		stats->packets_lost++;
	}

	_(STDOUT_FILENO, pbuf);
	return 0;
}

/** 
 * Safety check function to ensure the ping and stats structures are valid.
 * This function checks if the structures are not NULL, if the ping structure is valid,
 * and if the target hostname or IP is set. It also checks if the socket file descriptor
 * is valid.
 *
 * @param ping   Pointer to the t_ping structure containing ping settings.
 * @param stats  Pointer to the t_ping_stats structure to store statistics.
 * @return       0 on success, -1 on failure.
*/
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

/**
 * Main event loop for the ping program.
 * This function handles sending and receiving ICMP echo requests and replies,
 * manages timeouts, and prints statistics.
 *
 * @param ping   Pointer to the t_ping structure containing ping settings.
 * @param stats  Pointer to the t_ping_stats structure to store statistics.
 * @return       0 on success, -1 on failure.
*/
int event_loop(t_ping *ping, t_ping_stats *stats)
{
	fd_set	   read_fds;
	t_options *flag = g_data->arg;
	t_timeval  tv, now, next_ping, last_ping, start, elapsed;

	if (safety_check(ping, stats) < 0)
		return -1;

	if (flag->flood) {
		if (fcntl(ping->sockfd, F_SETFL, O_NONBLOCK) == -1) {
			print_error("Failed to set socket to non-blocking mode.");
			exit(EXIT_FAILURE);
		}
	}

	print_ping_info(ping);

	gettimeofday(&start, NULL);
	gettimeofday(&last_ping, NULL);

	if (send_ping(ping) < 0)
		return -1;
	stats->packets_sent++;
	ping->ping_count++;

	gettimeofday(&now, NULL);
	stats->last_ping_time = now;
	next_ping = last_ping;
	next_ping.tv_sec += ping->ping_interval;

	ping->nfds = ping->sockfd + 1;

	while (true) {
		FD_ZERO(&read_fds);
		FD_SET(ping->sockfd, &read_fds);

		gettimeofday(&now, NULL);

		if (flag->flood)
			next_ping = now;
		else {
			next_ping = stats->last_ping_time;
			next_ping.tv_sec += ping->ping_interval;
		}

		timeval_sub(&next_ping, &now, &tv);

		if (g_data->arg->timeout && g_data->arg->t_timeout.tv_sec > 0) {
			timeval_sub(&now, &start, &elapsed);
			if (elapsed.tv_sec >= g_data->arg->t_timeout.tv_sec) {
				if (stats->packets_lost + stats->packets_received != stats->packets_sent)
					stats->packets_lost =
						stats->packets_sent - (stats->packets_received + stats->packets_lost);
				print_global_stats(ping, stats);
				exit(EXIT_SUCCESS);
			}
		}

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
			timersub(&now, &last_ping, &diff);
			if (ping->ping_timeout > 0 && diff.tv_sec > (time_t)ping->ping_timeout)
				stats->packets_lost++;
		}

		if (ready && FD_ISSET(ping->sockfd, &read_fds)) {
			if (receive_ping(ping, stats) < 0)
				return -1;
		}

		if ((flag->flood || timeval_cmp(&now, &next_ping) >= 0) &&
			ping->ping_count < g_data->ping_count) {
			int send_status = send_ping(ping);
			if (send_status < 0) {
				perror("Failed to send ping");
				return -1;
			} else if (send_status == 0) {
				if (flag->flood)
					nanosleep(&(struct timespec){0, 100000}, NULL);
				stats->packets_sent++;
				ping->ping_count++;
				last_ping = now;
				if (!flag->flood) {
					gettimeofday(&now, NULL);
					stats->last_ping_time = now;
				}
			}

			next_ping = now;
			next_ping.tv_sec += ping->ping_interval;
		}

		if (stats->packets_received + stats->packets_lost >= (int)g_data->ping_count)
			break;
	}

	if (stats->packets_lost + stats->packets_received != stats->packets_sent)
		stats->packets_lost = stats->packets_sent - (stats->packets_received + stats->packets_lost);

	print_global_stats(ping, stats);

	return 0;
}