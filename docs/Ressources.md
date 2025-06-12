To test and see the ICMP packets:
	sudo tcpdump -nn -vvv -s 0 icmp and dst host 8.8.8.8
	./ft_ping 8.8.8.8