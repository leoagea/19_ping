/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   option.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 16:04:42 by lagea             #+#    #+#             */
/*   Updated: 2025/06/20 18:42:47 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

static void parse_optarg(t_options *opt, const char *optarg, const char *option_name)
{
	char buf[256];
	
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
		}
		else if (strcmp(option_name, "timeout") == 0) {
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

void parse_arg(t_data *data)
{
	t_options *opt = calloc(1, sizeof(t_options));
	init_options(opt);

	const char *short_opts = "hvc:i:qfw:";
	const struct option long_opts[] = {
		{ "help",     no_argument,       NULL, 'h' },
		{ "verbose",  no_argument,       NULL, 'v' },
		{ "count",	  required_argument, NULL, 'c' },
		{ "interval", required_argument, NULL, 'i' },
		{ "quiet",    no_argument,       NULL, 'q' },
		{ "flood",    required_argument, NULL, 'f' },
		{ "timeout",  required_argument, NULL, 'w' },
		{ NULL,       0,                 NULL,  0  }
    };
	
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
				opt->flood = true;
				break;

			case 'w':
				parse_optarg(opt, optarg, "timeout");
				break;

			case '?':
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