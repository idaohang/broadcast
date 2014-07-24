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

#define GPSFILE "/dev/ttyUSB1"
#define BUFSIZE 10000 // for modem and gps
#define SBUFSIZE 16 // for conf updates
#define ERRORSTRING "ERROR"
#define SERV_TIME 15
#define SUPDATE 3600 //one hour
#define SIP "/opt/broadcast.sip.conf"
#define SPORT "/opt/broadcast.sport.conf"
#define LIP "/opt/broadcast.lip.conf"
#define LPORT "/opt/broadcast.lport.conf"
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
	//printf("\nIP: (%s)\n", buffer);
	//cout << "\nIP: " << data;
	//cout << "hello\n";
	//return data;
}
vector<unsigned short> updateport (int fd) {
	cout << "\nUpdating port";
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
	/*
	stringstream str(xx);
	//cout << "\nPort String: " << data;
	unsigned int portshort;
	str >> portshort;
	//cout << "\nPort Short: " << portshort;
	if (!portshort) {
		cerr << "\nconversion failed";
		return DEFAULTPORT;
	}
	return (unsigned short)portshort;
	*/
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
	vector<string> localip;
	vector<unsigned short> localport;
	vector<string> serverip;
	vector<unsigned short> serverport;
	bool first = true;
	int fail = 0;
	int which = 0;
	while(read(gpsdata,buf,BUFSIZE)) {
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
			/*
			int lip = open(CONFLOC"bcast.lip.conf",O_RDONLY);
			int lport = open(CONFLOC"bcast.lport.conf",O_RDONLY);
			int sip = open(CONFLOC"bcast.sip.conf",O_RDONLY);
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
			*/
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
			if (which > 2) {
				which = 0;
			}
			bool test = bcast(data,id,localip[which],localport[which]);
			if (!test) {
				++fail;
				cerr << "\nFAIL: " << fail << endl;
			}
			time(&curtime);
			seconds = difftime(curtime,starttime);
			//cout << seconds << endl;
			if (seconds >= SERV_TIME) {
				time(&starttime);
				bool test = bcast(data,id,serverip[which],serverport[which]);
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
		msleep(10);
	}
	cerr << "\nEnd of file?\n";
	close(gpsdata);
	return 0;
}
