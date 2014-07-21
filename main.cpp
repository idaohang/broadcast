#include <iostream>
#include <string>
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
#define BUFSIZE 10000
#define ERRORSTRING "ERROR"
const string host = "192.168.43.255";
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
	char inf [ ] = "ATI1\r\n";
	ssize_t wb = write(modem,inf,6);
	if (wb < 4) {
		cerr << "Error writing modem\n";
	}
	char buf[BUFSIZE];
	sleep(1);
	int i;
	int ii = read(modem,buf,BUFSIZE);
	cout << ii << endl;
	while(ii > 0) { 
		string raw(buf, ii);
		unsigned int found = raw.find("MEID:");
		if (found != string::npos) {
			string data = raw.substr(found+6, string::npos);
			cout << "ID:" << data;
			close(modem);
			return data;
		}
		i++;
		ii = read(modem,buf,BUFSIZE);
	}
	return ERRORSTRING;
}
//send data plus identifying info to host at host port
bool bcast (string line,string id) {
	try {
		UDPSocket sock;
		string data = line + id;
		sock.sendTo(data.c_str(),data.size(), host, host_port);
	}
	catch (SocketException &e) {
	cerr << e.what() << "\n";
	return false;
	}
	cout << "DATA SENT\n";
	return true;
}
int main () {
	int gpsdata = open(GPSFILE, O_RDONLY | O_NOCTTY | O_NDELAY);
	if (gpsdata == -1) {
		cerr << "error opening file\n";
	}
	else {
		fcntl(gpsdata,F_SETFL,0);
	}
	string id = ident();
	if (id == ERRORSTRING) {
		cerr << "error obtaining id\n";
	}
	
	char buf[BUFSIZE];
	while(read(gpsdata,buf,BUFSIZE)) {
		string data(buf);
		unsigned int found = data.find("GPRMC");
		if (found == 1) {
			cout << data << endl;
			bool test = bcast(data,id);
			if (!test) {
				cerr << "FAIL: " << data << endl;
			}
		}
		sleep(0);
	}
	
	cerr << "End of file?\n";
	close(gpsdata);
	return 1;
}
