/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   verbose.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 16:26:50 by lagea             #+#    #+#             */
/*   Updated: 2025/06/24 14:39:56 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

/**
 * Fills the buffer with a dump of the IP header.
 * This function formats the IP header fields into a human-readable format,
 * including version, IHL, TOS, total length, ID, flags, fragment offset
 * TTL, protocol, checksum, source address, and destination address.
 * It also includes a hex dump of the IP header data.
 * @param ip_hdr Pointer to the IP header structure.
 * @param buf Pointer to the buffer to fill with the IP header information.
 * @param len Pointer to the current length of the buffer.
 * @param packet Pointer to the raw packet data containing the IP header.
 * @return None
 */
void fill_IP_header_dump(t_iphdr *ip_hdr, char *buf, size_t *len, const unsigned char *packet)
{
	size_t ihl_bytes = ip_hdr->ihl * 4;

	*len += snprintf(buf + *len, sizeof(buf) - *len, "IP Hdr Dump:\n ");

	/* 2. Hex pairs with spacing/newlines like inetutils ping */
	for (size_t i = 0; i < ihl_bytes; ++i) {
		int byte = packet[i];
		if (byte)
			*len += snprintf(buf + *len, sizeof(buf) - *len, "%02x", packet[i]);

		if ((i & 1) == 1)
			*len += snprintf(buf + *len, sizeof(buf) - *len, " ");
	}

	*len += snprintf(buf + *len, sizeof(buf) - *len, "\n");
}

/**
 * Fills the buffer with a formatted dump of the IP header.
 * This function extracts relevant fields from the IP header,
 * formats them into a human-readable string,
 * and appends it to the buffer.
 * @param ip_hdr Pointer to the IP header structure.
 * @param buf Pointer to the buffer to fill with the IP header information.
 * @param len Pointer to the current length of the buffer.
 * @return None
 */
void fill_IP_header(t_iphdr *ip_hdr, char *buf, size_t *len)
{
	uint16_t totlen = ntohs(ip_hdr->tot_len);
	uint16_t ident = ntohs(ip_hdr->id);
	uint16_t frag = ntohs(ip_hdr->frag_off);

	uint8_t	 flags = (frag & 0xE000) >> 13; /* high 3 bits */
	uint16_t offset = frag & 0x1FFF;		/* low 13 bits */

	struct iphdr *inner_ip = NULL;
	char		  src[INET_ADDRSTRLEN];
	char		  dst[INET_ADDRSTRLEN];

	if (ip_hdr->protocol == IPPROTO_ICMP) {
		// Skip outer IP + ICMP headers to get to inner IP header
		inner_ip = (struct iphdr *)((char *)ip_hdr + (ip_hdr->ihl * 4) + sizeof(struct icmphdr));
		inet_ntop(AF_INET, &inner_ip->daddr, dst, sizeof(dst)); // Original destination
		inet_ntop(AF_INET, &ip_hdr->daddr, src, sizeof(src));	// Router that sent TTL exceeded
	} else {
		inet_ntop(AF_INET, &ip_hdr->saddr, dst, sizeof(dst));
		inet_ntop(AF_INET, &ip_hdr->daddr, src, sizeof(src));
	}

	*len += snprintf(buf + *len, sizeof(buf) - *len,
					 "Vr HL TOS  Len   ID Flg  off TTL Pro  cks       Src       Dst     Data\n");
	*len += snprintf(buf + *len, sizeof(buf) - *len,
					 " %1x  %1x  %02x %04x %04x   %1x %04x  %02x  %02x %04x %s  %s\n",
					 ip_hdr->version, ip_hdr->ihl, ip_hdr->tos, totlen, ident, flags, offset,
					 ip_hdr->ttl, ip_hdr->protocol, ntohs(ip_hdr->check), src, dst);
}

/**
 * Fills the buffer with a formatted dump of the ICMP header.
 * This function extracts relevant fields from the ICMP header,
 * including type, code, size, identifier, and sequence number,
 * and appends it to the buffer.
 * @param inner_hdr Pointer to the inner ICMP header structure.
 * @param outer_hdr Pointer to the outer ICMP header structure.
 * @param buf Pointer to the buffer to fill with the ICMP header information.
 * @param len Pointer to the current length of the buffer.
 * @param bytes The size of the ICMP packet.
 * @return None
 */
void fill_ICMP_header(t_icmphdr *inner_hdr, t_icmphdr *outer_hdr, char *buf, size_t *len,
					  size_t bytes)
{
	*len += snprintf(buf + *len, sizeof(buf) - *len,
					 "ICMP type: %d, code: %d, size: %zu, id: 0x%04X, seq: 0x%04X\n",
					 inner_hdr->type, ntohs(outer_hdr->code), bytes, ntohs(inner_hdr->un.echo.id),
					 ntohs(inner_hdr->un.echo.sequence));
}