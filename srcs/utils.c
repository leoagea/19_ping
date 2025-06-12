/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 14:16:40 by lagea             #+#    #+#             */
/*   Updated: 2025/06/12 15:16:13 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

void print_error(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
}

void usage(void)
{
	print_error("ping: missing host operand");
	print_error("Try 'ping --help' for more information.");
}

static bool isValidIpAddress(const char *ipAddress)
{
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
	return result != 0;
}

void checkTarget(t_ping *ping, const char *target)
{
	if (!ping || !target) {
		print_error("Invalid ping or target.");
		ping->is_valid = false;
		return ;
	}
	
	if (isValidIpAddress(target)){
		ping->target_ip = inet_addr(target);
		if (ping->target_ip == INADDR_NONE) {
			print_error("Invalid IP address format.");
			ping->is_valid = false;
			return ;
		}
		ping->is_valid = true;
		return ;
	}

	struct hostent *he = gethostbyname(target);
	if (he == NULL){
		ping->is_valid = false;
		print_error("ping: unknown host");
		exit(EXIT_FAILURE); 
	}
	
	ping->target_hostname = strdup(target);
	ping->target_ip = *(in_addr_t *)he->h_addr_list[0];
	if (ping->target_ip == INADDR_NONE) {
		print_error("Invalid hostname resolution.");
		freePointer((void **)&ping->target_hostname);
		ping->is_valid = false;
		return ;
	}
	
	ping->is_valid = true;
}