#include "bcast.h"

int carfile = -1;
bool ready_to_receive = false;

void carread() {
	int rb;
	char buf[BUFSIZE + 1];
	bool read_done = false;
	while  (!read_done) {
		sleep(1);
		memset(buf, 0, BUFSIZE + 1);
		rb = read(carfile, buf, BUFSIZE);
		//if (rb == -1) read_done = true; //return -1;
		//buf[rb] = 0;
		//if (buf[0] != 62) {
			cout << "DATA READ(" << rb << "): " << buf << endl;
			for (int i = 0; i < rb; i++) {
				cout << "[" << (int)buf[i] << "] ";
			}
			cout << endl;
		//}
		if ((buf[0] == 62) && (buf[1] == 10)){
			cout << "Have prompt\n";
			ready_to_receive = true;
		}
	}
	//return rb;
}

bool carinit() {
	termios oldt;
	termios newt;
	bool init = true;
	const char *initcom[5];
	initcom[1] = "ATZ\r\n";
	//initcom[2] = "010D\r\n";
	//initcom[3] = "ATRV\r\n";
	//initcom[4] = "ATDP\r\n";
	//initcom[2] = "ATSP00\r\n";
	//initcom[3] = "0902\r\n";
	//char alldata[BUFSIZE * 3 + 1];
	//assume serport is car
	cout << serport << endl;
	carfile = open(serport.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	thread car(carread);
	if (carfile == -1) {
		perror("Open Car");
		close(carfile);
		return false;
	}
	else {
		fcntl(carfile,F_SETFL,0);
		tcgetattr(carfile,&oldt);
		newt = oldt;
		speed_t ispeed = cfgetispeed(&oldt);
		speed_t ospeed = cfgetospeed(&oldt);
		cout << "In: " << ispeed << endl;
		cout << "Out: " << ospeed << endl;
		cfsetispeed(&newt,B38400);
		cfsetospeed(&newt,B38400);
		int test = tcsetattr(carfile,TCSANOW,&newt);
		if (test == -1) {
			cout << "\nFailed to set speed" << endl;
		}
		tcgetattr(carfile,&oldt);
		ispeed = cfgetispeed(&oldt);
		ospeed = cfgetospeed(&oldt);
		cout << "In: " << ispeed << endl;
		cout << "Out: " << ospeed << endl;
		sleep(1);
		//tcflush(carfile, TCIOFLUSH);
		int wb;
		wb = write(carfile, initcom[1], strlen(initcom[1]));
		for (int i = 1; i < 2; ++i) {
			//char initbuf[BUFSIZE];
			//char *initbuf;
			//int wb = carread((char *)initcom[i],strlen((char *)initcom[i]),&initbuf);
			while (!ready_to_receive)
				sleep(1);
			cout << "SENT: (" << initcom[i] << ")" << endl;
			wb = write(carfile, initcom[i], strlen(initcom[2]));
			ready_to_receive = false;
			if (wb < 0) {
				perror("Write car");
				return -1;
			}
			sleep(2);

			if (wb == -1) {
				perror("Write Car");
				//close(carfile);
				//return false;
			}
			else if(wb < 6) cout << "Bytes Written: " << wb << endl;
			else {
				cout << "\nCar Tracker successfully initialized: " << initcom[2] << endl;
				//close(carfile);
				//return true;
				init = false;
			}
		}
	}
	cout << "Done with loop\n";
	while(1) {
		sleep(1);
	}

	return init;
}
string carinfo() {
	char vincom[6] = "0902\r";
	char speedcom[6] = "010D\r";
	string vin = "VIN";
	string speed = "SPEED";
	char buf[BUFSIZE];
	//int rb;
	//int wb;
	if (carfile == -1) {
		perror("Open Carfile info");
		close(carfile);
		return "?";
	}
	else {
		//int rb = 
		//carread(vincom,6,buf);
		cout << buf << endl;
		string x(buf);
		vin = x;
		//rb = 
		//carread(speedcom,6,buf);
		cout << buf << endl;
		string y(buf);
		speed = y;
	}
	string retval = vin + "," + speed;
	return retval;
}

