SOCK = PracticalSocket.cpp 
ID = ident.cpp
CPP = g++
FLAGS = -Wall -O3
CURL = -lcurl
BINLOC = /usr/local/bin/
CONFLOC = /opt/
CONF = conf

all: clean bcast info

bcast: bcast.cpp $(SOCK) $(ID)
	$(CPP) $(FLAGS) bcast.cpp $(SOCK) $(ID) -o bcast

info: info.cpp $(ID)
	$(CPP) $(FLAGS) info.cpp $(ID) -o info $(CURL)

listen: listen.cpp $(SOCK)
	$(CPP) $(FLAGS) listen.cpp $(SOCK) -o listen
clean:
	rm -rf *.o *~ bcast

install: all
	cp bcast $(BINLOC)
	cp info $(BINLOC)
	cp devdetect.sh $(BINLOC)broadcast
	mkdir -p  $(CONFLOC)
	cp -rf ./conf/* $(CONFLOC)

uninstall: clean
	rm -f $(BINLOC)bcast $(BINLOC)info 
	rm -rf $(CONFLOC)broadcast.conf $(CONFLOC)broadcast.local.conf
