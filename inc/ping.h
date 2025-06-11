/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 16:53:22 by lagea             #+#    #+#             */
/*   Updated: 2025/06/11 16:37:05 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PING_H__
# define __PING_H__

/*#############################################################################
# Includes
#############################################################################*/

#include "struct.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>  // close
#include <poll.h>   // poll
#include <time.h>   // time, time_t
#include <errno.h>   // errno, ENOPROTOOPT
#include <arpa/inet.h> // inet_pton
#include <netdb.h>    // gethostbyname
#include <string.h>  // strdup
#include <sys/time.h> // struct timeval
#include <sys/socket.h> // socket, setsockopt

#define PING_DEFAULT_COUNT 4
#define MAX(a, b) ((a) > (b) ? (a) : (b))
/*#############################################################################
# Init.c
#############################################################################*/

void	init_data(t_data *data);
void 	init_socket(t_ping *ping);

/*#############################################################################
# Event.c
#############################################################################*/

int event_loop(t_ping *ping, t_ping_stats *stats);

/*#############################################################################
# Utils.c
#############################################################################*/

void 	print_error(const char *msg);
void	usage(void);
void	checkTarget(t_data *data, t_ping *ping, const char *target);

/*#############################################################################
# Free.c
#############################################################################*/

void	freePointer(void **content);
void 	free_data(t_data *data);

/*#############################################################################
# Debug.c
#############################################################################*/

void	debug_print_all_ping_struct(t_data *data);


#endif