#define _VARS_
#include "ident.h"

UDPSocket sock;
//send data plus identifying info to host at host port
bool bcast (string line,string id,string &hostip, unsigned short destport) {
	try {
		string data = line + "," + id;
		sock.sendTo(data.c_str(),data.size(), hostip, destport);
		//cout << ":" << sock.getLocalPort() << endl;

	}
	catch (SocketException &e) {
	cerr << endl << e.what() << endl;
	return false;
	}
	sock.disconnect();
	return true;
}

vector<string> updateip (string data) {
	vector<string> v;
	istringstream ss(data);
	string x;
	while(ss >> x) {
		//cout << x << "\n";
		v.push_back(x);
	}
	/*
	cout << "v 0 " << v[0] << "\n";
	cout << "v 1 " << v[1] << "\n";
	cout << "v 2 " << v[2] << "\n";
	*/
	return v;
}
vector<unsigned short> updateport (string data) {
	//cout << "\nUpdating port\n";
	istringstream ss(data);
	vector<unsigned short> v;
	string x;
	while(ss >> x) {
		//cout << x << endl;
		stringstream str(x);
		unsigned short portshort;
		str >> portshort;
		//cout << portshort << "\n";
		v.push_back(portshort);
	}
	/*
	for (int i = 0; i < 3; ++i) {
		cout << "V[" << i << "]: " << v[i] << "\n";
	}
	*/
	return v;
}
int main () {
	if (!conf()) exit(1);
	device();
	id = ident();
	//string s_ip = DEFAULTIP;
	//unsigned short us_port = DEFAULTPORT;
	int gpsdata;

	do {
		gpsdata = open(gps.c_str(), O_RDONLY | O_NOCTTY | O_NDELAY | O_NONBLOCK);
		if (gpsdata == -1) {
			perror("GPS OPEN");
		}
	} while(gpsdata == -1);
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
	int fail = 0;
	int lnumip;
	int snumip;
	int rl = 0;
	char buf[BUFSIZE];
	rl = read(gpsdata,buf,BUFSIZE);
	lnumip = 0;
	snumip = 0;
	while(rl > 0) {
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
			system("ifdown eth1");
			sleep(1);
			system("ifup eth1");
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
		//cout << "\n";
	}
	cerr << "\nModem has stopped transmitting data\n";
	close(gpsdata);
	return 0;
}
