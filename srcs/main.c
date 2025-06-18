/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 16:53:36 by lagea             #+#    #+#             */
/*   Updated: 2025/06/18 15:21:30 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

t_data *g_data = NULL;
struct sigaction g_sigact;

static void ping_loop(t_data *data)
{
	size_t i = 0;
	while (i < data->ping_size)
	{
		checkTarget(&g_data->ping[i], g_data->av[i + 1]);
		// debug_print_all_ping_struct(g_data);
		init_socket(&g_data->ping[i]);
		if (event_loop(&g_data->ping[i], &g_data->stats[i]) < 0){
			print_error("Event loop failed");
		}
		g_data->ping_count++;
		i++;
	}
}

int main(int ac, char **av)
{
	if (ac < 2){
		usage();
		return 1;
	}
	g_data = calloc(1, sizeof(t_data));
	g_data->ac = ac;
	g_data->av = av;
	
	// printf("PID: %d\n", getpid());
	atexit(free_data);
	init_data(g_data);
	init_signals();
	ping_loop(g_data);
	// free_data(&data);
	
	return 0;
}