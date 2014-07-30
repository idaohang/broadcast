#include "ident.h"
void device () {
	int usb = open(USBDATA, O_RDONLY);
	if (usb == -1) {
		perror("usb open");
	}
	char buf[IDBUFSIZE];
	int rb = read(usb,buf,IDBUFSIZE);
	close(usb);
	string data(buf,rb);
	istringstream ss(data);
	string line;
	while (getline(ss, line)) {
		//cout << "getline: " << line << endl;
		istringstream is(line);
		string x;
		vector<string> v;
		while(is >> x) {
			//cout << x << "\n";
			v.push_back(x);
		}

		/*
		id0 num0 dr0
		id1 num1 dr1
		id2 num2 dr2
		id3 num3 dr3
		*/
		//cout << "Output " << v[0] << " " << v[1] << " " << v[2] << endl;
		if(v[2] == "GobiSerial") { 
			if (v[1] == "02") {
				gps = v[0];
			}
			else if (v[1] == "03") {
				modem = v[0];
			}
		}
		else {
			serport = v[0];
		}
	}
	if (gps.empty() || modem.empty()) {
		cerr << "Gobi identification failed\n";
	}
	cout << "GPS: " << gps << endl;
	cout << "modem: " << modem << endl;
}

string ident () {
	sleep(20);
	int modemfile;
	do {
		modemfile = open(modem.c_str(), O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
		if (modemfile == -1) {
			cerr << "\nModem not open\n";
			perror("MODEM OPEN");
			sleep(5);
		}
	} while(modemfile == -1);
	cout << "\nmodem open\n";
	fcntl(modemfile, F_SETFL, 0);
	char inf [ ] = "ATI1\r\n";
	ssize_t wb = write(modemfile,inf,6);
	if (wb < 4) {
		cerr << "\nError writing modemfile\n";
	}
	char buf[IDBUFSIZE];
	sleep(1);
	int i;
	int ii = read(modemfile,buf,IDBUFSIZE);
	while(ii > 0) { 
		string raw(buf, ii);
		unsigned int found = raw.find("MEID:");
		if (found != string::npos) {
			string data = raw.substr(found+6, string::npos);
			//cout << "Raw: " << raw << endl;
			cout << "\nID:" << data;
			close(modemfile);
			return data.substr(0,data.length() - 1);
		}
		i++;
		ii = read(modemfile,buf,IDBUFSIZE);
	}
	return ERRORSTRING;
}

void msleep(unsigned int seconds) {
	usleep(seconds * 1000); //takes microseconds
}
