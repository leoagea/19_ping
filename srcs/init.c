/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 14:28:17 by lagea             #+#    #+#             */
/*   Updated: 2025/06/03 14:59:38 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

void init_data(t_data *data, int ac)
{
	data->ping_size = ac - 1;
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