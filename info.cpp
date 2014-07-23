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
string connectto(const char* addr) {
//idk
return BLANK;
}
int main () {
	cout << "I Exist!\n";
	string saddra = "droid.taxitron.net"+loc;
	string saddrb = "droid.taxitron.com"+loc;
	const char* addra = saddra.c_str();
	const char* addrb = saddrb.c_str();
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
