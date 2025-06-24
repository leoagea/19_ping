/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   resolve.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 13:34:38 by lagea             #+#    #+#             */
/*   Updated: 2025/06/24 15:25:03 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

/**
 * Checks if the target is a valid hostname or IP address.
 * This function uses getaddrinfo to resolve the target
 * and checks if it is a valid IPv4 address.
 * If the target is a valid hostname,
 * it stores the hostname in the ping structure.
 * If the target is an IP address, it stores the IP address.
 * If the resolution fails, it prints an error message
 * and sets the ping structure to invalid.
 * @param ping   Pointer to the t_ping structure containing ping settings.
 * @param target The target hostname or IP address to resolve.
 * @return None
 */
void checkTarget(t_ping *ping, const char *target)
{
	if (!ping || !target) {
		print_error("Invalid ping or target.");
		ping->is_valid = false;
		return;
	}

	struct addrinfo	 hints;
	struct addrinfo *result = NULL;
	int				 status;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_flags = AI_CANONNAME;

	status = getaddrinfo(target, NULL, &hints, &result);
	if (status != 0) {
		print_error("unknown host\n");
		ping->is_valid = false;
		return;
	}

	struct addrinfo *rp;
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		if (rp->ai_family == AF_INET) {
			// Copy the address to our sockaddr_storage
			memcpy(&ping->addr, rp->ai_addr, rp->ai_addrlen);
			break;
		} else {
			print_error("Only IPv4 addresses are supported");
			ping->is_valid = false;
			freeaddrinfo(result);
			return;
		}
	}

	if (rp == NULL) {
		print_error("No suitable address found");
		ping->is_valid = false;
		freeaddrinfo(result);
		return;
	}

	// Check if target was a hostname (not an IP address)
	struct sockaddr_in sa4;
	bool			   is_ipv4 = (inet_pton(AF_INET, target, &sa4.sin_addr) == 1);

	if (!is_ipv4) {
		ping->target_hostname = strdup(target);
		if (!ping->target_hostname) {
			print_error("Memory allocation failed for hostname");
			ping->is_valid = false;
			freeaddrinfo(result);
			return;
		}
	} else {
		ping->target_hostname = NULL;
	}

	ping->target_ip = ((struct sockaddr_in *)&ping->addr)->sin_addr.s_addr;

	ping->is_valid = true;
	freeaddrinfo(result);
}
