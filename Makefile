all: clean
	g++ -O3 -Wall main.cpp PracticalSocket.cpp ident.cpp -o bcast
clean:
	rm -rf *.o *~ bcast
install: all
	cp bcast /usr/local/bin
	chown root /usr/local/bin/bcast
	chmod 1771 /usr/local/bin/bcast
	mkdir -p  /etc/bcast
	cp -rf ./broadcast  /etc
uninstall: clean
	rm -f /usr/local/bin/bcast
	rm -rf /etc/broadcast 
