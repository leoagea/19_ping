/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 16:53:36 by lagea             #+#    #+#             */
/*   Updated: 2025/06/24 14:42:09 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

t_data			*g_data = NULL;
struct sigaction g_sigact;

/**
 * Initializes the ping loop, iterating through each ping target.
 * For each target, it checks if the target is valid, initializes the socket,
 * and starts the event loop for sending and receiving pings.
 * The loop continues until all specified ping targets have been processed.
 * @param data Pointer to the global data structure containing ping targets.
 * @return None
 */
static void ping_loop(t_data *data)
{
	size_t i = 0;
	while (i < data->ping_nb) {
		checkTarget(&g_data->ping[i], g_data->arg->inputs[i]);
		init_socket(&g_data->ping[i]);
		if (event_loop(&g_data->ping[i], &g_data->stats[i]) < 0)
			return;
		g_data->ping_index++;
		i++;
	}
}

/**
 * Checks if the program is run with root privileges.
 * This function checks the user ID of the process.
 * If the user ID is not 0 (root), it prints an error message
 * and suggests running the program with sudo.
 * @return true if the program is run with root privileges, false otherwise.  
*/
static bool check_root_privileges(void)
{
	if (getuid() != 0) {
		print_error("This program requires root privileges.\n");
		print_error("Please run with sudo: sudo ./ft_ping <destination>\n");
		return false;
	}
	return true;
}

int main(int ac, char **av)
{
	if (ac < 2)
		return usage(), EXIT_FAILURE;

	if (!check_root_privileges())
		return EXIT_FAILURE;

	g_data = calloc(1, sizeof(t_data));
	if (!g_data) {
		print_error("Memory allocation failed for global data.");
		return EXIT_FAILURE;
	}

	g_data->ac = ac;
	g_data->av = av;

	atexit(free_data);

	parse_arg(g_data);
	init_data(g_data);
	init_signals();
	ping_loop(g_data);

	return EXIT_SUCCESS;
}