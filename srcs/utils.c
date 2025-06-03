/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 14:16:40 by lagea             #+#    #+#             */
/*   Updated: 2025/06/03 14:23:07 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

void print_error(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
}

void usage(void)
{
	print_error("ping: missing host operand");
	print_error("Try 'ping --help' for more information.");
}