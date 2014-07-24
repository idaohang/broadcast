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
#define IPCONF "/opt/broadcast.sip.conf"
#define PORTCONF "/opt/broadcast.sport.conf"
#define WAITTIME 3600
string loc = "/tr.aspx?M=" + ident();
size_t write_callback(char *ptr, size_t size, size_t nmemb, string *userdata) {
	//cout << "New Data: " << ptr << endl;
	string newstr(ptr);
	//cout << "New string: " << newstr << endl;
	*userdata = newstr;
	//cout << "All data: " << *userdata << endl;
	if (!newstr.empty()) {
		return sizeof(size*nmemb);
	}
	else {
		return 0;
	}
}
string getdata(const char* addr) {
	curl_global_init(CURL_GLOBAL_NOTHING);
	//cout << "init curl\n";
	CURL *handle = curl_easy_init();
	curl_easy_setopt(handle,CURLOPT_URL,addr);
	string data;
	string* buf = &data;
	//cout << "Made buf\n";
	curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,write_callback);
	curl_easy_setopt(handle,CURLOPT_WRITEDATA,buf);
	curl_easy_perform(handle);
	//cout << "curl_easy_perform\n";
	curl_easy_cleanup(handle);
	curl_global_cleanup();
	//cout << "cleanup\n";
	//process data
	//cout << "Yay we have: " << data << endl;
	return data;

}
string finder (string tag, string data) {
	signed int startpoint = data.find("<" + tag + ">");
	signed int endpoint = data.find("</" + tag + ">");
	string result;
	if (startpoint  == -1 || endpoint == -1) {
		cerr << "Could not find " << tag << endl;
	}
	else {
		//cout << tag << " Start Point: " << startpoint << endl;
		//cout << tag << " End Point: " << endpoint << endl;
		result = data.substr(startpoint+6,endpoint-(startpoint+6));
		//cout << tag << ": " << result << endl << endl;
	}
	return result;
}
int main () {
	cout << "I Exist!\n";
	//sleep(300); //5 minutes
	string saddra = "droid.taxitron.net"+loc;
	string saddrb = "droid.taxitron.com"+loc;
	const char* addra = saddra.c_str();
	const char* addrb = saddrb.c_str();
	//cout << "\nADDRA: " << addra << endl;
	//cout << "\nADDRB: " << addrb << endl;
	//cout << "loc: " << loc << endl;
	//cout << "To the loop!\n";
	bool dostuff = true;
	while(dostuff) {
		string data;
		data = getdata(addra);
		//cout << "DATA: " << data << endl;
		if(data.empty()) {
			data = getdata(addrb);
			//cout << "DATA: " << data << endl;
		}
		if(data.empty()) {
			cerr << "\nNo data available. Retrying";
		}
		if (!data.empty()) {
			//find TCP 1, 2, and 3
			//this needs to be a function...

			string tcp1 = finder("TCP1",data);
			signed int tcp1colon = tcp1.find(":");
			string tcp1ip = tcp1.substr(0,tcp1colon-1);
			string tcp1port = tcp1.substr(tcp1colon+1);

			string tcp2 = finder("TCP2",data);
			signed int tcp2colon = tcp2.find(":");
			string tcp2ip = tcp2.substr(0,tcp2colon-1);
			string tcp2port = tcp2.substr(tcp2colon+1);

			string tcp3 = finder("TCP3",data);
			signed int tcp3colon = tcp3.find(":");
			string tcp3ip = tcp3.substr(0,tcp3colon-1);
			string tcp3port = tcp3.substr(tcp3colon+1);

			string ips = tcp1ip + " " + tcp2ip + " " + tcp3ip + ";";
			string ports = tcp1port + " " + tcp2port + " " + tcp3port + ";";
			cout << "ips: " << ips << "\n";
			cout << "ports" << ports << "\n";
			ofstream sip;
			sip.open(IPCONF, ofstream::out | ofstream::trunc);
			sip.write(ips.c_str(),ips.length());

			ofstream sport;
			sport.open(PORTCONF, ofstream::out | ofstream::trunc);
			sport.write(ports.c_str(),ports.length());

			dostuff = false;
		}
		//cout << "\n\n";
	}
	return 0;
}
