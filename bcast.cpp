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
#define SBUFSIZE 50 // for conf updates
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
#define NUM 3

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
	//cout << "\nUpdating ip\n";
	char buffer[SBUFSIZE];
	vector<string> v;
	int i = read(fd,&buffer,SBUFSIZE);
	string data(buffer,i-1);
	istringstream ss(data);
	string x;
	while(ss >> x) {
		cout << x << "\n";
		v.push_back(x);
	}
	cout << "v 0 " << v[0] << "\n";
	cout << "v 1 " << v[1] << "\n";
	cout << "v 2 " << v[2] << "\n";
	return v;
}
vector<unsigned short> updateport (int fd) {
	cout << "\nUpdating port\n";
	char buffer[SBUFSIZE];
	int i = read(fd,&buffer,SBUFSIZE);
	string data(buffer,i-1);
	istringstream ss(data);
	vector<unsigned short> v;
	string x;
	while(ss >> x) {
		cout << x << endl;
		stringstream str(x);
		unsigned short portshort;
		str >> portshort;
		cout << portshort << "\n";
		v.push_back(portshort);
	}
	for (int i = 0; i < 3; ++i) {
		cout << "V[" << i << "]: " << v[i] << "\n";
	}
	return v;
}
int main () {
	//string s_ip = DEFAULTIP;
	//unsigned short us_port = DEFAULTPORT;
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
	int numip;
	int rl = 0;
	char buf[BUFSIZE];
	rl = read(gpsdata,buf,BUFSIZE);
	numip = 0;
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
			cout << "\ndata found\n";
			numip++;
			//cout << "numip ++\n";
			if (numip > NUM - 1) {
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
			//cout << "Seconds: " << seconds << endl;
			if (seconds >= SERV_TIME) {
				cout << "\nSERVER\n";
				time(&starttime);
				if(serverip[numip] != BLANKIP) {
					cout << "Trying " << serverip[numip] << endl;
					bool test = bcast(data,id,serverip[numip],serverport[numip]);
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
		cout << "\n";
	}
	cerr << "\nModem has stopped transmitting data\n";
	close(gpsdata);
	return 0;
}
