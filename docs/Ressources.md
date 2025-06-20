To enable the unprivileged sockets creation:
	sudo sysctl -w net.ipv4.ping_group_range="0 2147483647"


To make it permanent(not recommended):
	add this line in /etc/sysctl.conf:
		net.ipv4.ping_group_range = 0 2147483647


To test and see the ICMP packets:
	sudo tcpdump -nn -vvv -s 0 icmp and dst host 8.8.8.8
	./ft_ping 8.8.8.8

To get all symbols from libc:
	nm /usr/lib/x86_64-linux-gnu/libc.a | grep ' T ' > libc.txt

### Layer explanation
| Layer                        | \*\*Simple Echo \*\*<br>*(request you transmit & reply you expect)*                     | \*\*TTL-exceeded/Error \*\*<br>*(what a router sends back when TTL hits 0)*                      |
| ---------------------------- | --------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------ |
| **Outer IPv4 header**        | 20 bytes                                                                                | 20 bytes – added by the router that generated the error                                          |
| **Outer ICMP header**        | 8 bytes   `Type 0` (Echo-Reply) / `Type 8` (Echo-Request)<br>`Code 0`                   | 8 bytes   `Type 11` (Time-to-live exceeded) / `Type 3` (Unreachable)<br>`Code` depends on reason |
| **Quoted inner IPv4 header** | *(none)*                                                                                | 20 bytes — first 20 bytes of the IP packet that caused the error (your Echo-Request)             |
| **Quoted inner ICMP header** | *(none)*                                                                                | 8 bytes — the ICMP Echo header you originally sent (`Type 8 Code 0`, *id*, *seq*)                |
| **Payload**                  | 56 bytes (default) — often starts with a `struct timeval` timestamp followed by padding | 56 bytes — router copies the first 56 data bytes of your packet                                  |


## Time to live exceeded explanation

IP Hdr Dump:
 4500 0054 7ca9 4000 3f01 a750 0a00 020f 0a9c 0105
| Hex (20 B) | Field → Value                                                    | Interpretation |
| ---------- | ---------------------------------------------------------------- | -------------- |
| `45`       | **Version 4**, **IHL 5** (5 × 4 = 20 B header)                   |                |
| `00`       | **TOS/DSCP 00**                                                  |                |
| `0054`     | **Total length 0x54** = *84 bytes* (20 IP + 8 ICMP + 56 payload) |                |
| `7ca9`     | **Identification 0x7ca9**                                        |                |
| `4000`     | **Flags = 010** (`DF` set), **offset 0**                         |                |
| `3f`       | **TTL 0x3f** = 63 (was 64, decremented once before looping)      |                |
| `01`       | **Protocol 1** = ICMP                                            |                |
| `a750`     | Header checksum                                                  |                |
| `0a00020f` | **Src 10.0.2.15** (your VM)                                      |                |
| `0a9c0105` | **Dst 10.156.1.5** (final target)                                |                |

Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data
 4  5  00 0054 7ca9   2 0000  3f  01 a750 10.0.2.15  10.156.1.5
Flg 2 → DF bit set (010₂).

off 0000 → not fragmented.

TTL 0x3f matches the hex dump.


ICMP: type 8, code 0, size 64, id 0xd02c, seq 0x0002
| Field               | Value                                                                  | Note |
| ------------------- | ---------------------------------------------------------------------- | ---- |
| **type 8 / code 0** | Echo-Request                                                           |      |
| **size 64**         | 8-byte ICMP header + 56-byte payload                                   |      |
| **id 0xd02c**       | Low 16 bits of your process PID                                        |      |
| **seq 0x0002**      | Third probe in this run (`-c 1` only sends one, here for illustration) |      |
