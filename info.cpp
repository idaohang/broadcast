#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include "ident.h"
#include "PracticalSocket.h"
using namespace std;

int main () {
	string id = ident();
	
	return 0;
}
