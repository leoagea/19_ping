/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 15:43:34 by lagea             #+#    #+#             */
/*   Updated: 2025/06/24 14:44:39 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

/**
 * Debugging functions to print the internal state of the ping structures.
 * These functions are useful for debugging and understanding the current state
 * of the ping operations.
 * @param data Pointer to the t_data structure containing ping information.
 * @return None
*/
void debug_print_all_ping_struct(t_data *data)
{
	size_t i;

	if (!data || !data->ping) {
		print_error("Data or ping structure is NULL.");
		return;
	}

	for (i = 0; i < data->ping_nb; i++) {
		printf("Ping %zu:\n", i + 1);
		printf("  Target Hostname: %s\n",
			   data->ping[i].target_hostname ? data->ping[i].target_hostname : "N/A");
		printf("  Target IP raw: %d\n", data->ping[i].target_ip);
		printf("  Target IP: %s\n", data->ping[i].target_ip != INADDR_NONE
										? inet_ntoa(*(struct in_addr *)&data->ping[i].target_ip)
										: "N/A");
		printf("  Is Valid: %s\n", data->ping[i].is_valid ? "true" : "false");
	}
}

/**
 * Debugging function to print the statistics of a ping operation.
 * This function prints the number of packets sent, received, lost,
 * and the minimum and maximum round-trip times (RTT).
 * @param stats Pointer to the t_ping_stats structure containing statistics.
 * @return None
*/
void debug_print_stats(t_ping_stats *stats)
{
	if (!stats) {
		print_error("Stats structure is NULL.");
		return;
	}

	printf("Ping Statistics:\n");
	printf("  Packets Sent: %d\n", stats->packets_sent);
	printf("  Packets Received: %d\n", stats->packets_received);
	printf("  Packets Lost: %d\n", stats->packets_lost);
	printf("  Min RTT: %.3f ms\n", stats->min_rtt);
	printf("  Max RTT: %.3f ms\n", stats->max_rtt);
}