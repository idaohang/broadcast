#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include "PracticalSocket.h"
using namespace std;

#define HOST "192.168.43.255"
#define HOST_PORT 4551
#define GPSFILE "/dev/ttyUSB0"

int main () {
	string line;
	ifstream gpsdata;
	gpsdata.open(GPSFILE);
	while(getline(gpsdata,line)) {
		found = line.find("$GPRMC");
		if (found != string::npos) {
			cout << "found data: " << line;
			try {
    				UDPSocket sock;
      				sock.sendTo(line, strlen(line), HOST, HOST_PORT);
  			}
			 catch (SocketException &e) {
    				cerr << e.what() << endl;
    			}
		}
			sleep(0.1);
	}
}
