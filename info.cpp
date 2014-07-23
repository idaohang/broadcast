#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include "ident.h"
using namespace std;
#define BUFLEN 100000
#define BLANK "000"
#define CONFLOC "/opt/bcast.conf"
#define WAITTIME 3600
string loc = "/tr.aspx?M=" + ident();
int fd;
string data = BLANK;
int connectto(const char* addr) {
	string place = addr+loc+" > "+CONFLOC;
	char* curl = strcat("curl ",place.c_str());
	system(curl);
	return fd;
}
int main () {
	cout << "I Exist!\n";
	const char* addra = "droid.taxitron.net";
	const char* addrb = "droid.taxitron.com";
	cout << "\nADDRA: " << addra << endl;
	cout << "\nADDRB: " << addrb << endl;
	cout << "loc: " << loc << endl;
	bool fail = true;
	string data = BLANK;
	cout << "To the loop!\n";
	while (fail) {
		data = connectto(addra);
		if (data != BLANK) {
			fail = false;
		} else {
			data = connectto(addrb);
			if (data != BLANK) {
				fail = false;
			}
		}
	}
	//process data
	cout << "Yay we have: " << data << endl;
	return 0;
}
