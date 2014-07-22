#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include "ident.h"
#include "PracticalSocket.h"
using namespace std;
#define BUFLEN 100000

int main () {
	cout << "I Exist!\n";
	string id = ident();
	const string addra = "droid.taxitron.net";
	const string addrb = "droid.taxitron.com";
	const string loc = "/tr.aspx?M=" + id;
	cout << "\nADDRA: " << addra << endl;
	cout << "\nADDRB: " << addrb << endl;
	cout << "loc: " << loc << endl;
	TCPSocket socka(addra,80);
	TCPSocket sockb(addrb,80);
	bool fail = true;
	//Make request here
	cout << "To the loop!\n";
	while (fail) {
		char buf[BUFLEN];
		int rb;
		try {
			cout << "Trying AddrA\n";
			rb = socka.recv(buf,BUFLEN);
			if (!(rb > 0)) {
				fail = false;
			}

		} catch(SocketException &e) {
			cerr << e.what() << endl;
			msleep(100);
			try {
				cout << "Trying AddrB\n";
				rb = sockb.recv(buf,BUFLEN);
				fail = false;
			} catch (SocketException &e) {
				cerr << e.what() << endl;
				if (!(rb > 0)) {
					fail = false;
				}

			}
		}
		string data(buf,BUFLEN);
		cout << "\nDATA FOUND: " << buf << endl;
	}
	return 0;
}
