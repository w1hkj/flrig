#ifndef _RIG_IO_H
#define _RIG_IO_H

#include <cstring>
#include <cmath>

#include <FL/Fl.H>

using namespace std;

enum MODES {LSB, USB, CW};

enum BAUDS {
	BR300, BR600, BR1200, BR2400, BR4800, BR9600, BR19200, 
	BR38400, BR57600, BR115200, BR230400, BR460800 };

#define RXBUFFSIZE 16384

extern const char *szBaudRates[];

extern bool startXcvrSerial();
extern bool startAuxSerial();
extern bool startSepSerial();

extern int readResponse();
extern int sendCommand(string s, int nread = -1);
extern int waitResponse(int);

extern void clearSerialPort();
extern int rig_nbr;
extern char replybuff[];
extern string replystr;

#endif
