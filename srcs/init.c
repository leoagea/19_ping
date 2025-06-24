/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 14:28:17 by lagea             #+#    #+#             */
/*   Updated: 2025/06/24 14:42:30 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

/**
 * Initializes the global data structure with default values.
 * This function allocates memory for the ping and stats arrays,
 * sets the ping index to zero, and initializes the ping count
 * if it is not already set.
 * If memory allocation fails, it prints an error message and exits.
 * @param data Pointer to the t_data structure to be initialized.
 * @return None
 */
void init_data(t_data *data)
{
	data->ping_index = 0;

	if (data->ping_count == 0)
		data->ping_count = PING_DEFAULT_COUNT;

	data->ping = calloc(data->ping_nb + 1, sizeof(t_ping));
	if (!data->ping) {
		print_error("Memory allocation failed for ping structure.");
		exit(EXIT_FAILURE);
	}

	data->stats = calloc(data->ping_nb + 1, sizeof(t_ping_stats));
	if (!data->stats) {
		free(data->ping);
		print_error("Memory allocation failed for ping stats structure.");
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < data->ping_nb; i++) {
		memset(&data->ping[i], 0, sizeof(t_ping));
		memset(&data->stats[i], 0, sizeof(t_ping_stats));
	}
}

/**
 * Initializes the options structure with default values.
 * This function sets all fields of the options structure to zero
 * or false, preparing it for use.
 * @param options Pointer to the t_options structure to be initialized.
 * @return None
 */
void init_options(t_options *options)
{
	if (!options)
		return;

	memset(options, 0, sizeof(t_options));
}

/**
 * Initializes the signal handlers for SIGINT, SIGTERM, and SIGQUIT.
 * This function sets up the global signal action structure and registers
 * the signal handlers for the specified signals.
 * If any error occurs during the setup, it prints an error message and exits.
 * @param None
 * @return None
 */
void init_signals(void)
{
	memset(&g_sigact, 0, sizeof(struct sigaction));
	g_sigact.sa_handler = signal_handler;
	sigemptyset(&g_sigact.sa_mask);
	g_sigact.sa_flags = 0;

	if (sigaction(SIGINT, &g_sigact, NULL) < 0 || sigaction(SIGTERM, &g_sigact, NULL) < 0 ||
		sigaction(SIGQUIT, &g_sigact, NULL) < 0) {
		perror("Failed to set signal handlers");
		exit(EXIT_FAILURE);
	}
}

/**
 * Initializes the socket for ICMP ping operations.
 * It sets the necessary socket options and prepares the ping structure.
 * If any error occurs during initialization, it sets the ping structure to invalid.
 * @param ping Pointer to the t_ping structure to be initialized.
 * @return None
 */
void init_socket(t_ping *ping)
{
	if (!ping) {
		print_error("Ping structure is NULL.");
		return;
	}

	if (!ping->is_valid) {
		print_error("Ping structure is not valid.");
		return;
	}

	ping->rtt = calloc(g_data->ping_count, sizeof(double));
	if (!ping->rtt) {
		print_error("Memory allocation failed for RTT array.");
		ping->is_valid = false;
		return;
	}

	ping->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (ping->sockfd < 0) {
		print_error("Failed to create socket.");
		ping->is_valid = false;
		return;
	}
	ping->ping_count = 0;
	ping->ping_interval =
		(g_data->arg->ping_interval > 0) ? g_data->arg->ping_interval : PING_DEFAULT_INTERVAL;
	ping->ping_timeout =
		(g_data->arg->ping_interval > 0) ? g_data->arg->ping_interval : PING_DEFAULT_TIMEOUT;

	int status;

	struct timeval tv = {.tv_sec = 1, .tv_usec = 0};
	status = setsockopt(ping->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
	if (status == -1) {
		print_error("setsockopt SO_RCVTIMEO");
		close(ping->sockfd);
		ping->is_valid = false;
		return;
	}

	int ttl = 64;
	status = setsockopt(ping->sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof ttl);
	if (status == -1) {
		perror("setsockopt IP_TTL");
		close(ping->sockfd);
		ping->is_valid = false;
		return;
	}

	/* 3.  Ask the kernel to hand us the TTL that came back in each reply
		(Linux: IP_RECVTTL; BSDs use IP_RECVIPV4TTL) */
	int on = 1;
	status = setsockopt(ping->sockfd, IPPROTO_IP, IP_RECVTTL, &on, sizeof on);
	if (status == -1) {
		/* ENOPROTOOPT just means the kernel doesn’t support this feature */
		if (errno != ENOPROTOOPT) {
			perror("setsockopt IP_RECVTTL");
			close(ping->sockfd);
			ping->is_valid = false;
			return;
		}
	}
}