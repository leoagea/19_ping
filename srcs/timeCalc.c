/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timeCalc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 15:49:54 by lagea             #+#    #+#             */
/*   Updated: 2025/06/16 16:18:07 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

void rtt_calculate(t_ping *ping, t_ping_stats *stats, char *buf)
{
    struct timeval send_ts;
    struct timeval recv_ts;
    struct timeval result;
    
    double rtt;

    // Extract the send timestamp from the payload
    memcpy(&send_ts, buf, sizeof(send_ts));

    // Get the current time as the receive timestamp
    gettimeofday(&recv_ts, NULL);

    // Calculate RTT in seconds
    timeval_sub(&recv_ts, &send_ts, &result);
    rtt =  (result.tv_sec + result.tv_usec) / 1000000.0;

    stats->packets_received++;
    
    if (stats->packets_received == 1 || rtt < stats->min_rtt) {
        stats->min_rtt = rtt;
    }
    
    if (rtt > stats->max_rtt) {
        stats->max_rtt = rtt;
    }

    ping->rtt[ping->ping_count - 1] = rtt; 

    // printf("RTT: %.3f ms\n", rtt * 1000);
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

    return (ping->ping_count > 1) ? sqrt(sum / (PING_DEFAULT_COUNT)) * 1000 : 0.0;
}