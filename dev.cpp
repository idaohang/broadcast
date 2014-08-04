#include "ident.h"

void device () {
	system("/usr/local/bin/broadcast");
	char devname[30];
	char inumber[4];
	char driver[30];
	ifstream filein(USBDATA);
	while(!filein.getline(devname,30,',').eof()) {
		filein.getline(inumber,4,',');
		filein.getline(driver,30);
		//cout << "Output (" << driver << ") (" << inumber << ") (" << devname << ")" << endl;

		if(strcmp(driver, "GobiSerial") == 0) { 
			if (strcmp(inumber, "02") == 0) {
				gps = devname;
			}
			else if (strcmp(inumber, "03") == 0) {
				modem = devname;
			}
		}
		else {
			serport = devname;
		}
	}
	if (gps.empty() || modem.empty()) {
		cerr << "Gobi identification failed\n";
	}
	cout << "GPS: " << gps << endl;
	cout << "modem: " << modem << endl;
}
