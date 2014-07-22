#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <fcntl.h>
using namespace std;
#define ERRORSTRING "ERROR"
#define IDBUFSIZE 10000
#define MODEMFILE "/dev/ttyUSB2"

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
	char buf[IDBUFSIZE];
	sleep(1);
	int i;
	int ii = read(modem,buf,IDBUFSIZE);
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
		ii = read(modem,buf,IDBUFSIZE);
	}
	return ERRORSTRING;
}
void msleep(unsigned int seconds) {
	usleep(seconds * 1000); //takes microseconds
}
