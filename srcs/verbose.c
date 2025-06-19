/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   verbose.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 16:26:50 by lagea             #+#    #+#             */
/*   Updated: 2025/06/19 18:29:58 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ping.h"

/**
 * fill_IP_header_dump()
 *   ip_hdr: pointer to the IP header structure
 *   buf: buffer to fill with IP header information
 *   len: pointer to the current length of the buffer
 *
 * Writes:
 *   Fills the buffer with IP version, IHL, TOS, total length, ID, fragment offset,
 *   TTL, protocol, checksum, source address, and destination address.
 */
void fill_IP_header_dump(t_iphdr *ip_hdr, char *buf,  size_t *len, const unsigned char *packet)
{
	size_t ihl_bytes = ip_hdr->ihl * 4;

    *len += snprintf(buf + *len, sizeof(buf) - *len, "IP Hdr Dump:\n ");

    /* 2. Hex pairs with spacing/newlines like inetutils ping */
    for (size_t i = 0; i < ihl_bytes; ++i) {
		int byte = packet[i];
		if (byte )
        *len += snprintf(buf + *len, sizeof(buf) - *len, "%02x", packet[i]);

        if ((i & 1) == 1)
            *len += snprintf(buf + *len, sizeof(buf) - *len, " ");
    }

    *len += snprintf(buf + *len, sizeof(buf) - *len, "\n");
}

/**
 * fill_IP_header_dump()
 *   ip_hdr: pointer to the IP header structure
 *   buf: buffer to fill with IP header information
 *   len: pointer to the current length of the buffer
 *
 * Writes:
 *   Fills the buffer with IP version, IHL, TOS, total length, ID, flags, fragment offset,
 *   TTL, protocol, and checksum.
 */
void fill_IP_header(t_iphdr *ip_hdr, char *buf, size_t *len)
{
    uint16_t totlen = ntohs(ip_hdr->tot_len);
    uint16_t ident  = ntohs(ip_hdr->id);
    uint16_t frag   = ntohs(ip_hdr->frag_off);

    uint8_t  flags  = (frag & 0xE000) >> 13;   /* high 3 bits */
    uint16_t offset =  frag & 0x1FFF;          /* low 13 bits */

	struct iphdr *inner_ip = NULL;
    char src[INET_ADDRSTRLEN];
    char dst[INET_ADDRSTRLEN];

	if (ip_hdr->protocol == IPPROTO_ICMP) {
		// Skip outer IP + ICMP headers to get to inner IP header
		inner_ip = (struct iphdr *)((char *)ip_hdr + (ip_hdr->ihl * 4) + sizeof(struct icmphdr));
		inet_ntop(AF_INET, &inner_ip->daddr, dst, sizeof(dst));  // Original destination
		inet_ntop(AF_INET, &ip_hdr->daddr, src, sizeof(src));    // Router that sent TTL exceeded
	} else {
		inet_ntop(AF_INET, &ip_hdr->saddr, dst, sizeof(dst));
		inet_ntop(AF_INET, &ip_hdr->daddr, src, sizeof(src));
	}


    *len += snprintf(buf + *len, sizeof(buf) - *len, "Vr HL TOS  Len   ID Flg  off TTL Pro  cks       Src       Dst     Data\n");
    *len += snprintf(buf + *len, sizeof(buf) - *len, " %1x  %1x  %02x %04lx %04x   %1x %04x  %02x  %02x %04x %s  %s\n",
           ip_hdr->version,
           ip_hdr->ihl,
           ip_hdr->tos,
           totlen - sizeof(struct iphdr),
           ident,
           flags,
           offset,
           ip_hdr->ttl,
           ip_hdr->protocol,
           ntohs(ip_hdr->check),
           src,
           dst);
}

/**
 * fill_ICMP_header()
 *   inner_hdr: pointer to the inner ICMP header (Echo Request)
 *   outer_hdr: pointer to the outer ICMP header (Echo Reply)
 *   buf: buffer to fill with ICMP header information
 *   len: pointer to the current length of the buffer
 *   bytes: size of the payload
 *
 * Writes:
 *   Fills the buffer with ICMP type, code, size, id, and sequence number.
 */
void fill_ICMP_header(t_icmphdr *inner_hdr, t_icmphdr *outer_hdr, char *buf, size_t *len, size_t bytes)
{
	*len += snprintf(buf + *len , sizeof(buf) - *len, "ICMP type: %d, code: %d, size: %zu, id: 0x%04X, seq: 0x%04X\n",
			inner_hdr->type,
			ntohs(outer_hdr->code),
			bytes,
			ntohs(inner_hdr->un.echo.id),
			ntohs(inner_hdr->un.echo.sequence));
}