/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 14:16:40 by lagea             #+#    #+#             */
/*   Updated: 2025/06/24 14:53:27 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

/**
 * Prints an error message to the standard error output.
 * This function formats the error message with a prefix "ping: "
 * and writes it to the standard error file descriptor.
 * @param msg The error message to be printed.
 * @return None
 */
void print_error(const char *msg)
{
	char buf[1024] = {0};

	snprintf(buf, sizeof(buf), "ping: %s", msg);
	_(STDERR_FILENO, buf);
}

/**
 * Prints the usage information for the ping command.
 * @param None
 * @return None
 */
void usage(void)
{
	print_error("ping: missing host operand");
	print_error("Try 'ping --help' for more information.");
}


/**
 * Adds two timeval structures and stores the result in a third timeval structure.
 * This function handles the addition of seconds and microseconds,
 * ensuring that the microseconds do not exceed 1,000,000.
 * If the result exceeds 1,000,000 microseconds, it increments the seconds accordingly
 * and adjusts the microseconds.
 * @param a      Pointer to the first timeval structure.
 * @param b      Pointer to the second timeval structure.
 * @param result Pointer to the timeval structure where the result will be stored.
 * @return None
 */
void timeval_add(const struct timeval *a, const struct timeval *b, struct timeval *result)
{
	if (!a || !b || !result)
		return;

	result->tv_sec = a->tv_sec + b->tv_sec;
	result->tv_usec = a->tv_usec + b->tv_usec;
	if (result->tv_usec >= 1000000) {
		result->tv_sec++;
		result->tv_usec -= 1000000;
	}
}

/**
 * Subtracts two timeval structures and stores the result in a third timeval structure.
 * This function handles the subtraction of seconds and microseconds,
 * ensuring that the microseconds do not become negative.
 * If the result's microseconds are negative, it decrements the seconds
 * and adjusts the microseconds accordingly.
 * @param a      Pointer to the first timeval structure.
 * @param b      Pointer to the second timeval structure.
 * @param result Pointer to the timeval structure where the result will be stored.
 * @return None
 */
void timeval_sub(const struct timeval *a, const struct timeval *b, struct timeval *result)
{
	if (!a || !b || !result)
		return;

	result->tv_sec = a->tv_sec - b->tv_sec;
	result->tv_usec = a->tv_usec - b->tv_usec;
	if (result->tv_usec < 0) {
		result->tv_sec--;
		result->tv_usec += 1000000;
	}
}

/**
 * Compares two timeval structures.
 * This function compares the seconds and microseconds of the two timeval structures.
 * It returns:
 * -1 if the first timeval is less than the second,
 * 1 if the first timeval is greater than the second,
 * 0 if they are equal.
 * @param a Pointer to the first timeval structure.
 * @param b Pointer to the second timeval structure.
 * @return An integer indicating the comparison result:
 */
int timeval_cmp(const struct timeval *a, const struct timeval *b)
{
	if (!a || !b)
		return 0;

	if (a->tv_sec < b->tv_sec)
		return -1;
	if (a->tv_sec > b->tv_sec)
		return 1;
	if (a->tv_usec < b->tv_usec)
		return -1;
	if (a->tv_usec > b->tv_usec)
		return 1;
	return 0;
}

/**
 * Calculates the checksum for a buffer.
 * This function computes the checksum for a given buffer
 * by summing up the 16-bit words in the buffer.
 * If the buffer length is odd, it pads the last byte with zero.
 * It then folds any carries from the top 16 bits into the lower 16 bits
 * and returns the one's-complement of the sum as a 16-bit value.
 * @param buf Pointer to the buffer containing the data.
 * @param len Length of the buffer in bytes.
 * @return The calculated checksum as a 16-bit unsigned integer.
 */
uint16_t checksum(void *buf, size_t len)
{
	uint32_t  sum = 0;
	uint16_t *data = buf;

	// Sum up 16-bit words
	while (len > 1) {
		sum += *data++;
		len -= 2;
	}

	// If there's a leftover byte, pad with zero to form a 16-bit word
	if (len == 1) {
		uint8_t tail[2] = {*(uint8_t *)data, 0};
		sum += *(uint16_t *)tail;
	}

	// Fold any carries from the top 16 bits into the lower 16 bits
	while (sum >> 16) {
		sum = (sum & 0xFFFF) + (sum >> 16);
	}

	// One's-complement and return
	return (uint16_t)~sum;
}

/**
 * Signal handler for SIGINT.
 * This function is called when the program receives a SIGINT signal (usually from Ctrl+C).
 * It prints the global statistics for the current ping session
 * and exits the program gracefully.
 * @param sig The signal number received (in this case, SIGINT).
 * @return None
 */
void signal_handler(int sig)
{
	if (sig == SIGINT) {
		t_ping		 *ping = &g_data->ping[g_data->ping_index];
		t_ping_stats *stats = &g_data->stats[g_data->ping_index];
		print_global_stats(ping, stats);
		exit(EXIT_SUCCESS);
	}
}

/**
 * Displays the help message for the ping command.
 * This function prints the usage information,
 * including the command syntax and available options.
 * It provides a brief description of each option,
 * such as enabling verbose output or showing help.
 * This function is called when the user requests help
 * or when the command is run with the `-h` or `--help` option
 * to provide guidance on how to use the ping command.
 * @param None
 * @return None
 */
void help(void)
{
    size_t len = 0;
    char buf[HELP_BUF_LEN] = {0};

	len = snprintf(buf, HELP_BUF_LEN, "Usage: ping [options] <host>\n");
	len += snprintf(buf + len, HELP_BUF_LEN - len, "Options:\n");
	len += snprintf(buf + len, HELP_BUF_LEN - len, "  -h, --help        Show this help message and exit\n");
	len += snprintf(buf + len, HELP_BUF_LEN - len, "  -v, --verbose     Enable verbose output\n");

	len += snprintf(buf + len, HELP_BUF_LEN - len, "  -c, --count <n>   Stop after sending <n> packets\n");
	len += snprintf(buf + len, HELP_BUF_LEN - len, "  -i, --interval <n> Set interval between packets to <n> seconds\n");
	len += snprintf(buf + len, HELP_BUF_LEN - len, "  -q, --quiet       Suppress output except for errors\n");
	len += snprintf(buf + len, HELP_BUF_LEN - len, "  -f, --flood       Flood ping (send packets as fast as possible)\n");
	len += snprintf(buf + len, HELP_BUF_LEN - len, "  -w, --timeout <n> Set timeout for each packet to <n> seconds\n");
	len += snprintf(buf + len, HELP_BUF_LEN - len, "\n");

	_(STDOUT_FILENO, buf);
}

/**
 * Exits the program with a failure status and prints an error message.
 * This function formats the error message with a prefix "ping: "
 * and writes it to the standard error file descriptor.
 * It then terminates the program with an exit status of EXIT_FAILURE.
 * @param msg The error message to be printed before exiting.
 * @return None
 */
void exit_failure(const char *msg)
{
	_(STDERR_FILENO, msg);
	exit(EXIT_FAILURE);
}
