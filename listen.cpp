#include <iostream>
#include <string>
#include "PracticalSocket.h"
#define BUFSIZE 10000
using namespace std;

int main() {
	const unsigned short localport = 4551;
	string foreignaddr = "192.168.96.51";
	unsigned short foreignport = 4551;

	UDPSocket sock(localport);
	while (true) {
		cout << "Listening on " << localport << "\n";
		cout << "Listening for " << foreignaddr << ":" << foreignport << "\n";
		char buf[BUFSIZE];
		sock.recvFrom(buf,BUFSIZE,foreignaddr,foreignport);
		cout << "\n[" << buf << "]\n";
	}
	cerr << "stopped\n";
	return 0;
}
