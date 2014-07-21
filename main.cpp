#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include "PracticalSocket.h"
using namespace std;

#define GPSFILE "/dev/ttyUSB0"
#define MODEMFILE "/dev/ttyUSB1"
#define BUFSIZE 500
#define TIMEOUT 5000
#define ERRORSTRING "ERROR"
const string host = "192.168.96.204";
unsigned short host_port = 4551;

string ident () {
	//not sure how to get id
	return ERRORSTRING;
}
bool bcast (string line) {
	try {
		UDPSocket sock;
		cout << "trying udp\n";
		string data = line + "::" + ident();
		sock.sendTo(data.c_str(),data.size(), host, host_port);
	}
	catch (SocketException &e) {
	cerr << e.what() << "\n";
	return false;
	}
	cout << "success?\n";
	return true;
}
int main () {
	cout << "running\n";
	string line;
	
	cout << "started\n";
	if (ret != 1) {
		cerr << "error opening file\n";
	}
	cout << "Opened serial port\n";
	while(gpsdata.ReadString(buf, '$', BUFSIZE, TIMEOUT) > 0) {
		cout << "finding data\n";
		string data(buf);
		unsigned int found = data.find("GPRMC");
		if (found != string::npos) {
			cout << "found data: " << data << endl;
			bool test = bcast(data);
			if (!test) {
				cerr << "FAIL: " << data << endl;
			}
		}
		sleep(1);
	}
	cerr << "End of file?\n";
	gpsdata.Close();
	return 0;
}
