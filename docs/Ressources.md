To enable the unprivileged sockets creation:
	sudo sysctl -w net.ipv4.ping_group_range="0 2147483647"


To make it permanent(not recommended):
	add this line in /etc/sysctl.conf:
		net.ipv4.ping_group_range = 0 2147483647


To test and see the ICMP packets:
	sudo tcpdump -nn -vvv -s 0 icmp and dst host 8.8.8.8
	./ft_ping 8.8.8.8