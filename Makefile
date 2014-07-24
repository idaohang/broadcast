BFILES = PracticalSocket.cpp ident.cpp
IFILES = ident.cpp
CPP = g++
FLAGS = -Wall
CURL = -lcurl
BINLOC = /usr/local/bin/
CONFLOC = /opt/
CONF = conf
all: clean bcast info
bcast: bcast.cpp $(BFILES)
	$(CPP) $(FLAGS) bcast.cpp $(BFILES) -o bcast
info: info.cpp $(IFILES)
	$(CPP) $(FLAGS) info.cpp $(IFILES) -o info $(CURL)
clean:
	rm -rf *.o *~ bcast
install: all
	cp bcast $(BINLOC)
	cp info $(BINLOC)
	chown root $(BINLOC)bcast
	chmod 1771 $(BINLOC)bcast
	mkdir -p  $(CONFLOC)
	cp -rf ./conf/* $(CONFLOC)
uninstall: clean
	rm -f $(BINLOC)bcast $(BINLOC)info 
	rm -rf $(CONFLOC)broadcast.conf $(CONFLOC)broadcast.local.conf
