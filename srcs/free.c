/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 14:33:52 by lagea             #+#    #+#             */
/*   Updated: 2025/06/03 16:26:02 by lagea            ###   ########.fr       */
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
        {
            free(ping[i].target_hostname);
            ping[i].target_hostname = NULL;
        }
        i++;
    }
    freePointer((void **)&ping);
}

void free_data(t_data *data)
{
	free_ping_struct(data->ping, data->ping_size);
	freePointer((void **)&data->stats);
}