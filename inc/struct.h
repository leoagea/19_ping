/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 14:14:02 by lagea             #+#    #+#             */
/*   Updated: 2025/06/24 13:36:45 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __STRUCT_H__
#define __STRUCT_H__

#include <stddef.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <stdbool.h>

#define MAX_PAYLOAD_SIZE   56
#define RECV_BUFFER_SIZE   2048
#define SIZEOF_ICMP_HEADER sizeof(struct icmp)

typedef struct timeval			t_timeval;
typedef struct sockaddr_storage t_sockaddr;

typedef struct s_options {
	bool	  help;			 // true if -h or --help was passed
	bool	  verbose;		 // true if -v or --verbose
	bool	  quiet;		 // true if -c or --count was passed
	bool	  flood;		 // true if -f or --flood was passed
	bool	  timeout;		 // true if -w or --timeout was passed
	char	**inputs;		 // remaining positional arguments
	int		  n_inputs;		 // count of inputs
	size_t	  ping_interval; // interval between pings in seconds
	t_timeval t_timeout;	 // timeout for each ping in seconds
} t_options;

typedef struct s_ping {
	int	   sockfd;
	int	   nfds;
	size_t ping_count;
	size_t ping_interval;
	size_t ping_timeout;

	int		ttl;
	double *rtt;

	bool	  is_valid;
	char	 *target_hostname;
	in_addr_t target_ip;

	struct {
		ssize_t bytes_read;
		ssize_t iph_len;
		char	recv_buffer[RECV_BUFFER_SIZE];
		char	addrstr[INET6_ADDRSTRLEN];

		ssize_t bytes_sent;
		char	send_buffer[MAX_PAYLOAD_SIZE + SIZEOF_ICMP_HEADER];
	} packet;

	t_sockaddr addr;

} t_ping;

typedef struct s_ping_stats {
	int	   packets_sent;
	int	   packets_received;
	int	   packets_lost;
	double min_rtt;
	double max_rtt;

	t_timeval next_ping_time;
	t_timeval last_ping_time;
} t_ping_stats;

typedef struct s_data {
	int	   ac;
	char **av;

	t_options	 *arg;
	t_ping		 *ping;
	t_ping_stats *stats;
	size_t		  ping_nb;
	size_t		  ping_index;
	size_t		  ping_count;
} t_data;

#endif