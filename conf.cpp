#include "bcast.h"
bool conf() {
	ifstream conffile(CONFFILE);
	if (conffile.is_open()) {
		string buf;
		vector<string> v;
		while (conffile >> buf) {
			//cout << "(" << buf << ")" << endl;
			v.push_back(buf);
		}
		addressa = v[0];
		addressb = v[1];
		localips = v[2] + " " + v[3] + " " + v[4];
		localports = v[5] + " " + v[6] + " " + v[7];
		pingip = v[8];
		cout << "\nSuccessfully updated local conf\n";
		cout << "IPs: " << localips << endl;
		cout << "Ports: " << localports << endl;
		cout << "Ping IP: " << pingip << endl;
		return true;
	}
	else {
		cerr << "\nFailed to open local conf\n";
		return false;
	}
}


