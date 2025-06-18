/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timeCalc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 15:49:54 by lagea             #+#    #+#             */
/*   Updated: 2025/06/18 15:06:26 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

void rtt_calculate(t_ping *ping, t_ping_stats *stats, char *buf)
{
    struct timeval send_ts;
    struct timeval now;
    struct timeval result;
    
    double rtt;

    memcpy(&send_ts, buf, sizeof(send_ts));

    gettimeofday(&now, NULL);

    timeval_sub(&now, &send_ts, &result);
    rtt =  result.tv_sec * 1000.0 + result.tv_usec / 1000.0;
    
    if (stats->packets_received == 0) {
        stats->min_rtt = rtt;
        stats->max_rtt = rtt;
    }else {
        stats->min_rtt = fmin(stats->min_rtt, rtt);
        stats->max_rtt = fmax(stats->max_rtt, rtt);
    }

    ping->rtt[ping->ping_count - 1] = rtt; 
}

double rtt_avg_calculate(t_ping *ping)
{
    double total_rtt = 0.0;

    for (int i = 0; i < PING_DEFAULT_COUNT; i++) {
        if (ping->rtt[i] >= 0) {
            total_rtt += ping->rtt[i];
        }
    }

    return (ping->ping_count > 0) ? (total_rtt / ping->ping_count) : 0.0;
}

double stddev_calculate(t_ping *ping, double average)
{
    double sum = 0.0;

    for (size_t i = 0; i < PING_DEFAULT_COUNT; i++) {
        if (ping->rtt[i] >= 0) {
            double diff = ping->rtt[i] - average;
            sum += diff * diff;
        }
    }

    return (ping->ping_count > 1) ? sqrt(sum / (PING_DEFAULT_COUNT)) : 0.0;
}