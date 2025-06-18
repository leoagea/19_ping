/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 14:33:52 by lagea             #+#    #+#             */
/*   Updated: 2025/06/18 18:31:19 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

void freePointer(void **content)
{
	if (*content)
	{
		free(*content);
		*content = NULL;
	}
}

static void free_ping_struct(t_ping *ping, size_t size)
{
	size_t i = 0;
    
    if (!ping)
        return;
        
    while (i < size)
    {
        if (ping[i].target_hostname)
            freePointer((void **)&ping[i].target_hostname);
        if (ping[i].rtt)
            freePointer((void **)&ping[i].rtt);
        if (ping[i].sockfd >= 0){
            close(ping[i].sockfd);
            ping[i].sockfd = -1;
        }
        i++;
    }
    freePointer((void **)&ping);
}

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