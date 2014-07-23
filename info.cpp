#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include "ident.h"
#include <curl/curl.h>
using namespace std;
#define BUFLEN 100000
#define BLANK "000"
#define CONFLOC "/opt/bcast.conf"
#define WAITTIME 3600
string loc = "/tr.aspx?M=" + ident();
string data = BLANK;
string connectto(const char* addr) {
	curl_global_init(CURL_GLOBAL_NOTHING);
	CURL *handle = curl_easy_init();
	curl_easy_setopt(handle,CURLOPT_URL,addr);
	char buf[BUFLEN];
	curl_easy_setopt(handle,CURLOPT_WRITEDATA,buf);
	curl_easy_perform(handle);
	curl_easy_cleanup(handle);
	curl_global_cleanup();
	return "  ";
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
