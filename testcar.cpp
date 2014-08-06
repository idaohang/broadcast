#define _VARS_
#include "bcast.h"
int main () {
	serport = "/dev/ttyUSB0";
	cout << "CI: " << carinit();
	while (1)
		sleep(1);
	//cout << "Car Info\n" << carinfo() << endl;
	return 0;
}
