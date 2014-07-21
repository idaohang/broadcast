#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <fcntl.h>
#include <termios.h>
#include "PracticalSocket.h"
using namespace std;

#define GPSFILE "/dev/ttyUSB0"
#define MODEMFILE "/dev/ttyUSB1"
#define BUFSIZE 100
#define ERRORSTRING "ERROR"
const string host = "192.168.96.204";
unsigned short host_port = 4551;

string ident () {
return "   ";
}
//send data plus identifying info to host at host port
bool bcast (string line) {
	try {
		UDPSocket sock;
		cout << "trying udp\n";
		string data = line + "::" + ident();
		sock.sendTo(data.c_str(),data.size(), host, host_port);
	}
	catch (SocketException &e) {
	cerr << e.what() << "\n";
	return false;
	}
	cout << "DATA SENT\n\n";
	return true;
}
int main () {
	cout << "running\n";
	int gpsdata = open(GPSFILE, O_RDONLY | O_NOCTTY | O_NDELAY);
	if (gpsdata == -1) {
		cerr << "error opening file\n";
	}
	else {
		fcntl(gpsdata,F_SETFL,0);
		cout << "started\n";
	}
	char buf[BUFSIZE];
	cout << "ready\n";
	while(read(gpsdata,buf,BUFSIZE)) {
		cout << "finding data\n";
		string data(buf);
		cout << data << endl;
		unsigned int found = data.find("GPRMC");
		if (found == 1) {
			cout << "found data at " << found << endl;;
			bool test = bcast(data);
			if (!test) {
				cerr << "FAIL: " << data << endl;
			}
		}
		sleep(1);
	}
	cerr << "End of file?\n";
	close(gpsdata);
	return 0;
}
