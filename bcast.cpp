#include <vector>
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
#include <algorithm>
#include <iterator>
#include "PracticalSocket.h"
#include "ident.h"
using namespace std;

#define BUFSIZE 10000 // for modem and gps
#define SBUFSIZE 16 // for conf updates
#define ERRORSTRING "ERROR"
#define SERV_TIME 15
#define SUPDATE 400 //5 minutes 30 seconds
#define SIP "/opt/broadcast.sip.conf"
#define SPORT "/opt/broadcast.sport.conf"
#define LIP "/opt/broadcast.lip.conf"
#define LPORT "/opt/broadcast.lport.conf"
#define DEFAULTIP "24.248.166.184";
//#define DEFAULTIP "192.168.96.204";
//#define DEFAULTIP "24.248.166.181";
#define DEFAULTPORT 4451
#define FAIL 30 // Maximum # of failiures allowed
#define BLANKIP "0.0.0.0"

UDPSocket sock;

//send data plus identifying info to host at host port
bool bcast (string line,string id,string &hostip, unsigned short destport) {
	try {
		string data = line + "," + id;
		sock.sendTo(data.c_str(),data.size(), hostip, destport);
		cout << "Local Port: " << sock.getLocalPort() << "\n";

	}
	catch (SocketException &e) {
	cerr << "\n" << e.what() << "\n";
	return false;
	}
	sock.disconnect();
	return true;
}

vector<string> updateip (int fd) {
	//cout << "\nUpdating ip";
	char buffer[SBUFSIZE];
	vector<string> v;
	int i = read(fd,&buffer,SBUFSIZE);
	string data(buffer,i-1);
	istringstream ss(data);
	while(!ss.eof()) {
		string x;
		getline(ss,x,' ');
		v.push_back(x);
	}
	return v;
}
vector<unsigned short> updateport (int fd) {
	//cout << "\nUpdating port";
	char buffer[SBUFSIZE];
	int i = read(fd,&buffer,SBUFSIZE);
	string data(buffer,i-1);
	istringstream ss(data);
	vector<unsigned short> v;
	while(!ss.eof()) {
		string x;
		getline(ss,x,' ');
		stringstream str(x);
		unsigned short portshort;
		str >> portshort;
		v.push_back(portshort);
	}
	return v;
}
int main () {
	string s_ip = DEFAULTIP;
	unsigned short us_port = DEFAULTPORT;
	string id = ident();
	int gpsdata;

	do {
		gpsdata = open("/dev/ttyUSB1", O_RDONLY | O_NOCTTY | O_NDELAY);
		if (gpsdata == -1) {
			perror("GPS OPEN");
		}
	} while(gpsdata == -1);
	fcntl(gpsdata,F_SETFL,0);
/*
	char buffer[BUFSIZE];
	int rb = read(gpsdata,buffer,BUFSIZE);
	if(rb == -1) { //there is a serial adapter plugged in because we get no data
		close(gpsdata);
		gpsdata = open("/dev/ttyUSB1", O_RDONLY | O_NOCTTY | O_NDELAY);
		if (gpsdata == -1) {
			perror("GPS OPEN 2");
		}
	}
	else {
		string stuff(buffer,BUFSIZE);
		if ((signed int)stuff.find("$G") == -1) { //there is a serial adapter plugged in because we don't get gps data
			close(gpsdata);
			gpsdata = open("/dev/ttyUSB1", O_RDONLY | O_NOCTTY | O_NDELAY);
			if (gpsdata == -1) {
				perror("GPS OPEN 3");
			}

		}
	}
*/
	if (id == ERRORSTRING) {
		cerr << "\nerror obtaining id\n";
	}
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
	vector<string> localip;
	vector<unsigned short> localport;
	vector<string> serverip;
	vector<unsigned short> serverport;
	bool first = true;
	int fail = 0;
	int numip = 0;
	int rl = 0;
	char buf[BUFSIZE];
	rl = read(gpsdata,buf,BUFSIZE);
	while(rl > 0) {
		//cout << "Bcast loop\n";
		time(&ucurtime);
		useconds = difftime(ucurtime,ustarttime);
		//cout << "USeconds: " << useconds << endl;
		if (useconds >= SUPDATE || first) {
			cout << "\nUpdating conf\n";
			time(&ustarttime);
			int sip = open(SIP,O_RDONLY);
			int sport = open(SPORT,O_RDONLY);
			int lip = open(LIP,O_RDONLY);
			int lport = open(LPORT,O_RDONLY);
			if (sip == -1 || sport == -1 || lip == -1 || lport == -1) {
				cerr << "\nFailed to open conf files. Retrying...\n";
				cerr << "lip: " << lip << " lport: " << " sip: " << sip << " sport: " << sport << endl;
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
			cout << "\nBcast conf updated successfully\n" << endl;
		}
		string alldata(buf, rl);
		//cout << "all data from buf\n";
		string data = "000";
		//cout << "data init\n";
		if (!alldata.empty()) {
			//cout << "fill data";
			data = alldata.substr(0,alldata.length() - 1);
		}
		//cout << "\nGPS DATA: " << data << "\n";
		unsigned int found = data.find("$GPRMC");
		//cout << "find data\n";
		if (found == 0) {
			//cout << "\ndata found\n";
			numip++;
			//cout << "numip ++";
			if (numip > 2) {
				numip = 0;
			}
			if (localip[numip] != BLANKIP) {
				bool test = bcast(data,id,localip[numip],localport[numip]);
				if (!test) {
					++fail;
					cerr << "\nFAIL: " << fail << endl;
				}
			}
			time(&curtime);
			seconds = difftime(curtime,starttime);
			//cout << seconds << endl;
			if (seconds >= SERV_TIME) {
				cout << "\nSERVER\n";
				time(&starttime);
				if(s_ip != BLANKIP) {
					cout << "Trying " << s_ip << endl;
					bool test = bcast(data,id,s_ip,us_port);
					if (!test) {
						++fail;
						cerr << "\nServer FAIL: " << fail << endl;
					}
				}
			}
		}
		if (fail >= FAIL) {
			cerr << "\nMaximum Failiure. Rebooting\n";
			close(gpsdata);
			//system("reboot");
			break;
		}
		msleep(100);
		rl = read(gpsdata,buf,BUFSIZE);
	}
	cerr << "\nEnd of file?\n";
	close(gpsdata);
	return 0;
}
