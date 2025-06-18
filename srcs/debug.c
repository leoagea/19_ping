/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 15:43:34 by lagea             #+#    #+#             */
/*   Updated: 2025/06/18 16:29:56 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

void debug_print_all_ping_struct(t_data *data)
{
	size_t i;

	if (!data || !data->ping)
	{
		print_error("Data or ping structure is NULL.");
		return;
	}

	for (i = 0; i < data->ping_nb; i++)
	{
		printf("Ping %zu:\n", i + 1);
		printf("  Target Hostname: %s\n", data->ping[i].target_hostname ? data->ping[i].target_hostname : "N/A");
		printf("  Target IP raw: %d\n", data->ping[i].target_ip);
		printf("  Target IP: %s\n", data->ping[i].target_ip != INADDR_NONE ? inet_ntoa(*(struct in_addr *)&data->ping[i].target_ip) : "N/A");
		printf("  Is Valid: %s\n", data->ping[i].is_valid ? "true" : "false");
	}
}