#ifndef _IDENT_H
#define _IDENT_H
#include <string>
std::string ident();
void msleep(unsigned int seconds);
void device();
extern std::string modem;
extern std::string gps;
extern std::string serport;
#endif
