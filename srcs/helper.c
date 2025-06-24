/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 13:32:42 by lagea             #+#    #+#             */
/*   Updated: 2025/06/24 13:43:02 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

/**
 * Retrieves the outer ICMP header from the given buffer.
 * This function assumes that the buffer contains an IPv4 packet
 * with an ICMP Time-Exceeded message.
 * It skips the outer IP header and the outer ICMP header,
 * returning a pointer to the inner ICMP header.
 * @param buf Pointer to the buffer containing the packet data.
 * @return A pointer to the inner ICMP header structure.
 */
t_icmphdr *get_inner_icmp_header(char *buf)
{
	/* 1. Skip the OUTER IPv4 header */
	struct iphdr *ip_outer = (struct iphdr *)buf;
	size_t		  outer_ip_len = ip_outer->ihl * 4;

	uint8_t *pointer = (uint8_t *)buf + outer_ip_len;

	/* 2. Skip the OUTER ICMP Time-Exceeded header (always 8 bytes) */
	pointer += 8;

	/* 3. p now points at the QUOTED inner IPv4 header */
	struct iphdr *ip_inner = (struct iphdr *)pointer;
	size_t		  inner_ip_len = ip_inner->ihl * 4;
	pointer += inner_ip_len;

	/* 4. p now points at the QUOTED inner ICMP Echo header */
	struct icmphdr *icmp_inner = (struct icmphdr *)pointer;

	return icmp_inner;
}

/**
 * Retrieves the outer ICMP header from the given buffer.
 * This function assumes that the buffer contains an IPv4 packet
 * with an ICMP Time-Exceeded message.
 * It skips the outer IP header and the outer ICMP header,
 * returning a pointer to the inner ICMP header.
 * @param buf Pointer to the buffer containing the packet data.
 * @return A pointer to the outer ICMP header structure.
 */
t_icmphdr *get_outer_icmp_header(char *buf)
{
	/* 1. Skip the OUTER IPv4 header */
	struct iphdr *ip_outer = (struct iphdr *)buf;
	size_t		  outer_ip_len = ip_outer->ihl * 4;

	uint8_t *pointer = (uint8_t *)buf + outer_ip_len;

	/* 2. Skip the OUTER ICMP Time-Exceeded header (always 8 bytes) */
	struct icmphdr *icmp_outer = (struct icmphdr *)pointer;

	return icmp_outer;
}

/**
 * Retrieves the outer IP header from the given buffer.
 * This function assumes that the buffer contains an IPv4 packet.
 * It casts the buffer to a pointer of type t_iphdr and returns it.
 * @param buf Pointer to the buffer containing the packet data.
 * @return A pointer to the outer IP header structure.
 */
t_iphdr *get_outer_ip_header(char *buf)
{
	return (struct iphdr *)buf;
}

/**
 * Converts a sockaddr structure to a human-readable IP address string.
 * This function assumes the address is an IPv4 address.
 * It uses inet_ntop to convert the address to a string format.
 * @param addr Pointer to the sockaddr structure containing the IP address.
 * @return A static string containing the IP address in dotted-decimal format.
 */
char *get_ip_string(const t_sockaddr *addr)
{
	static char ip_str[INET6_ADDRSTRLEN];

	struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
	inet_ntop(AF_INET, &addr4->sin_addr, ip_str, INET_ADDRSTRLEN);

	return ip_str;
}