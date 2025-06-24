/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 16:53:22 by lagea             #+#    #+#             */
/*   Updated: 2025/06/24 14:51:24 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PING_H__
#define __PING_H__

/*#############################################################################
# Includes
#############################################################################*/

#include "struct.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>			 // sigaction
#include <unistd.h>			 // close
#include <time.h>			 // time, time_t
#include <math.h>			 // fmin, fmax, sqrt
#include <errno.h>			 // errno, ENOPROTOOPT
#include <arpa/inet.h>		 // inet_pton
#include <string.h>			 // strdup
#include <sys/time.h>		 // struct timeval
#include <sys/socket.h>		 // socket, setsockopt
#include <netinet/ip_icmp.h> // ICMP protocol definitions
#include <getopt.h>			 // getopt_long
#include <fcntl.h>			 // fcntl
#include <netdb.h>			 // getaddrinfo, gai_strerror

/*#############################################################################
# Define Variables
#############################################################################*/

#define PING_DEFAULT_COUNT	  10000
#define PING_DEFAULT_INTERVAL 1
#define PING_DEFAULT_TIMEOUT  1
#define MAX_PAYLOAD_SIZE	  56
#define RECV_BUFFER_SIZE	  2048
#define IP_HEADER_SIZE		  20
#define ICMP4_HEADER_SIZE	  sizeof(struct icmphdr)
#define BUF_LEN				  256
#define HELP_BUF_LEN		  1024
#define _(fd, msg)			  write(fd, msg, strlen(msg));

/*#############################################################################
# Typedef Variables
#############################################################################*/

typedef struct icmphdr t_icmphdr;
typedef struct iphdr   t_iphdr;

/*#############################################################################
# Global Variables
#############################################################################*/

extern t_data		   *g_data;
extern struct sigaction g_sigact;

/*#############################################################################
# Init.c
#############################################################################*/

void init_data(t_data *data);
void init_options(t_options *options);
void init_signals(void);
void init_socket(t_ping *ping);

/*#############################################################################
# Option.c
#############################################################################*/

void parse_arg(t_data *data);

/*#############################################################################
# Resolve.c
#############################################################################*/

void checkTarget(t_ping *ping, const char *target);

/*#############################################################################
# Event.c
#############################################################################*/

int event_loop(t_ping *ping, t_ping_stats *stats);

/*#############################################################################
# Request.c
#############################################################################*/

void build_echo_request(char *buf, size_t payload_len, int count);
int	 handle_echo_reply(t_ping *ping, t_ping_stats *stats, char *buf);

/*#############################################################################
# TimeCalc.c
#############################################################################*/

void   rtt_calculate(t_ping *ping, t_ping_stats *stats, char *buf);
double rtt_avg_calculate(t_ping *ping);
double stddev_calculate(t_ping *ping, double average);

/*#############################################################################
# Print.c
#############################################################################*/

void print_ping_info(t_ping *ping);
void print_global_stats(t_ping *ping, t_ping_stats *stats);
void print_ping_stats(t_ping *ping);
void print_ttl_exceeded(t_ping *ping, char *ip_add);

/*#############################################################################
# Verbose.c
#############################################################################*/

void fill_IP_header_dump(t_iphdr *ip_hdr, char *buf, size_t *len, const unsigned char *packet);
void fill_IP_header(t_iphdr *ip_hdr, char *buf, size_t *len);
void fill_ICMP_header(t_icmphdr *inner_header, t_icmphdr *outer_header, char *buf, size_t *len,
					  size_t bytes);

/*#############################################################################
# Utils.c
#############################################################################*/

void	 print_error(const char *msg);
void	 usage(void);
void	 timeval_add(const struct timeval *a, const struct timeval *b, struct timeval *result);
void	 timeval_sub(const struct timeval *a, const struct timeval *b, struct timeval *result);
int		 timeval_cmp(const struct timeval *a, const struct timeval *b);
uint16_t checksum(void *buf, size_t len);
void	 signal_handler(int signum);
void	 help(void);
void	 exit_failure(const char *msg);

/*#############################################################################
# Helper.c
#############################################################################*/

t_icmphdr *get_inner_icmp_header(char *buf);
t_icmphdr *get_outer_icmp_header(char *buf);
t_iphdr	  *get_outer_ip_header(char *buf);
char	  *get_ip_string(const t_sockaddr *addr);

/*#############################################################################
# Free.c
#############################################################################*/

void freePointer(void **content);
void free_data(void);

/*#############################################################################
# Debug.c
#############################################################################*/

void debug_print_all_ping_struct(t_data *data);
void debug_print_stats(t_ping_stats *stats);

#endif