#include <iostream>
#include <string>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <fcntl.h>
#include <termios.h>
#include <ctime>
#include <sstream>
#include "PracticalSocket.h"
#include "ident.h"
using namespace std;

#define GPSFILE "/dev/ttyUSB1"
#define BUFSIZE 10000 // for modem and gps
#define SBUFSIZE 16 // for conf updates
#define ERRORSTRING "ERROR"
#define SERV_TIME 15
#define SUPDATE 3600 //one hour
#define CONFLOC "/etc/broadcast/"
#define DEFAULTPORT 4451
#define FAIL 10 // Maximum # of failiures allowed

UDPSocket sock;

//send data plus identifying info to host at host port
bool bcast (string line,string id,string &hostip,unsigned short destport) {
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
	//cout << "\nUpdating ip";
	char buffer[SBUFSIZE];
	int i = read(fd,&buffer,SBUFSIZE);
	string data(buffer,i-1);
	//printf("\nIP: (%s)\n", buffer);
	//cout << "\nIP: " << data;
	//cout << "hello\n";
	return data;
}
unsigned short updateport (int fd) {
	//cout << "\nUpdating port";
	char buffer[SBUFSIZE];
	int i = read(fd,&buffer,SBUFSIZE);
	string data(buffer,i-1);
	stringstream str(buffer);
	//cout << "\nPort String: " << data;
	unsigned int portshort;
	str >> portshort;
	//cout << "\nPort Short: " << portshort;
	if (!portshort) {
		cerr << "\nconversion failed";
		return DEFAULTPORT;
	}
	return (unsigned short)portshort;
}
int main () {
	int gpsdata = open(GPSFILE, O_RDONLY | O_NOCTTY | O_NDELAY);
	if (gpsdata == -1) {
		cerr << "\nerror opening gps\n";
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
	time(&curtime);
	double seconds;
	time_t ustarttime;
	time(&ustarttime);
	time_t ucurtime;
	time(&ucurtime);
	double useconds;
	string localip = " ";
	unsigned short localport = 0;
	string serverip = " ";
	unsigned short serverport = 0;
	bool first = true;
	int fail = 0;
	while(read(gpsdata,buf,BUFSIZE)) {
		useconds = difftime(ucurtime,ustarttime);
		//cout << "USeconds: " << useconds << endl;
		if (useconds >= SUPDATE || first) {
			//cout << "\nUpdating conf\n";
			time(&ustarttime);
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
				++fail;
				cerr << "Conf FAIL: " << fail;
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
				++fail;
				cerr << "\nFAIL: " << fail << endl;
			}
			time(&curtime);
			seconds = difftime(curtime,starttime);
			//cout << seconds << endl;
			if (seconds >= SERV_TIME) {
				time(&starttime);
				bool test = bcast(data,id,serverip,serverport);
				if (!test) {
					++fail;
					cerr << "\nServer FAIL: " << fail << endl;
				}
			}
		}
		if (fail >= FAIL) {
			cerr << "\nMaximum Failiure. Exiting...\n";
			close(gpsdata);
			break;
		}
		sleep(0);
	}
	cerr << "\nEnd of file?\n";
	close(gpsdata);
	return 0;
}
