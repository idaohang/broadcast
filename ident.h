#ifndef _IDENT_H
#define _IDENT_H
#include <string>
#define BUFSIZE 10000 // for modem and gps
#define SBUFSIZE 50 // for conf updates
#define ERRORSTRING "ERROR"
#define SERV_TIME 15
#define SUPDATE 3600 //5 minutes 30 seconds
#define LOCALIP "0.0.0.0 0.0.0.0 0.0.0.0"
#define LOCALPORT "04551 04551 04551"
std::string ident();
void msleep(unsigned int seconds);
void device();
bool info();
extern std::string modem;
extern std::string gps;
extern std::string serport;
extern std::string sip;
extern std::string sport;
#endif
