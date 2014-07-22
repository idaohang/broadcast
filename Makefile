all: clean
	g++ -O3 -Wall main.cpp PracticalSocket.cpp -o bcast
clean:
	rm -rf *.o *~ bcast 
install: all
	cp bcast /usr/local/bin
