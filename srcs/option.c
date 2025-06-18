/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   option.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 16:04:42 by lagea             #+#    #+#             */
/*   Updated: 2025/06/18 18:31:57 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

void parse_arg(t_data *data)
{
	t_options *opt = calloc(1, sizeof(t_options));
	init_options(opt);

	const char *short_opts = "h?v";
	const struct option long_opts[] = {
		{ "help",    no_argument,       NULL, 'h' },
		{ "verbose", no_argument,       NULL, 'v' },
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
			case '?':
				exit(EXIT_FAILURE);
			default:
				print_error("ping: unexpected error in option parsing");
				exit(EXIT_FAILURE);
		}
	}
	
	data->ping_nb = data->ac - optind;
	opt->n_inputs = data->ping_nb;
	if (opt->n_inputs > 0)
        opt->inputs = &data->av[optind];

	 data->arg = opt;
}