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
// From http://stackoverflow.com/a/1494435
void Replace(std::string& str, const std::string& oldStr, const std::string& newStr) {
	size_t pos = 0;
	while((pos = str.find(oldStr, pos)) != std::string::npos) {
		str.replace(pos, oldStr.length(), newStr);
                pos += newStr.length();
	}
}

int main () {
	string line;
	ifstream gpsdata;
	gpsdata.open(GPSFILE);
	while(getline(gpsdata,line)) {
		Replace(line,","," ");
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
		}
			sleep(0.1);
	}
}
