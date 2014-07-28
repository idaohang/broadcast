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

const int perm = O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK;

//returns location of GPS device for other functions
int device () {
	int test = open("/dev/ttyUSB0",perm);
	char buf[IDBUFSIZE];
	int rb = read(test,buf,IDBUFSIZE);
	if (rb > 0) {
		//is this the gps?
		string data(buf,rb);
		unsigned int found = data.find("$");
		if (found != string::npos) return 0;
	}
	else if (rb == -1) {
		//something went wrong
		perror("Open ttyUSB0");
	}
	close(test);

	int test1 = open("/dev/ttyUSB1", perm);
	char buf1[IDBUFSIZE];
	int rb1 = read(test1,buf1,IDBUFSIZE);

	if (rb1 >  0) {
		string data1(buf1,rb1);
		unsigned int found1 = data1.find("$");
		if (found1 != string::npos) return 1;
	}
	else if (rb1 == -1) perror("Open ttyUSB1");
	close(test1);

	int test2 = open("/dev/ttyUSB2", perm);
	char buf2[IDBUFSIZE];
	int rb2 = read(test,buf,IDBUFSIZE);

	if (rb2 > 0) {
		string data2(buf2,rb2);
		unsigned int found2 = data2.find("$");
		if (found2 != string::npos) return 2;
	}
	else if (rb == -1) perror("Open ttyUSB2");
	close(test2);
	//Should never get here
	cerr << "No Operational Netgear AirCard 341U found\n";
	return -1;
}
int GPSFile() {
	int  dev = device();
	if (dev == 0) return open("/dev/ttyUSB0",perm);
	else if (dev == 1) return open("/dev/ttyUSB1",perm);
	else if (dev == 2) return open("/dev/ttyUSB2",perm);
	else return -1;
}
int ModemFile () {
	int dev = device();
	if (dev == 0) return open("/dev/ttyUSB1",perm);
	else if (dev == 1) return open("/dev/ttyUSB2",perm);
	else if (dev == 2) return open("/dev/ttyUSB3",perm);
	else return -1;
}

string ident () {
	//sleep(30);
	int modem;
	do {
		modem = ModemFile();
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
const char* c_ident () {
	int modem = ModemFile();
	if (modem == -1) {
		cerr << "\nError opening modem\n"; 
		return ERRORSTRING;
	}
	else {
		fcntl(modem, F_SETFL, 0);
	}
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
			cout << "\nID:" << data;
			close(modem);
			return data.c_str();
		}
		i++;
		ii = read(modem,buf,IDBUFSIZE);
	}
	return ERRORSTRING;
}
void msleep(unsigned int seconds) {
	usleep(seconds * 1000); //takes microseconds
}
