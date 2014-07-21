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

#define GPSFILE "/dev/ttyUSB1"
#define MODEMFILE "/dev/ttyUSB2"
#define BUFSIZE 100
#define ERRORSTRING "ERROR"
const string host = "192.168.96.204";
unsigned short host_port = 4551;

string ident () {
	int modem = open(MODEMFILE, O_RDWR | O_NOCTTY | O_NDELAY);
	if (modem == -1) {
		cerr << "Error opening modem\n";
		return ERRORSTRING;
	}
	else {
		fcntl(modem, F_SETFL, 0);
	}
	char inf [ ] = "ATI1/r/n";
	ssize_t wb = write(modem,inf,6);
	cout << "wb: " << wb << endl;
	if (wb < 4) {
		cerr << "Error writing modem\n";
	}
	char buf[BUFSIZE];
	string ar[BUFSIZE];
	sleep(1);
	cout << "done sleeping\n";
	int i;
	while(read(modem, buf, BUFSIZE) > 0) { 
		cout << "workingon stuff\n";
		string raw(buf, BUFSIZE);
		cout << "More stuff\n";
		ar[i] = raw;
		cout << "buf:" << buf << endl;
		i++;
	}
	for (i = 0; i < BUFSIZE; i++) {
		cout << i << " " << ar[i];
		unsigned int found = ar[i].find("MEID:");
		if (found != string::npos) {
			string data = ar[i].substr(found+5, string::npos);
			cout << "ID: " << data;
			close(modem);
			return data;
		}
	}
	return ERRORSTRING;
}
//send data plus identifying info to host at host port
bool bcast (string line,string id) {
	try {
		UDPSocket sock;
		cout << "trying udp\n";
		string data = line + "::" + id;
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
		cout << "opened gps\n";
	}
	string id = ident();
	if (id == ERRORSTRING) {
		cerr << "error obtaining id\n";
	}
	/*
	char buf[BUFSIZE]
	cout << "ready\n";
	while(read(gpsdata,buf,BUFSIZE)) {
		cout << "finding data\n";
		string data(buf);
		cout << data << endl;
		unsigned int found = data.find("GPRMC");
		if (found == 1) {
			cout << "found data at " << found << endl;;
			bool test = bcast(data,id);
			if (!test) {
				cerr << "FAIL: " << data << endl;
			}
		}
		sleep(1);
	}
	*/
	cerr << "End of file?\n";
	close(gpsdata);
	return 0;
}
