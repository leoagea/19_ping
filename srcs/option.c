/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   option.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 16:04:42 by lagea             #+#    #+#             */
/*   Updated: 2025/06/26 14:50:07 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

/**
 * Parses the option argument for count, interval, or timeout.
 * This function checks if the provided option argument
 * is valid and sets the corresponding
 * field in the options structure.
 * If the option argument is invalid,
 * it prints an error message and exits the program.
 * @param opt Pointer to the t_options structure.
 * @param optarg The option argument to be parsed.
 * @param option_name The name of the option being parsed (e.g., "count", "interval", "timeout").
 * @return None
 */
static void parse_optarg(t_options *opt, const char *optarg, const char *option_name)
{
	char buf[256] = {0};

	if (optarg) {
		if (strcmp(option_name, "count") == 0) {
			int count = atoi(optarg);
			if (count <= 0) {
				snprintf(buf, sizeof(buf), "ping: invalid count value '%s'\n", optarg);
				exit_failure(buf);
			}
			g_data->ping_count = count;
		} else if (strcmp(option_name, "interval") == 0) {
			opt->ping_interval = atoi(optarg);
			if (opt->ping_interval <= 0) {
				snprintf(buf, sizeof(buf), "ping: invalid interval value '%s'\n", optarg);
				exit_failure(buf);
			}
		} else if (strcmp(option_name, "timeout") == 0) {
			opt->t_timeout.tv_sec = atoi(optarg);
			if (opt->t_timeout.tv_sec <= 0) {
				snprintf(buf, sizeof(buf), "ping: invalid timeout value '%s'\n", optarg);
				exit_failure(buf);
			}
			opt->timeout = true;
		} else {
			snprintf(buf, sizeof(buf), "ping: unknown option '%s'\n", option_name);
			exit_failure(buf);
		}
	} else {
		snprintf(buf, sizeof(buf), "ping: option requires an argument -- '%s'\n", option_name);
		exit_failure(buf);
	}
}

/**
 * Parses command-line arguments for the ping program.
 * This function initializes the options structure and processes
 * the command-line arguments using getopt_long.
 * It sets various options such as verbose mode,
 * count of pings, interval between pings, quiet mode,
 * flood mode, and timeout.
 * If the help option is specified, it displays the help message
 * and exits the program.
 * @param data Pointer to the t_data structure containing command-line arguments.
 * @return None
 */
void parse_arg(t_data *data)
{
	t_options *opt = calloc(1, sizeof(t_options));
	if (!opt) {
		print_error("Memory allocation failed for options structure.");
		exit(EXIT_FAILURE);
	}

	init_options(opt);

	const char		   *short_opts = "h?vc:i:qfw:";
	const struct option long_opts[] = {
		{"help", no_argument, NULL, 'h'},
		{"verbose", no_argument, NULL, 'v'},
		{"count", required_argument, NULL, 'c'},
		{"interval", required_argument, NULL, 'i'},
		{"quiet", no_argument, NULL, 'q'},
		{"flood", required_argument, NULL, 'f'},
		{"timeout", required_argument, NULL, 'w'},
		{NULL, 0, NULL, 0}};

	int option;
	while ((option = getopt_long(data->ac, data->av, short_opts, long_opts, NULL)) != -1) {
		switch (option) {
			case 'h':
				help();
				exit(EXIT_SUCCESS);
			
			case 'v':
				opt->verbose = true;
				break;
				
			case 'c':
				parse_optarg(opt, optarg, "count");
				break;
				
			case 'i':
				parse_optarg(opt, optarg, "interval");
				break;
				
			case 'q':
				opt->quiet = true;
				break;
				
			case 'f':
				opt->quiet = true;
				opt->flood = true;
				break;
				
			case 'w':
				parse_optarg(opt, optarg, "timeout");
				break;
				
			case '?':
				if (optopt == 0 || optopt == '?') {
					help();
					exit(EXIT_SUCCESS);
				}
				else {
					char buf[256];
					snprintf(buf, sizeof(buf), "Try 'ping --help' for more information.\n");
					exit_failure(buf);
				}
				break;

			default:
				_(STDERR_FILENO, "Try 'ping --help' for more information.\n");
				exit(EXIT_FAILURE);
		}
	}

	data->ping_nb = data->ac - optind;
	opt->n_inputs = data->ping_nb;
	if (opt->n_inputs > 0)
		opt->inputs = &data->av[optind];

	data->arg = opt;
}