/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 14:14:02 by lagea             #+#    #+#             */
/*   Updated: 2025/06/03 14:48:53 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __STRUCT_H__
# define __STRUCT_H__

#include <stddef.h>

typedef struct s_ping
{
	int		sockfd;
	int		ping_count;
	int		ping_interval;
	int		ping_timeout;
	char	*target_ip;
}	t_ping;

typedef struct s_ping_stats
{
	int		packets_sent;
	int		packets_received;
	int		packets_lost;
	double	min_rtt;
	double	max_rtt;
	double	avg_rtt;
}	t_ping_stats;

typedef struct s_data
{
	t_ping			*ping;
	t_ping_stats	*stats;
	size_t			ping_size;
}	t_data;

#endif