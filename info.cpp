#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include "ident.h"
#include "happyhttp.h"
using namespace std;
#define BUFLEN 100000
#define BLANK "000"
#define CONFLOC "/opt/bcast.conf"
const char* loc = "/tr.aspx?M=" + c_ident();
int fd;
string data = BLANK;
int count = 0;
//functions for connection
void OnBegin (const happyhttp::Response* r, void* userdata) {
	printf("Begin (%d,%s)",r->getstatus(),r->getreason());
	count = 0;
}
void OnData (const happyhttp::Response* r,const void* userdata, const unsigned char* buf, int n) {
	fread(buf, 1, n, stdout);
	string newstring(buf,n);
	data += newstring;
	++count;
}
void OnComplete () {
	cout << "Found data: " << data << endl;
}
string connectto(string addr) {
	happyhttp::Connection conn(addr,80);
	conn.setcallbacks(OnBegin,OnData,OnComplete);
	conn.request("GET", loc, 0, 0, 0);
	while(conn.outstanding()) {
		conn.pump();
	}
	return data;
}
int main () {
	cout << "I Exist!\n";
	const string addra = "droid.taxitron.net";
	const string addrb = "droid.taxitron.com";
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
