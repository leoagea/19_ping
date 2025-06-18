/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 14:14:02 by lagea             #+#    #+#             */
/*   Updated: 2025/06/18 16:29:31 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __STRUCT_H__
# define __STRUCT_H__

#include <stddef.h>
#include <netinet/in.h>
#include <stdbool.h>

typedef struct timeval t_timeval;

typedef struct s_options {
    bool      help;          // true if -h or --help was passed
    bool      verbose;       // true if -v or --verbose
    char    **inputs;        // remaining positional arguments
    int       n_inputs;      // count of inputs
} t_options;

typedef struct s_ping {
	int		sockfd;
	int  	nfds;
	int		ping_count;
	int		ping_interval;
	int		ping_timeout;
	double	*rtt;

	bool	is_valid;
	char 	*target_hostname;
	in_addr_t	target_ip;
}	t_ping;

typedef struct s_ping_stats {
	int		packets_sent;
	int		packets_received;
	int		packets_lost;
	double	min_rtt;
	double	max_rtt;

	t_timeval 	next_ping_time;
	t_timeval	last_ping_time;
}	t_ping_stats;

typedef struct s_data {
	int				ac;
	char			**av;
	
	t_options  		*arg;
	t_ping			*ping;
	t_ping_stats	*stats;
	size_t			ping_nb;
	size_t			ping_count;
}	t_data;

#endif