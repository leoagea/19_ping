/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timeCalc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 15:49:54 by lagea             #+#    #+#             */
/*   Updated: 2025/06/24 14:40:40 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

/**
 * Calculates the round-trip time (RTT) for a ping.
 * This function takes the send timestamp from the buffer,
 * gets the current time,
 * and calculates the difference
 * between the two timestamps.
 * It updates the minimum and maximum RTT values
 * in the statistics structure.
 * The RTT is stored in the ping structure's RTT array.
 * @param ping   Pointer to the t_ping structure containing ping settings.
 * @param stats  Pointer to the t_ping_stats structure to store statistics.
 * @param buf    The buffer containing the send timestamp.
 * @return None
 */
void rtt_calculate(t_ping *ping, t_ping_stats *stats, char *buf)
{
	struct timeval send_ts;
	struct timeval now;
	struct timeval result;

	double rtt;

	memcpy(&send_ts, buf, sizeof(send_ts));

	gettimeofday(&now, NULL);

	timeval_sub(&now, &send_ts, &result);
	rtt = result.tv_sec * 1000.0 + result.tv_usec / 1000.0;

	if (stats->packets_received == 0) {
		stats->min_rtt = rtt;
		stats->max_rtt = rtt;
	} else {
		stats->min_rtt = fmin(stats->min_rtt, rtt);
		stats->max_rtt = fmax(stats->max_rtt, rtt);
	}

	ping->rtt[ping->ping_count - 1] = rtt;
}

/**
 * Calculates the average round-trip time (RTT) from the RTT array.
 * This function sums up all valid RTT values and divides by the count
 * of valid RTTs to get the average.
 * If no valid RTTs are found, it returns 0.0.
 * @param ping Pointer to the t_ping structure containing ping settings.
 * @return The average RTT as a double.
 */
double rtt_avg_calculate(t_ping *ping)
{
	double total_rtt = 0.0;

	for (size_t i = 0; i < ping->ping_count; i++) {
		if (ping->rtt[i] >= 0) {
			total_rtt += ping->rtt[i];
		}
	}

	return (ping->ping_count > 0) ? (total_rtt / ping->ping_count) : 0.0;
}

/**
 * Calculates the standard deviation of the round-trip times (RTT).
 * This function computes the sum of the squared differences
 * between each RTT and the average RTT,
 * then divides by the count of RTTs to get the variance,
 * and finally takes the square root to get the standard deviation.
 * If there is only one RTT, it returns 0.0.
 * @param ping    Pointer to the t_ping structure containing ping settings.
 * @param average The average RTT calculated from the RTT array.
 * @return The standard deviation of the RTTs as a double.
 */
double stddev_calculate(t_ping *ping, double average)
{
	double sum = 0.0;

	for (size_t i = 0; i < ping->ping_count; i++) {
		if (ping->rtt[i] >= 0) {
			double diff = ping->rtt[i] - average;
			sum += diff * diff;
		}
	}

	return (ping->ping_count > 1) ? sqrt(sum / (ping->ping_count)) : 0.0;
}