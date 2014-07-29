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
//#define MODEMFILE "/dev/ttyUSB2"
//#define PERM = O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK;

//returns location of GPS device for other functions
int device () {
	sleep(1);
	int test = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	cout << "ttyUSB0: " << test << endl;
	char buf[IDBUFSIZE];
	int rb = read(test,buf,IDBUFSIZE);
	if (rb > 0) {
		//is this the gps?
		string data(buf,rb);
		unsigned int found = data.find("$");
		if (found != string::npos)  {
			close(test);
			return 0;
		}
	}
	else if (rb == -1) {
		//something went wrong
		perror("Open ttyUSB0");
	}
	close(test);

	int test1 = open("/dev/ttyUSB1", O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	cout << "ttyUSB1: " << test1 << endl;
	char buf1[IDBUFSIZE];
	int rb1 = read(test1,buf1,IDBUFSIZE);

	if (rb1 >  0) {
		string data1(buf1,rb1);
		unsigned int found1 = data1.find("$");
		if (found1 != string::npos) {
			close(test1);
			return 1;
		}
	}
	else if (rb1 == -1) perror("Open ttyUSB1");
	close(test1);

	int test2 = open("/dev/ttyUSB2", O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	cout << "ttyUSB2: " << test2 << endl;
	char buf2[IDBUFSIZE];
	int rb2 = read(test,buf,IDBUFSIZE);

	if (rb2 > 0) {
		string data2(buf2,rb2);
		unsigned int found2 = data2.find("$");
		if (found2 != string::npos) {
			close(test2);
			return 2;
		}
	}
	else if (rb == -1) perror("Open ttyUSB2");
	close(test2);
	//Should never get here
	cerr << "No Operational Netgear AirCard 341U found\n";
	return -1;
}
string GPSFile() {
	int  dev = device();
	if (dev == 0) return "/dev/ttyUSB0";
	else if (dev == 1) return "/dev/ttyUSB1";
	else if (dev == 2) return "/dev/ttyUSB2";
	else if (dev == -1) return "-1";
	else return "0";
}
string ModemFile () {
	int dev = device();
	if (dev == 0) return "/dev/ttyUSB1";
	else if (dev == 1) return "/dev/ttyUSB2";
	else if (dev == 2) return "/dev/ttyUSB3";
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
