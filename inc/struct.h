/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 14:14:02 by lagea             #+#    #+#             */
/*   Updated: 2025/06/12 16:48:55 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __STRUCT_H__
# define __STRUCT_H__

#include <stddef.h>
#include <netinet/in.h>
#include <stdbool.h>

typedef struct timeval t_timeval;

typedef struct s_ping
{
	int		sockfd;
	int  	nfds;
	int		ping_count;
	int		ping_interval;
	int		ping_timeout;
	
	bool	is_valid;
	char 	*target_hostname;
	in_addr_t	target_ip;
}	t_ping;

typedef struct s_ping_stats
{
	int		packets_sent;
	int		packets_received;
	int		packets_lost;
	double	min_rtt;
	double	max_rtt;
	double	avg_rtt;

	t_timeval 	next_ping_time;
	t_timeval	last_ping_time;
}	t_ping_stats;

typedef struct s_data
{
	int				ac;
	char			**av;
	
	t_ping			*ping;
	t_ping_stats	*stats;
	size_t			ping_size;
}	t_data;

#endif