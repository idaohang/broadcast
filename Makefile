all: clean
	g++ -O3 -Wall main.cpp PracticalSocket.cpp -o bcast
clean:
	rm -rf *.o *~ bcast
install: all
	cp bcast /usr/local/bin
	chown root /usr/local/bin/bcast
	chmod 1771 /usr/local/bin/bcast
uninstall: clean
	rm /usr/local/bin/bcast
