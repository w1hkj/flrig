#include <math.h>
#include <string>

#include <FL/Fl.H>

#include "rig.h"
#include "support.h"
#include "util.h"
#include "debug.h"
#include "status.h"
#include "rig_io.h"

using namespace std;

extern bool test;

int rig_nbr = 0;

const char *nuline = "\n";

static int iBaudRates[] = { 300, 600, 1200, 2400, 4800, 9600,
	19200, 38400, 57600, 115200, 230400, 460800 };
const char *szBaudRates[] = { "300", "600", "1200", "2400", "4800", "9600",
	"19200", "38400", "57600", "115200", "230400", "460800", NULL };

int BaudRate(int n)
{
	if (n > (int)sizeof(iBaudRates)) return 1200;
	return (iBaudRates[n]);
}


bool startXcvrSerial()
{
	bypass_serial_thread_loop = true;
// setup commands for serial port
	if (progStatus.xcvr_serial_port == "NONE") return false;

	RigSerial.Device(progStatus.xcvr_serial_port);
	RigSerial.Baud(BaudRate(progStatus.comm_baudrate));
	RigSerial.Stopbits(progStatus.stopbits);
	RigSerial.Retries(progStatus.comm_retries);
	RigSerial.Timeout(progStatus.comm_timeout);
	RigSerial.RTSptt(progStatus.comm_rtsptt);
	RigSerial.DTRptt(progStatus.comm_dtrptt);
	RigSerial.RTSCTS(progStatus.comm_rtscts);
	RigSerial.RTS(progStatus.comm_rtsplus);
	RigSerial.DTR(progStatus.comm_dtrplus);

	if (!RigSerial.OpenPort()) {
		LOG_ERROR("Cannot access %s", progStatus.xcvr_serial_port.c_str());
		return false;
	} else if (debug::level == debug::DEBUG_LEVEL) {
		LOG_DEBUG("\n\
Serial port:\n\
    Port     : %s\n\
    Baud     : %d\n\
	Stopbits : %d\n\
    Retries  : %d\n\
    Timeout  : %d\n\
    Loop     : %d\n\
    RTSCTS   : %d\n\
    CATptt   : %d\n\
    RTSptt   : %d\n\
    DTRptt   : %d\n\
    RTS+     : %d\n\
    DTR+     : %d\n",
			progStatus.xcvr_serial_port.c_str(),
			progStatus.comm_baudrate,
			progStatus.stopbits,
			progStatus.comm_retries,
			progStatus.comm_timeout,
			progStatus.serloop_timing,
			progStatus.comm_rtscts,
			progStatus.comm_catptt,
			progStatus.comm_rtsptt,
			progStatus.comm_dtrptt,
			progStatus.comm_rtsplus,
			progStatus.comm_dtrplus );
	}

	RigSerial.FlushBuffer();
//	bypass_serial_thread_loop = false;
	return true;
}

bool startAuxSerial()
{
	if (progStatus.aux_serial_port == "NONE") return false;

	AuxSerial.Device(progStatus.aux_serial_port);
	AuxSerial.Baud(1200);
	AuxSerial.RTS(progStatus.aux_rts);
	AuxSerial.DTR(progStatus.aux_dtr);

	if (!AuxSerial.OpenPort()) {
		LOG_ERROR("Cannot access %s", progStatus.aux_serial_port.c_str());
		return false;
	}
	return true;
}

bool startSepSerial()
{
	if (progStatus.sep_serial_port == "NONE") return false;

	SepSerial.Device(progStatus.sep_serial_port);
	SepSerial.Baud(1200);

	SepSerial.RTSCTS(false);
	SepSerial.RTS(progStatus.sep_rtsplus);
	SepSerial.RTSptt(progStatus.sep_rtsptt);

	SepSerial.DTR(progStatus.sep_dtrplus);
	SepSerial.DTRptt(progStatus.sep_dtrptt);

	if (!SepSerial.OpenPort()) {
		LOG_ERROR("Cannot access %s", progStatus.sep_serial_port.c_str());
		return false;
	}
	return true;
}

char replybuff[RXBUFFSIZE+1];
string replystr;

int readResponse()
{
	int numread = 0;
	replystr.clear();
	memset(replybuff, 0, RXBUFFSIZE + 1);
	numread = RigSerial.ReadBuffer(replybuff, RXBUFFSIZE);
//	LOG_DEBUG("rsp:%3d, %s", numread, str2hex(replybuff, numread));
	for (int i = 0; i < numread; replystr += replybuff[i++]);
	return numread;
}

int sendCommand (string s, int nread)
{
	int numwrite = (int)s.size();

//	LOG_DEBUG("cmd:%3d, %s", (int)s.length(), str2hex(s.data(), s.length()));
	if (RigSerial.IsOpen() == false) {
		replystr.clear();
		return 0;
	}

	if (progStatus.byte_interval == 0)
		RigSerial.WriteBuffer(s.c_str(), numwrite);
	else
		for (size_t i = 0; i < s.length(); i++) {
			RigSerial.WriteByte(s[i]);
			MilliSleep(progStatus.byte_interval);
		}

	MilliSleep( progStatus.comm_wait );

	if (nread == 0) {
		replystr.clear();
		return 0;
	}
	if (nread > 0)
		MilliSleep( (int)((nread + progStatus.comm_echo ? numwrite : 0)*11000.0/RigSerial.Baud()));

	return readResponse();

}

int waitResponse(int timeout)
{
	int n = 0;
	if (RigSerial.IsOpen() == false)
		return 0;
	MilliSleep(10);
	if (!(n = readResponse()))
		for (int t = 0; t <= timeout; t += 50) {
			if ((n = readResponse())) break;
			MilliSleep(50);
			Fl::awake();
		}
	return n;
}

void clearSerialPort()
{
	if (RigSerial.IsOpen() == false) return;
	RigSerial.FlushBuffer();
}
