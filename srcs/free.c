/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 14:33:52 by lagea             #+#    #+#             */
/*   Updated: 2025/06/24 14:42:55 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

/**
 * Helper function to free a pointer and set it to NULL.
 * This prevents double free errors and dangling pointers.
 * @param content Pointer to the pointer to be freed.
 * @return None
 */
void freePointer(void **content)
{
	if (*content) {
		free(*content);
		*content = NULL;
	}
}

/**
 * Frees the memory allocated for the ping structures.
 * It iterates through each ping structure, freeing its components
 * and closing the socket file descriptors.
 * @param ping Pointer to the array of t_ping structures.
 * @param size Number of ping structures to free.
 * @return None
 */
static void free_ping_struct(t_ping *ping, size_t size)
{
	size_t i = 0;

	if (!ping)
		return;

	while (i < size) {
		if (ping[i].target_hostname)
			freePointer((void **)&ping[i].target_hostname);
		if (ping[i].rtt)
			freePointer((void **)&ping[i].rtt);
		if (ping[i].sockfd >= 0) {
			close(ping[i].sockfd);
			ping[i].sockfd = -1;
		}
		i++;
	}
	freePointer((void **)&ping);
}

/**
 * Frees the global data structure and its components.
 * It checks if the global data pointer is not NULL,
 * then frees the ping structures, statistics, and options.
 * @param None
 * @return None
 */
void free_data(void)
{
	if (!g_data)
		return;

	if (g_data->ping)
		free_ping_struct(g_data->ping, g_data->ping_nb);
	if (g_data->stats)
		freePointer((void **)&g_data->stats);
	if (g_data->arg)
		freePointer((void **)&g_data->arg);
	freePointer((void **)&g_data);
}