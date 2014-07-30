FILES = main.cpp PracticalSocket.cpp ident.cpp info.cpp
CPP = g++
FLAGS = -Wall -O3 -lcurl
BINLOC = /usr/local/bin/

all: clean bcast

bcast: $(FILES)
	$(CPP) $(FLAGS) $(FILES) -o bcast
clean:
	rm -rf *.o *~ bcast

install: all
	cp bcast $(BINLOC)
	cp devdetect.sh $(BINLOC)broadcast

uninstall: clean
	rm -f $(BINLOC)bcast 
