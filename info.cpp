#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include "ident.h"
#include "PracticalSocket.h"
#define ADDRA "http://droid.taxitron.net/tr.aspx?M="
#define ADDRB "http://droid.taxitron.com/tr.aspx?M="
using namespace std;

int main () {
	string id = ident();
	TCPSocket socka(ADDRA""id);
	TCPSocket sockb(ADDRB""id);
	return 0;
}
