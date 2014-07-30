#ifndef _IDENT_H
#define _IDENT_H
#include <algorithm>
#include <iterator>
#include <string>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <fcntl.h>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <termios.h>
#include <ctime>
#include "PracticalSocket.h"
#include <curl/curl.h>
#include <fstream>

#define DEFAULTPORT 4451
#define FAIL 10 // Maximum # of failiures allowed
#define BLANKIP "0.0.0.0"
#define BUFSIZE 10000 // for modem and gps
#define SBUFSIZE 50 // for conf updates
#define ERRORSTRING "ERROR"
#define SERV_TIME 15
#define SUPDATE 3600 //5 minutes 30 seconds
#define WAITTIME 3600
#define BLANK "000"
#define ERRORSTRING "ERROR"
#define IDBUFSIZE 10000

#define USBDATA "/opt/usb"
#define LOCALIP "0.0.0.0 0.0.0.0 0.0.0.0"
#define LOCALPORT "04551 04551 04551"

using namespace std;

string ident();
void msleep(unsigned int seconds);
void device();
bool info();
extern string modem;
extern string gps;
extern string serport;
extern string sip;
extern string sport;
extern string id;
#endif
