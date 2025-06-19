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
