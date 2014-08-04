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
#define CONFFILE "/opt/broadcast.conf"
using namespace std;

//ident.cpp
string ident();
void msleep(unsigned int seconds);

//info.cpp
size_t write_callback(char *ptr, size_t size, size_t nmemb, string *userdata);
string getdata(const char* addr);
string finder (string tag, string data);
bool info();

//dev.cpp
void device();

//conf.cpp
bool conf();

//main.cpp
bool bcast (string line,string id,string &hostip, unsigned short destport);
vector<string> updateip (string data);
vector<unsigned short> updateport (string data);


#ifdef _VARS_

#define _EXTERN_

#else

#define _EXTERN_ extern

#endif

_EXTERN_ string modem;
_EXTERN_ string gps;
_EXTERN_ string serport;
_EXTERN_ string sip;
_EXTERN_ string sport;
_EXTERN_ string id;
_EXTERN_ string meterstatus;
_EXTERN_ string addressa;
_EXTERN_ string addressb;
_EXTERN_ string localips;
_EXTERN_ string localports;
#endif
