/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 14:33:52 by lagea             #+#    #+#             */
/*   Updated: 2025/06/03 14:59:16 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

static void freePointer(void **content)
{
	if (*content)
	{
		free(*content);
		*content = NULL;
	}
}

void free_data(t_data *data)
{
	freePointer((void **)&data->ping);
	freePointer((void **)&data->stats);
}