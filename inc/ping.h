/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 16:53:22 by lagea             #+#    #+#             */
/*   Updated: 2025/06/13 15:50:37 by lagea            ###   ########.fr       */
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
#include <signal.h>  // sigaction
#include <unistd.h>  // close
#include <poll.h>   // poll
#include <time.h>   // time, time_t
#include <errno.h>   // errno, ENOPROTOOPT
#include <arpa/inet.h> // inet_pton
#include <netdb.h>    // gethostbyname
#include <string.h>  // strdup
#include <sys/time.h> // struct timeval
#include <sys/socket.h> // socket, setsockopt
#include <netinet/ip_icmp.h> // ICMP protocol definitions

#define PING_DEFAULT_COUNT 4
#define MAX_PAYLOAD_SIZE 56
#define RECV_BUFFER_SIZE 2048

/*#############################################################################
# Global Variables
#############################################################################*/

extern t_data *g_data;
extern struct sigaction g_sigact;

/*#############################################################################
# Init.c
#############################################################################*/

void	init_data(t_data *data);
void 	init_signals(void);
void 	init_socket(t_ping *ping);

/*#############################################################################
# Event.c
#############################################################################*/

int		event_loop(t_ping *ping, t_ping_stats *stats);

/*#############################################################################
# Request.c
#############################################################################*/

void	build_echo_request(char *buf, size_t payload_len, int count);
int 	check_response_header(char *buf, int count);
void	rtt_calculate(t_ping_stats *stats, char *buf);

/*#############################################################################
# Utils.c
#############################################################################*/

void 	print_error(const char *msg);
void	usage(void);
void	checkTarget(t_ping *ping, const char *target);
void	timeval_add(const struct timeval *a, const struct timeval *b, struct timeval *result);
void	timeval_sub(const struct timeval *a, const struct timeval *b, struct timeval *result);
int 	timeval_cmp(const struct timeval *a, const struct timeval *b);
uint16_t checksum(void *buf, size_t len);
void 	print_result(t_ping *ping, t_ping_stats *stats);

/*#############################################################################
# Free.c
#############################################################################*/

void	freePointer(void **content);
void 	free_data(void);

/*#############################################################################
# Debug.c
#############################################################################*/

void	debug_print_all_ping_struct(t_data *data);


#endif