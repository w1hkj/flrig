//-----------------------------------------------------------------------------
//
// Serial i/o class
//
// copyright Dave Freese 2006, w1hkj@w1hkj.com
//
//-----------------------------------------------------------------------------

#include <string>

#include "serial.h"
#include "debug.h"

LOG_FILE_SOURCE(debug::LOG_RIGCONTROL);


#ifndef __WIN32__
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include <memory>

using namespace std;

Cserial::Cserial() {
	device = "ttyS0";
	baud = 1200;
	timeout = 50; //msec
	retries = 5;
	rts = dtr = false;
	rtsptt = dtrptt = false;
	rtscts = false;
	status = 0;
	stopbits = 2;
	fd = -1;
}

Cserial::~Cserial() {
	ClosePort();
}



///////////////////////////////////////////////////////
// Function name	: Cserial::OpenPort
// Description	  : Opens the port specified by strPortName
// Return type	  : bool
// Argument		 : c_string strPortName
///////////////////////////////////////////////////////
bool Cserial::CheckPort(string dev)  {
	string devstr = "/dev/";
	devstr.append(dev);
	int testfd = open( devstr.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if (testfd < 0)
		return false;
	close(fd);
	return true;
}

///////////////////////////////////////////////////////
// Function name	: Cserial::OpenPort
// Description	  : Opens the port specified by strPortName
// Return type	  : bool
// Argument		 : c_string strPortName
///////////////////////////////////////////////////////
bool Cserial::OpenPort()  {

#ifdef __CYGWIN__
	com_to_tty(device);
#endif
	string devstr = "/dev/";
	devstr.append(device);
	if ((fd = open( devstr.c_str(), O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
		return false;
// save current port settings
	tcflush (fd, TCIFLUSH);

	tcgetattr (fd, &oldtio);
	newtio = oldtio;

	newtio.c_cflag &= ~CSIZE;
	newtio.c_cflag |= CS8 | CLOCAL | CREAD;

	newtio.c_cflag &= ~PARENB;

	if (stopbits == 1)
		newtio.c_cflag &= ~CSTOPB; // 1 stop bit
	else
		newtio.c_cflag |= CSTOPB; // 2 stop bit

	if (rtscts)
		newtio.c_cflag |= CRTSCTS;
	else
		newtio.c_cflag &= ~CRTSCTS;

	newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	newtio.c_oflag &= ~OPOST;

	newtio.c_iflag &= ~IXON;

	switch(baud) {
		case 300:
			speed = B300;
			break;
		case 1200:
			speed = B1200;
			break;
		case 2400:
			speed = B2400;
			break;
		case 4800:
			speed = B4800;
			break;
		case 9600:
			speed = B9600;
			break;
		case 19200:
			speed = B19200;
			break;
		case 38400:
			speed = B38400;
			break;
		case 57600:
			speed = B57600;
			break;
		case 115200:
			speed = B115200;
			break;
		default:
			speed = B1200;
	}
	cfsetispeed(&newtio, speed);
	cfsetospeed(&newtio, speed);

	tcsetattr (fd, TCSANOW, &newtio);

	ioctl(fd, TIOCMGET, &status);
	origstatus = status;

	if (dtr)
		status |= TIOCM_DTR;		// set the DTR bit
	else
		status &= ~TIOCM_DTR;	   // clear the DTR bit

	if (rtscts == false) {		  // rts OK for ptt if RTSCTS not used
		if (rts)
			status |= TIOCM_RTS;		// set the RTS bit
		else
			status &= ~TIOCM_RTS;	   // clear the RTS bit
	}
	ioctl(fd, TIOCMSET, &status);

	FlushBuffer();

	return true;
}

///////////////////////////////////////////////////////
// Function name	: Cserial::setPTT
// Return type	  : void
///////////////////////////////////////////////////////
void Cserial::SetPTT(bool ON)
{
	if (fd < 0) {
	  LOG_DEBUG("ptt fd < 0");
		return;
	}
	if (dtrptt || rtsptt) {
		LOG_INFO("PTT %d, DTRptt %d, DTR %d, RTSptt %d, RTS %d, RTSCTS %d",
			  ON, dtrptt, dtr, rtsptt, rts, rtscts);
		ioctl(fd, TIOCMGET, &status);
		if (ON) {								  // ptt enabled
			if (dtrptt && dtr)  status &= ~TIOCM_DTR;	 // toggle low
			if (dtrptt && !dtr) status |= TIOCM_DTR;	  // toggle high
			if (!rtscts) {
				if (rtsptt && rts)  status &= ~TIOCM_RTS; // toggle low
				if (rtsptt && !rts) status |= TIOCM_RTS;  // toggle high
			}
		} else {										  // ptt disabled
			if (dtrptt && dtr)  status |= TIOCM_DTR;	  // toggle high
			if (dtrptt && !dtr) status &= ~TIOCM_DTR;	 // toggle low
			if (!rtscts) {
				if (rtsptt && rts)  status |= TIOCM_RTS;  // toggle high
				if (rtsptt && !rts) status &= ~TIOCM_RTS; // toggle low
			}
		}
//	  LOG_INFO("Status %02X, %s", status & 0xFF, uint2bin(status, 8));
		ioctl(fd, TIOCMSET, &status);
	}
//	LOG_DEBUG("No ptt specified");
}

void Cserial::setRTS(bool b)
{
	if (fd < 0)
		return;

	ioctl(fd, TIOCMGET, &status);
	if (b == true) 
		status |= TIOCM_RTS;  // toggle high
	else 
		status &= ~TIOCM_RTS; // toggle low
	ioctl(fd, TIOCMSET, &status);

}

void Cserial::setDTR(bool b)
{
	if (fd < 0)
		return;

	ioctl(fd, TIOCMGET, &status);
	if (b == true)
		status |= TIOCM_DTR;	  // toggle high
	else
		status &= ~TIOCM_DTR;	 // toggle low
	ioctl(fd, TIOCMSET, &status);

}

///////////////////////////////////////////////////////
// Function name	: Cserial::ClosePort
// Description	  : Closes the Port
// Return type	  : void
///////////////////////////////////////////////////////
void Cserial::ClosePort()
{
	if (fd < 0) return;
	int myfd = fd;
	fd = -1;
//  LOG_INFO("Serial port closed, fd = %d", myfd);
	ioctl(myfd, TIOCMSET, &origstatus);
	tcsetattr (myfd, TCSANOW, &oldtio);
	close(myfd);
	fd = -1;
	return;
}

bool  Cserial::IOselect ()
{
	fd_set rfds;
	struct timeval tv;
	int retval;

	FD_ZERO (&rfds);
	FD_SET (fd, &rfds);
	tv.tv_sec = timeout/1000;
	tv.tv_usec = (timeout % 1000) * 1000;
	retval = select (FD_SETSIZE, &rfds, (fd_set *)0, (fd_set *)0, &tv);
	if (retval <= 0) // no response from serial port or error returned
		return false;
	return true;
}


///////////////////////////////////////////////////////
// Function name	: Cserial::ReadBuffer
// Description	  : Reads upto nchars from the selected port
// Return type	  : # characters received
// Argument		 : pointer to buffer; # chars to read
///////////////////////////////////////////////////////
int  Cserial::ReadBuffer (char *buf, int nchars)
{
	if (fd < 0) return 0;
	int retnum, nread = 0;
	while (nchars > 0) {
		if (!IOselect()) {
			return nread;
		}
		retnum = read (fd, (char *)(buf + nread), nchars);
		if (retnum < 0)
			return 0;//nread;
		if (retnum == 0)
			return nread;
		nread += retnum;
		nchars -= retnum;
	}
	return nread;
}

///////////////////////////////////////////////////////
// Function name	: Cserial::WriteBuffer
// Description	  : Writes a string to the selected port
// Return type	  : bool
// Argument		 : BYTE by
///////////////////////////////////////////////////////
int Cserial::WriteBuffer(const char *buff, int n)
{
	if (fd < 0) return 0;
	int ret = write (fd, buff, n);
	return ret;
}

///////////////////////////////////////////////////////
// Function name : Cserial::FlushBuffer
// Description   : flushes the pending rx chars
// Return type   : void
///////////////////////////////////////////////////////
void Cserial::FlushBuffer()
{
	if (fd < 0)
		return;
	tcflush (fd, TCIFLUSH);
}

//=============================================================================
// WIN32 serial implementation
//=============================================================================

#else // __WIN32__

using namespace std;

///////////////////////////////////////////////////////
// Function name	: Cserial::CheckPort
// Description	  : Checks the port specified by strPortName
// Return type	  : bool
// Argument		 : c_string strPortName
///////////////////////////////////////////////////////
bool Cserial::CheckPort(string dev)  {
	static HANDLE hTest;
	string COMportname = "//./";

	COMportname.append(dev);

	hTest = CreateFile(COMportname.c_str(),
			  GENERIC_READ | GENERIC_WRITE,
			  0,
			  0,
			  OPEN_EXISTING,
			  0,
			  0);

	if(hTest == INVALID_HANDLE_VALUE)
		return false;
	CloseHandle(hTest);
	return true;
}

///////////////////////////////////////////////////////
// Function name	: Cserial::OpenPort
// Description	  : Opens the port specified by strPortName
// Return type	  : bool
// Argument		 : CString strPortName
///////////////////////////////////////////////////////
bool Cserial::OpenPort()
{
	string COMportname = "//./";
	COMportname.append(device);

	hComm = CreateFile(COMportname.c_str(),
			  GENERIC_READ | GENERIC_WRITE,
			  0,
			  0,
			  OPEN_EXISTING,
			  0,
			  0);

	if(!hComm)
		return false;

	ConfigurePort( baud, 8, false, NOPARITY, stopbits);
//LOG_INFO("Comm port %s open; handle = %d", device.c_str(), hComm);
	FlushBuffer();
	return true;
}


///////////////////////////////////////////////////////
// Function name	: Cserial::ClosePort
// Description	  : Closes the Port
// Return type	  : void
///////////////////////////////////////////////////////
void Cserial::ClosePort()
{
	if (hComm) {
		bPortReady = SetCommTimeouts (hComm, &CommTimeoutsSaved);
		CloseHandle(hComm);
	}
	hComm = 0;
	return;
}

bool Cserial::IsOpen()
{
	if (!hComm) return false;
	return true;
}

///////////////////////////////////////////////////////
// Function name	: Cserial::GetBytesRead
// Description	  :
// Return type	  : DWORD
///////////////////////////////////////////////////////
DWORD Cserial::GetBytesRead()
{
	return nBytesRead;
}

///////////////////////////////////////////////////////
// Function name	: Cserial::GetBytesWritten
// Description	  : returns total number of bytes written to port
// Return type	  : DWORD
///////////////////////////////////////////////////////
DWORD Cserial::GetBytesWritten()
{
	return nBytesWritten;
}


///////////////////////////////////////////////////////
// Function name	: Cserial::ReadByte
// Description	  : Reads a byte from the selected port
// Return type	  : bool
// Argument		 : BYTE& by
///////////////////////////////////////////////////////
bool Cserial::ReadByte(char & by)
{
static  BYTE byResByte[1024];
static  DWORD dwBytesTxD=0;

	if (!hComm) return false;
	if (ReadFile (hComm, &byResByte[0], 1, &dwBytesTxD, 0)) {
		if (dwBytesTxD == 1) {
			by = (UCHAR)byResByte[0];
			return true;
		}
	}
	by = 0;
	return false;
}

int  Cserial::ReadData (char *buf, int nchars)
{
	DWORD dwRead = 0;
	if (!ReadFile(hComm, buf, nchars, &dwRead, NULL))
		return 0;
	return (int) dwRead;
}

int Cserial::ReadChars (char *buf, int nchars, int msec)
{
	if (msec) Sleep(msec);
	return ReadData (buf, nchars);
}

void Cserial::FlushBuffer()
{
	char c;
	int count = 200;
	while (ReadByte(c) == true) {
		if (count-- == 0) break;
	}
}

///////////////////////////////////////////////////////
// Function name	: Cserial::WriteByte
// Description	  : Writes a Byte to teh selected port
// Return type	  : bool
// Argument		 : BYTE by
///////////////////////////////////////////////////////
bool Cserial::WriteByte(char by)
{
	if (!hComm) return false;
	nBytesWritten = 0;
	if (WriteFile(hComm,&by,1,&nBytesWritten,NULL)==0)
		return false;
	return true;
}

///////////////////////////////////////////////////////
// Function name	: Cserial::WriteBuffer
// Description	  : Writes a string to the selected port
// Return type	  : bool
// Argument		 : BYTE by
///////////////////////////////////////////////////////
int Cserial::WriteBuffer(const char *buff, int n)
{
	if (!hComm)
		return 0;
	WriteFile (hComm, buff, n, &nBytesWritten, NULL);
	return nBytesWritten;
}


///////////////////////////////////////////////////////
// Function name	: Cserial::SetCommunicationTimeouts
// Description	  : Sets the timeout for the selected port
// Return type	  : bool
// Argument		 : DWORD ReadIntervalTimeout
// Argument		 : DWORD ReadTotalTimeoutMultiplier
// Argument		 : DWORD ReadTotalTimeoutConstant
// Argument		 : DWORD WriteTotalTimeoutMultiplier
// Argument		 : DWORD WriteTotalTimeoutConstant
///////////////////////////////////////////////////////
bool Cserial::SetCommunicationTimeouts(
	DWORD ReadIntervalTimeout, // msec
	DWORD ReadTotalTimeoutMultiplier,
	DWORD ReadTotalTimeoutConstant,
	DWORD WriteTotalTimeoutMultiplier,
	DWORD WriteTotalTimeoutConstant
)
{
	if((bPortReady = GetCommTimeouts (hComm, &CommTimeoutsSaved))==0)   {
		return false;
	}
	LOG_DEBUG("\n\
Read Interval Timeout............... %ld\n\
Read Total Timeout Multiplier....... %ld\n\
Read Total Timeout Constant Timeout. %ld\n\
Write Total Timeout Constant........ %ld\n\
Write Total Timeout Multiplier...... %ld",
	CommTimeoutsSaved.ReadIntervalTimeout,
	CommTimeoutsSaved.ReadTotalTimeoutMultiplier,
	CommTimeoutsSaved.ReadTotalTimeoutConstant,
	CommTimeoutsSaved.WriteTotalTimeoutConstant,
	CommTimeoutsSaved.WriteTotalTimeoutMultiplier);

	CommTimeouts.ReadIntervalTimeout = ReadIntervalTimeout;
	CommTimeouts.ReadTotalTimeoutMultiplier = ReadTotalTimeoutMultiplier;
	CommTimeouts.ReadTotalTimeoutConstant = ReadTotalTimeoutConstant;
	CommTimeouts.WriteTotalTimeoutConstant = WriteTotalTimeoutConstant;
	CommTimeouts.WriteTotalTimeoutMultiplier = WriteTotalTimeoutMultiplier;

	bPortReady = SetCommTimeouts (hComm, &CommTimeouts);

	if(bPortReady ==0) {
		CloseHandle(hComm);
		return false;
	}

	return true;
}

/*
 * Remarks
 *
 * WriteTotalTimeoutMultiplier
 *
 * The multiplier used to calculate the total time-out period for write
 * operations, in milliseconds. For each write operation, this value is
 * multiplied by the number of bytes to be written.
 *
 * WriteTotalTimeoutConstant
 *
 * A constant used to calculate the total time-out period for write operations,
 * in milliseconds. For each write operation, this value is added to the product
 * of the WriteTotalTimeoutMultiplier member and the number of bytes to be
 * written.
 *
 * A value of zero for both the WriteTotalTimeoutMultiplier and
 * WriteTotalTimeoutConstant members indicates that total time-outs are not
 * used for write operations.
 *
 *
 * If an application sets ReadIntervalTimeout and ReadTotalTimeoutMultiplier to
 * MAXDWORD and sets ReadTotalTimeoutConstant to a value greater than zero and
 * less than MAXDWORD, one of the following occurs when the ReadFile function
 * is called:
 *
 * If there are any bytes in the input buffer, ReadFile returns immediately
 * with the bytes in the buffer.
 *
 * If there are no bytes in the input buffer, ReadFile waits until a byte
 * arrives and then returns immediately.
 *
 * *********************************************************************
 * 
 * If no bytes arrive within the time specified by ReadTotalTimeoutConstant,
 * ReadFile times out.
 * 
 * ReadIntervalTimeout
 *
 * The maximum time allowed to elapse between the arrival of two bytes on the
 * communications line, in milliseconds. During a ReadFile operation, the time
 * period begins when the first byte is received. If the interval between the
 * arrival of any two bytes exceeds this amount, the ReadFile operation is
 * completed and any buffered data is returned. A value of zero indicates that
 * interval time-outs are not used.
 *
 * A value of MAXDWORD, combined with zero values for both the
 * ReadTotalTimeoutConstant and ReadTotalTimeoutMultiplier members, specifies
 * that the read operation is to return immediately with the bytes that have
 * already been received, even if no bytes have been received.
 *
 * ReadTotalTimeoutMultiplier
 *
 * The multiplier used to calculate the total time-out period for read
 * operations, in milliseconds. For each read operation, this value is
 * multiplied by the requested number of bytes to be read.
 *
 * ReadTotalTimeoutConstant
 *
 * A constant used to calculate the total time-out period for read operations,
 * in milliseconds. For each read operation, this value is added to the product
 * of the ReadTotalTimeoutMultiplier member and the requested number of bytes.
 *
 * A value of zero for both the ReadTotalTimeoutMultiplier and
 * ReadTotalTimeoutConstant members indicates that total time-outs are not
 * used for read operations.
 *
*/

bool Cserial::SetCommTimeout() {
	return SetCommunicationTimeouts (
		0, 		// Read Interval Timeout
		0,		// Read Total Timeout Multiplier
		50,		// Read Total Timeout Constant
		50,		// Write Total Timeout Constant
		0		// Write Total Timeout Multiplier
		);
}

///////////////////////////////////////////////////////
// Function name	: ConfigurePort
// Description	  : Configures the Port
// Return type	  : bool
// Argument		 : DWORD BaudRate
// Argument		 : BYTE ByteSize
// Argument		 : DWORD fParity
// Argument		 : BYTE  Parity
// Argument		 : BYTE StopBits
///////////////////////////////////////////////////////
bool Cserial::ConfigurePort(
		DWORD   BaudRate,
		BYTE	ByteSize,
		DWORD   dwParity,
		BYTE	Parity,
		BYTE	StopBits)
{
	if((bPortReady = GetCommState(hComm, &dcb))==0) {
//	  LOG_ERROR("GetCommState Error on %s", device.c_str());
		CloseHandle(hComm);
		return false;
	}

	dcb.BaudRate			= BaudRate;
	dcb.ByteSize			= ByteSize;
	dcb.Parity				= Parity ;
	dcb.StopBits			= (StopBits == 1 ? ONESTOPBIT : TWOSTOPBITS);
	dcb.fBinary				= true;
	dcb.fDsrSensitivity		= false;
	dcb.fParity				= dwParity;
	dcb.fOutX				= false;
	dcb.fInX				= false;
	dcb.fNull				= false;
	dcb.fAbortOnError		= true;
	dcb.fOutxCtsFlow		= false;
	dcb.fOutxDsrFlow		= false;

	if (dtr)
		dcb.fDtrControl = DTR_CONTROL_ENABLE;
	else
		dcb.fDtrControl = DTR_CONTROL_DISABLE;

	dcb.fDsrSensitivity = false;

	if (rtscts) dcb.fRtsControl = RTS_CONTROL_ENABLE;
	else {
		if (rts)
			dcb.fRtsControl = RTS_CONTROL_ENABLE;
		else
			dcb.fRtsControl = RTS_CONTROL_DISABLE;
	}

	bPortReady = SetCommState(hComm, &dcb);
	if(bPortReady == 0) {
		CloseHandle(hComm);
		return false;
	}
  return SetCommTimeout();
}

///////////////////////////////////////////////////////
// Function name	: Cserial::setPTT
// Return type	  : void
///////////////////////////////////////////////////////
void Cserial::SetPTT(bool ON)
{
	if(!hComm) {
		LOG_ERROR("Invalid handle");
		return;
	}
	if ( !(dtrptt || rtsptt) )
		return;

	LOG_INFO("PTT %d, DTRptt %d, DTR %d, RTSptt %d, RTS %d, RTSCTS %d",
		ON, dtrptt, dtr, rtsptt, rts, rtscts);

	if (ON) {
		if (dtrptt && dtr)
			dcb.fDtrControl = DTR_CONTROL_DISABLE;
		if (dtrptt && !dtr)
			dcb.fDtrControl = DTR_CONTROL_ENABLE;
		if (!rtscts) {
			if (rtsptt && rts)
				dcb.fRtsControl = RTS_CONTROL_DISABLE;
			if (rtsptt && !rts)
				dcb.fRtsControl = RTS_CONTROL_ENABLE;
		}
	} else {
		if (dtrptt && dtr)
			dcb.fDtrControl = DTR_CONTROL_ENABLE;
		if (dtrptt && !dtr)
			dcb.fDtrControl = DTR_CONTROL_DISABLE;
		if (!rtscts) {
			if (rtsptt && rts)
				dcb.fRtsControl = RTS_CONTROL_ENABLE;
			if (rtsptt && !rts)
				dcb.fRtsControl = RTS_CONTROL_DISABLE;
		}
	}
	SetCommState(hComm, &dcb);
}

void Cserial::setRTS(bool b)
{
	if(!hComm)
		return;

	if (b == true)
		dcb.fRtsControl = RTS_CONTROL_ENABLE;
	else
		dcb.fRtsControl = RTS_CONTROL_DISABLE;

	SetCommState(hComm, &dcb);
}

void Cserial::setDTR(bool b)
{
	if(!hComm)
		return;

	if (b == true)
		dcb.fDtrControl = DTR_CONTROL_ENABLE;
	else
		dcb.fDtrControl = DTR_CONTROL_DISABLE;

	SetCommState(hComm, &dcb);
}

#endif //__WIN32__


#ifdef __WIN32__
#include <sstream>

// convert COMx to /dev/ttySy with y = x - 1
void com_to_tty(string& port)
{
	int pos = port.find('m');
	if (pos == string::npos) pos = port.find('M');
	if (pos == string::npos) return;
	stringstream ss;
	int n;
	ss << port.substr(pos+1);
	ss >> n;
	if (--n < 0)
		n = 0;
	ss.clear(); ss.str("");
	ss << "/dev/ttyS" << n;
	ss.seekp(0);
	port = ss.str();
}

// convert  /dev/ttySx to COMy with y = x + 1
void tty_to_com(string& port)
{
	int pos = port.find('S');
	if (pos == string::npos) return;
	stringstream ss;
	int n;
	ss << port.substr(pos+1);
	ss >> n;
	ss.clear(); ss.str("");
	ss << "COM" << n + 1;
	ss.seekp(0);
	port = ss.str();
}
#endif
