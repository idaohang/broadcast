#define _VARS_
#include "bcast.h"

int main () {
	//sleep(10);
	cout << "\nBcast init\n";
	conf();
	checkinternet();
	int fail = 0;
	device();
	if (gps != "" && modem != "") id = ident();
	else ++fail;
	//string s_ip = DEFAULTIP;
	//unsigned short us_port = DEFAULTPORT;
	int gpsdata = -1;
	if (gps != "" && modem != "") {
		do {
			gpsdata = open(gps.c_str(), O_RDONLY | O_NOCTTY | O_NDELAY);
			if (gpsdata == -1) {
				perror("GPS OPEN");
			}
		} while(gpsdata == -1);
	}
	else ++fail;
	//cout << "GPSDATA: " << gpsdata << endl;
	fcntl(gpsdata,F_SETFL,0);
	if (id == ERRORSTRING) {
		cerr << "\nerror obtaining id\n";
	}
	time_t starttime;
	time(&starttime);
	time_t curtime;
	time(&curtime);
	double seconds;
	time_t ustarttime;
	time(&ustarttime);
	time_t ucurtime;
	time(&ucurtime);
	double useconds;
	vector<string> localip = updateip(localips);
	vector<unsigned short> localport = updateport(localports);
	vector<string> serverip;
	vector<unsigned short> serverport;
	bool first = true;
	int lnumip;
	int snumip;
	int rl = 0;
	char buf[BUFSIZE];
	checkinternet();
	rl = read(gpsdata,buf,BUFSIZE);
	lnumip = 0;
	snumip = 0;
	int readfail = 0;
	while(readfail < 10) {
		//cout << "Bcast loop\n";
		time(&ucurtime);
		useconds = difftime(ucurtime,ustarttime);
		//cout << "USeconds: " << useconds << endl;
		if (useconds >= SUPDATE || first) {
			cout << "\nUpdating Bcast conf\n";
			time(&ustarttime);
			if(info()) {
				serverip = updateip(sip);
				serverport = updateport(sport);
				cout << "\nBcast conf updated successfully\n" << endl;
				first = false;
			}
			else {
			cerr << "\nBcast conf update failed\n";
			++fail;
			}
		}
		string alldata(buf, rl);
		//cout << "all data from buf\n";
		string data = "000";
		//cout << "data init\n";
		if (!alldata.empty()) {
			//cout << "fill data";
			data = alldata.substr(0,alldata.length() - 1);
		}
		//cout << "\nGPS DATA: " << data << "\n";
		unsigned int found = data.find("$GPRMC");
		//cout << "find data\n";
		if (found == 0) {
			//cout << "\ndata found\n";
			lnumip++;
			//cout << lnumip << "\n";
			if (lnumip > 2) {
				lnumip = 0;
			}
			if (localip[lnumip] != BLANKIP) {
				bcast(data,id,localip[lnumip],localport[lnumip]);
			}
			time(&curtime);
			seconds = difftime(curtime,starttime);
			//cout << "Seconds: " << seconds << endl;
			if (snumip > 2) {
				snumip = 0;
			}
			if (seconds >= SERV_TIME) {
				checkinternet();
				//cout << "\nSERVER\n";
				time(&starttime);
				if(serverip[snumip] != BLANKIP) {
					cout << "Trying " << serverip[snumip] << endl;
					bool test = bcast(data,id,serverip[snumip],serverport[snumip]);
					if (!test) {
						++fail;
						cerr << "\nServer FAIL: " << fail << endl;
					}
					else {
						fail = 0;
					}
					++snumip;
				}
			}
		}
		if (fail == 5 || fail == 1) {
			++fail;
			system("ifdown eth2");
			sleep(1);
			system("ifup eth2");
			sleep(1);
		}
		if (fail >= FAIL) {
			cerr << "\nMaximum Failiure. Stopping\n";
			close(gpsdata);
			system("reboot");
			break;
		}
		msleep(10);
		rl = read(gpsdata,buf,BUFSIZE);
		if (rl < 1) {
			perror("GPS READ");
			++readfail;
		}
		else readfail = 0;
		if (readfail > 0) {
			close(gpsdata);
			gpsdata = open(gps.c_str(), O_RDONLY | O_NOCTTY | O_NDELAY);
			if (gpsdata == -1) {
				perror("Open GPS Again");
				system("reboot");
			}
		}
		//cout << "\n";
	}
	cerr << "\nModem has been unplugged or butchered by software. Rebooting\n";
	if(gpsdata > 0) close(gpsdata);
	system("reboot");
	return 0;
}
