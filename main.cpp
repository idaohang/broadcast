#include <iostream>
#include <string>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <fcntl.h>
#include <termios.h>
#include <ctime>
#include "PracticalSocket.h"
using namespace std;

#define GPSFILE "/dev/ttyUSB1"
#define MODEMFILE "/dev/ttyUSB2"
#define BUFSIZE 10000
#define ERRORSTRING "ERROR"
#define SERV_TIME 15
#define SUPDATE 3600 //one hour
#define CONFLOC "/etc/bcast/"

UDPSocket sock;
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
		string data = line + "," + id;
		sock.sendTo(data.c_str(),data.size(), hostip, destport);
	}
	catch (SocketException &e) {
	cerr << "\n" << e.what() << "\n";
	return false;
	}
	return true;
}
string updateip (int fd) {
	cout << "\nUpdating ip";
	char buffer[BUFSIZE];
	read(fd,&buffer,BUFSIZE);
	string data(buffer);
	cout << "\nIP: " << data;
	return data;
}
unsigned short updateport (int fd) {
	cout << "\nUpdating port";
	unsigned short buffer;
	read(fd,&buffer,sizeof(buffer));
	cout << "\nPort: " << buffer;
	return buffer;
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
	time_t starttime;
	time(&starttime);
	time_t curtime;
	double seconds;
	string localip = " ";
	unsigned short localport = 0;
	string serverip = " ";
	unsigned short serverport = 0;
	bool first = true;
	while(read(gpsdata,buf,BUFSIZE)) {
		if (seconds >= SUPDATE || first) {
			int lip = open(CONFLOC"lip",O_RDONLY);
			int lport = open(CONFLOC"lport",O_RDONLY);
			int sip = open(CONFLOC"sip",O_RDONLY);
			int sport = open(CONFLOC"sport",O_RDONLY);
			if (lip == -1 || lport == -1 || sip == -1 || sport == -1) {
				cerr << "\nA conf file failed to open, no update";
				cerr <<"\nlip: " << lip << endl;
				cerr << "\nlport: " << lport << endl;
				cerr << "\nsip: " << sip << endl;
				cerr << "\nsport: " << sport << endl;
				sleep(0);
				break;
			}
			localip = updateip(lip);
			localport = updateport(lport);
			serverip = updateip(sip);
			serverport = updateport(sport);
			close(lip);
			close(lport);
			close(sip);
			close(sport);
			first = false;
		}

		string data(buf);
		unsigned int found = data.find("$GPRMC");
		if (found == 0) {
			bool test = bcast(data,id,localip,localport);
			if (!test) {
				cerr << "\nFAIL: " << data << endl;
			}
			time(&curtime);
			seconds = difftime(curtime,starttime);
			cout << seconds;
			if (seconds >= SERV_TIME) {
				bool test = bcast(data,id,serverip,serverport);
				if (!test) {
					cerr << "\nFAIL: " << data << endl;
				}
			}
		}
		sleep(0);
	}
	cerr << "\nEnd of file?\n";
	close(gpsdata);
	return 0;
}
