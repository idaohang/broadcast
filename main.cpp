#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "PracticalSocket.h"
using namespace std;
#define GPSFILE "/dev/ttyUSB0"

const string host = "192.168.96.181";
unsigned short host_port = 4551;
bool bcast (string line) {
	try {
		UDPSocket sock;
		cout << "trying udp";
		sock.sendTo(line.c_str(),line.size(), host, host_port);
	}
	catch (SocketException &e) {
	cerr << e.what() << "\n";
	return false;
	}
	return true;
}
int main () {
	 cout << "running\n";
	string line;
	ifstream gpsdata;
	gpsdata.open(GPSFILE);
	cout << "started";
	if (gpsdata.is_open()) {
		while(getline(gpsdata,line)) {
			cout << "finding data";
			unsigned int found = line.find("$GPRMC");
			if (found != string::npos) {
				cout << "found data: " << line << endl;
				bool test = bcast(line);
				if (!test) {
					cerr << "FAIL: " << line << endl;
				}
			}
				sleep(1);
		}
	}
	else  {
	cerr << "failure to read file";
	}
}
