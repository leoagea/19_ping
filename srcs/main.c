/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 16:53:36 by lagea             #+#    #+#             */
/*   Updated: 2025/06/11 16:35:56 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

static void ping_loop(t_data *data)
{
	size_t i = 0;
	while (i < data->ping_size)
	{
		checkTarget(data, &data->ping[i], data->av[i + 1]);
		if (!data->ping[i].is_valid)
		{
			i++;
			continue;
		}
		init_socket(&data->ping[i]);
		if (event_loop(&data->ping[i], &data->stats[i]) < 0)
		{
			print_error("Event loop failed");
		}
		i++;
	}
}

int main(int ac, char **av)
{
	t_data data;
	
	if (ac < 2){
		usage();
		return 1;
	}
	data.ac = ac;
	data.av = av;
	init_data(&data);
	ping_loop(&data);
	debug_print_all_ping_struct(&data);
	free_data(&data);
	
	return 0;
}