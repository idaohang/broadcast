#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
using namespace std;
#define GPSFILE "/dev/ttyUSB0"

const string host = "192.168.43.255";
unsigned short host_port = 4551;

bool bcast (string line) {

}
int main () {
	string line;
	ifstream gpsdata;
	gpsdata.open(GPSFILE);
	while(getline(gpsdata,line)) {
		int found = line.find("$GPRMC");
		if (found != string::npos) {
			cout << "found data: " << line;
			bool test = bcast(line);
			if (!test) {
				cerr << "FAIL: " << line;
			}
		}
			sleep(1);
	}
}
