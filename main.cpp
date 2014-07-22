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
#define SERV_TIME 45 //about 15 seconds
const string host = "192.168.96.204";
const unsigned short destport = 4451;
const string server = "24.248.166.181";
UDPSocket sock;
UDPSocket socka;
string ident () {
	int modem = open(MODEMFILE, O_RDWR | O_NOCTTY | O_NDELAY);
	if (modem == -1) {
		cerr << "\nError opening modem\n";
		return ERRORSTRING;
	}
	else {
		fcntl(modem, F_SETFL, 0);
	}
	char inf [ ] = "ATI1\r\n";
	ssize_t wb = write(modem,inf,6);
	if (wb < 4) {
		cerr << "\nError writing modem\n";
	}
	char buf[BUFSIZE];
	sleep(1);
	int i;
	int ii = read(modem,buf,BUFSIZE);
	while(ii > 0) { 
		string raw(buf, ii);
		unsigned int found = raw.find("MEID:");
		if (found != string::npos) {
			string data = raw.substr(found+6, string::npos);
			cout << "\nID:" << data;
			close(modem);
			return data;
		}
		i++;
		ii = read(modem,buf,BUFSIZE);
	}
	return ERRORSTRING;
}
//send data plus identifying info to host at host port
bool bcast (string line,string id,int type) {
	try {
		string data = line + "," + id;
		if (type == 0) {
			sock.sendTo(data.c_str(),data.size(), host, destport);
		}
		else if (type == 1) {
			
			socka.sendTo(data.c_str(),data.size(),server,destport);
		}
	}
	catch (SocketException &e) {
	cerr << "\n" << e.what() << "\n";
	return false;
	}
	return true;
}
int main () {
	int gpsdata = open(GPSFILE, O_RDONLY | O_NOCTTY | O_NDELAY);
	if (gpsdata == -1) {
		cerr << "\nerror opening file\n";
	}
	else {
		fcntl(gpsdata,F_SETFL,0);
	}
	string id = ident();
	if (id == ERRORSTRING) {
		cerr << "\nerror obtaining id\n";
	}
	
	char buf[BUFSIZE];
	int i = 0;
	while(read(gpsdata,buf,BUFSIZE)) {
		string data(buf);
		unsigned int found = data.find("$GPRMC");
		if (found == 0) {
			int type = 0;
			bool test = bcast(data,id,type);
			if (!test) {
				cerr << "\nFAIL: " << data << endl;
			}
			++i;
			if (i >= SERV_TIME) {
				type = 1;
				bool test = bcast(data,id,type);
				if (!test) {
					cerr << "\nFAIL: " << data << endl;
				}
				i = 0;
			}
		}
		sleep(0);
	}
	
	cerr << "\nEnd of file?\n";
	close(gpsdata);
	return 1;
}
