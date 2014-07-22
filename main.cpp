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
#define SERV_TIME 45 // about 15 seconds
#define SUPDATE 15000 //about 80 minutes
#define CONFLOC "/etc/bcast/"

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
bool bcast (string line,string id,string hostip,unsigned short destport) {
	try {
		UPDSocket sock;
		string data = line + "," + id;
		sock.sendTo(data.c_str(),data.size(), hostip, destport);
		sock.cleanUp();
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
	int i = 0; //time to broadcast to server
	int j = SUPDATE; //time to update ip files
	string localip;
	unsigned short localport;
	string serverip;
	unsigned short serverport;
	while(read(gpsdata,buf,BUFSIZE)) {
		string data(buf);
		unsigned int found = data.find("$GPRMC");
		if (found == 0) {
			int type = 0;
			bool test = bcast(data,id,local,localport);
			if (!test) {
				cerr << "\nFAIL: " << data << endl;
			}
			++i;
			if (i >= SERV_TIME) {
				type = 1;
				bool test = bcast(data,id,host,hostport);
				if (!test) {
					cerr << "\nFAIL: " << data << endl;
				}
				i = 0;
			}
			++j;
			if (j >= SUPDATE) {
				int lip = open(CONFLOC+"lip",O_RDONLY);
				int lport = open(CONFLOC+"lport",O_RDONLY);
				int sip = open(CONFLOC+"sip",O_RDONLY);
				int sport = open(CONFLOC+"sport",O_RDONLY);
				if (lip == -1 || lport == -1 || sip == -1 || sport == -1) {
					cerr << "A conf file failed to open, no update";
					break;
				}
				localip = updateip(lip);
				localport = updateport(lport);
				server = updateip(sip);
				serverport = updateport(sport);
				}
				j = 0;
			}
		}
		sleep(0);
	}
	
	cerr << "\nEnd of file?\n";
	close(gpsdata);
	return 1;
}
