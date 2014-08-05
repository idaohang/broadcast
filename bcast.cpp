#include "bcast.h"
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
void checkinternet() {
	bool notworking = true;
	while(notworking) {
		int ping = system("ping -c 1 -s 1 24.248.166.1 > /dev/null");
		if (ping > 0) {
			cerr << "\nNetwork misconfigured. Trying to recover...\n";
			system("ifdown eth2");
			sleep(2);
			system("ifup eth2");
			sleep(1);
		}
		else {
			notworking = false;
			//cout << "\nNetwork is operational\n";
		}
	}
}
