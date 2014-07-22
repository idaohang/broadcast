FILES  = PracticalSocket.cpp ident.cpp
CPP = g++
FLAGS = -Wall
all: clean
	$(CPP) $(FLAGS) main.cpp $(FILES) -o bcast
	$(CPP) $(FLAGS) info.cpp $(FILES) -o info 
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
