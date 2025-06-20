/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   option.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 16:04:42 by lagea             #+#    #+#             */
/*   Updated: 2025/06/20 14:43:21 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

void parse_arg(t_data *data)
{
	t_options *opt = calloc(1, sizeof(t_options));
	init_options(opt);

	const char *short_opts = "hvc:";
	const struct option long_opts[] = {
		{ "help",    no_argument,       NULL, 'h' },
		{ "verbose", no_argument,       NULL, 'v' },
		{ "count",	 required_argument,	NULL, 'c' },
		{ NULL,      0,                 NULL,  0  }
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
				if (optarg) {
					opt->ping_count = atoi(optarg);
					if (opt->ping_count <= 0) {
						char buf[256];
						snprintf(buf, sizeof(buf), "ping: invalid count value '%s'\n", optarg);
						_(STDERR_FILENO, buf);
						exit(EXIT_FAILURE);
					}
				} else {
					_(STDERR_FILENO, "ping: option requires an argument -- 'c'\n");
					exit(EXIT_FAILURE);
				}
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

	if (opt->ping_count != 0)
		data->ping_count = opt->ping_count;
	 data->arg = opt;
}