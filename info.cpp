#include "bcast.h"

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
	//cout << "Data: " << data << endl;
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
		result = data.substr(startpoint+6,endpoint-(startpoint+6));
	}
	return result;
}

bool info () {
	string loc = "/tr.aspx?M=" + id;
	string saddra = addressa+loc;
	string saddrb = addressb+loc;
	const char* addra = saddra.c_str();
	const char* addrb = saddrb.c_str();
		string data;
		data = getdata(addra);
		if(data.empty()) {
			data = getdata(addrb);
		}
		if(data.empty()) {
			cerr << "\nNo data available\n";
		}
		if (!data.empty()) {
			//find TCP 1, 2, and 3
			//this needs to be a function...
			string xml[] = { "TCP1", "TCP2", "TCP3" };
			string ips;
			string ports;
			for (int i = 0; i < 3; ++i) {
				string tcp = finder(xml[i], data);
				signed int tcpcolon = tcp.find(":");
				ips += tcp.substr(0,tcpcolon) + " ";
				ports += tcp.substr(tcpcolon+1) + " ";
			}

			/*

			string tcp2 = finder("TCP2",data);
			signed int tcp2colon = tcp2.find(":");
			string tcp2ip = tcp2.substr(0,tcp2colon);
			string tcp2port = tcp2.substr(tcp2colon+1);

			string tcp3 = finder("TCP3",data);
			signed int tcp3colon = tcp3.find(":");
			string tcp3ip = tcp3.substr(0,tcp3colon);
 			string tcp3port = tcp3.substr(tcp3colon+1);

			string ips = tcp1ip + " " + tcp2ip + " " + tcp3ip;
			string ports = tcp1port + " " + tcp2port + " " + tcp3port;
			*/
			cout << "\nIPs: " << ips << "\n";
			cout << "\nPorts: " << ports << "\n";
			if(ips.length() < 4 && ports.length() < 4) {
				cerr << "No IP's found. Am I Authorized?\n";
				return false;
			}
			else
			{
				sip = ips;
				sport = ports;
				/*
				ofstream sip;
				sip.open(IPCONF, ofstream::out | ofstream::trunc);
				sip.write(ips.c_str(),ips.length());
				ofstream sport;
			 	sport.open(PORTCONF, ofstream::out | ofstream::trunc);
				sport.write(ports.c_str(),ports.length());
				*/
				return true;
			}
		}
		//cout << "\n\n";
	return 0;
}
