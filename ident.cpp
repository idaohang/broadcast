#include "ident.h"
string ident () {
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
