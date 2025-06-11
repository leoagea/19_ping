/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 14:28:17 by lagea             #+#    #+#             */
/*   Updated: 2025/06/11 15:58:04 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

void init_data(t_data *data)
{
	data->ping_size = data->ac - 1;
	data->ping = calloc(data->ping_size + 1, sizeof(t_ping));
	if (!data->ping)
	{
		print_error("Memory allocation failed for ping structure.");
		exit(EXIT_FAILURE);
	}
	data->stats = calloc(data->ping_size + 1, sizeof(t_ping_stats));
	if (!data->stats)
	{
		free(data->ping);
		print_error("Memory allocation failed for ping stats structure.");
		exit(EXIT_FAILURE);
	}
}

void init_socket(t_ping *ping)
{
	ping->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (ping->sockfd < 0)
	{
		print_error("Failed to create raw socket.");
		ping->is_valid = false;
		return ;
	}
	ping->ping_count = 0;
	ping->ping_interval = 1;
	ping->ping_timeout = 5;

	int status;

	struct timeval tv = { .tv_sec = 1, .tv_usec = 0 };
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
		if (errno != ENOPROTOOPT){
			perror("setsockopt IP_RECVTTL");
			close(ping->sockfd);
			ping->is_valid = false;
			return;
		}
	}
}