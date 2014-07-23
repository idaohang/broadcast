#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include "ident.h"
#include "http_fetcher.h"
using namespace std;
#define BUFLEN 100000
#define BLANK "000"
#define CONFLOC "/opt/bcast.conf"
#define WAITTIME 3600
string place = "/tr.aspx?M=" + ident();
char* loc = place.c_str();
int fd;
string data = BLANK;
string connectto(const char* addr) {
	const char** buf;
	int rb = http_fetch(addr,buf);
	if (rb  == -1) {
		const char* errors;
		errors = http_strerror();
		cerr << errors;
		return BLANK;
	}
	else {
		string str(buf, rb - 1);
		return str;
	}
}
int main () {
	cout << "I Exist!\n";
	const char* addra = strcat("droid.taxitron.net",loc);
	const char* addrb = strcat("droid.taxitron.com",loc);
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
