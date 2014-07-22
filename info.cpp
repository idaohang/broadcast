#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include "ident.h"
#include "PracticalSocket.h"
#define ADDRA ""
#define ADDRB ""
using namespace std;

int main () {
	string id = ident();
	TCPSocket socka();
	TCPSocket sockb();
	socka.connect();
	return 0;
}
