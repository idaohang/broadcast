#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <fcntl.h>
#include <sstream>
#include <vector>
using namespace std;
#define ERRORSTRING "ERROR"
#define IDBUFSIZE 10000
//#define MODEMFILE "/dev/ttyUSB2"
//#define PERM = O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK;
#define USBDATA "/opt/usb"
//returns location of GPS device for other functions
int device () {
	int usb = open(USBDATA, O_RDONLY);
	if (usb == -1) {
		perror("usb open");
		return -1;
	}
	char buf[IDBUFSIZE];
	vector<string> v;
	int rb = read(usb,buf,IDBUFSIZE);
	while (rb > 0) {
		string data(buf,rb);
		istringstream ss(data);
		string x;
		while(ss >> x) {
			v.push_back(x);
		}
		rb = read(usb,buf,IDBUFSIZE);
	}
	/*
	id0 num0 dr0
	id1 num1 dr1
	id2 num2 dr2
	id3 num3 dr3
	*/
	if((v[3] == "GobiSerial" && v[2] == "02") && (v[6] == "GobiSerial" && v[5] == "03")) {
		//the gps is ttyUSB0 modem is 1
		return 0;
	}
	if((v[5] == "GobiSerial" && v[4] == "02") && (v[8] == "GobiSerial" && v[7] == "03")) {
		//gps is ttyUSB1 modem is 2
		return 1;
	}
	if((v[8] == "GobiSerial" && v[7] == "02") && (v[12] == "GobiSerial" && v[11] == "03")) {
		//the gps is ttyUSB2, modem is 3
		return 2;
	}
	if(v[12] == "GobiSerial" && v[11] == "03") {
		//gps is 3 I hope modem is 4
		return 3;
	}
	else {
		cerr << "\nAutodetection failed\n";
		return 0;
	}
}
string GPSFile() {
	int  dev = device();
	if (dev == 0) return "/dev/ttyUSB0";
	else if (dev == 1) return "/dev/ttyUSB1";
	else if (dev == 2) return "/dev/ttyUSB2";
	else if (dev == 3) return "/dev/ttyUSB3";
	else if (dev == -1) return "-1";
	else return "0";
}
string ModemFile () {
	int dev = device();
	if (dev == 0) return "/dev/ttyUSB1";
	else if (dev == 1) return "/dev/ttyUSB2";
	else if (dev == 2) return "/dev/ttyUSB3";
	else if (dev == 3) return "/dev/ttyUSB4";
	else if (dev == -1) return "-1";
	else return "0";
}
string ident () {
	sleep(20);
	int modem;
	do {
		modem = open(ModemFile().c_str(), O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
		if (modem == -1) {
			cerr << "\nModem not open\n";
			perror("MODEM OPEN");
			sleep(5);
		}
	} while(modem == -1);
	cout << "\nmodem open\n";
	fcntl(modem, F_SETFL, 0);
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
			//cout << "Raw: " << raw << endl;
			cout << "\nID:" << data;
			close(modem);
			return data.substr(0,data.length() - 1);
		}
		i++;
		ii = read(modem,buf,IDBUFSIZE);
	}
	return ERRORSTRING;
}

void msleep(unsigned int seconds) {
	usleep(seconds * 1000); //takes microseconds
}
