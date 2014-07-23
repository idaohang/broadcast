BFILES = PracticalSocket.cpp ident.cpp
IFILES = ident.cpp
CPP = g++
FLAGS = -Wall
CURL = -lcurl

all: clean bcast info
bcast: main.cpp $(BFILES)
	$(CPP) $(FLAGS) main.cpp $(BFILES) -o bcast
info: info.cpp $(IFILES)
	$(CPP) $(FLAGS) info.cpp $(IFILES) -o info $(CURL)
clean:
	rm -rf *.o *~ bcast
install: all
	cp bcast /usr/local/bin
	cp info /usr/local/bin
	chown root /usr/local/bin/bcast
	chmod 1771 /usr/local/bin/bcast
	mkdir -p  /opt/
	cp -rf ./broadcast  /opt
uninstall: clean
	rm -f /usr/local/bin/bcast
	rm -rf /etc/broadcast 
