#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <curl/curl.h>
#include "ident.h"
using namespace std;
#define BUFLEN 100000
#define BLANK "000"
#define CONFLOC "log.log"
#define WAITTIME 3600
string loc = "/tr.aspx?M=" + ident();
size_t write_callback(char *ptr, size_t size, size_t nmemb, string *userdata) {
	cout << "New Data: " << ptr << endl;
	string newstr(ptr);
	cout << "New string: " << newstr << endl;
	*userdata = newstr;
	cout << "All data: " << *userdata << endl;
	if (!newstr.empty()) {
		return sizeof(size*nmemb);
	}
	else {
		return 0;
	}
}
string finder (string tag, string data) {
	signed int startpoint = data.find("/><" + tag + ">");
	signed int endpoint = data.find("</" + tag + "><br");
	string result;
	if (startpoint  == -1 || endpoint == -1) {
		cerr << "Could not find " << tag << endl;
	}
	else {
		cout << tag << " Start Point: " << startpoint << endl;
		cout << tag << " End Point: " << startpoint << endl;
		result = data.substr(startpoint,endpoint);
		cout << tag << ": " << result << endl;
	}
	return result;
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
	cout << "To the loop!\n";
	curl_global_init(CURL_GLOBAL_NOTHING);
	cout << "init curl\n";
	CURL *handle = curl_easy_init();
	curl_easy_setopt(handle,CURLOPT_URL,addra);
	string data;
	string* buf = &data;
	cout << "Made buf\n";
	curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,write_callback);
	curl_easy_setopt(handle,CURLOPT_WRITEDATA,buf);
	curl_easy_perform(handle);
	cout << "curl_easy_perform\n";
	curl_easy_cleanup(handle);
	curl_global_cleanup();
	cout << "cleanup\n";
	//process data
	cout << "Yay we have: " << data << endl;
	//check addrb?

	//find TCP 1, 2, and 3
	string tcp1 = finder("TCP1",data);
	string tcp2 = finder("TCP2",data);
	string tcp3 = finder("TCP3",data);
	string tcp = tcp1+","+tcp2+","+tcp3+";";
	cout << "TCP: " << tcp << "\nTCP Length: " << tcp.length() << "\n";
	ofstream conf;
	conf.open(CONFLOC, ofstream::out | ofstream::trunc);
	conf.write(tcp.c_str(),tcp.length());
	return 0;
}
