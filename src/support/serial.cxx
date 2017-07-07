// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// This file is part of flrig.
//
// flrig is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// flrig is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include <string>
#include <iostream>

#include "serial.h"
#include "debug.h"

#include "rig.h"

LOG_FILE_SOURCE(debug::LOG_RIGCONTROL);

#if SERIAL_DEBUG
FILE *serlog = 0;
#endif

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
	device = "/dev/ttyS0";
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
	int testfd = open( dev.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
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

	if ((fd = open( device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY )) < 0)
		return false;
#if SERIAL_DEBUG
	fprintf(serlog,"%s opened\n", device.c_str());
#endif
// save current port settings
	tcflush (fd, TCIFLUSH);

	tcgetattr (fd, &oldtio);
	newtio = oldtio;

	// 8 data bits
	newtio.c_cflag &= ~CSIZE;
	newtio.c_cflag |= CS8;
	// enable receiver, set local mode
	newtio.c_cflag |= (CLOCAL | CREAD);
	// no parity
	newtio.c_cflag &= ~PARENB;

	if (stopbits == 1)
		// 1 stop bit
		newtio.c_cflag &= ~CSTOPB;
	else
		// 2 stop bit
		newtio.c_cflag |= CSTOPB;

	if (rtscts)
		// h/w handshake
		newtio.c_cflag |= CRTSCTS;
	else
		// no h/w handshake
		newtio.c_cflag &= ~CRTSCTS;

	// raw input
	newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	// raw output
	newtio.c_oflag &= ~OPOST;
	// software flow control disabled
	newtio.c_iflag &= ~IXON;
	// do not translate CR to NL
	newtio.c_iflag &= ~ICRNL;

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
#if SERIAL_DEBUG
	fprintf(serlog, "ptt fd < 0\n");
#endif
	  LOG_ERROR("ptt fd < 0");
		return;
	}
	if (dtrptt || rtsptt) {
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
		LOG_INFO("PTT %d, DTRptt %d, DTR %d, RTSptt %d, RTS %d, RTSCTS %d, status %2X",
			  ON, dtrptt, dtr, rtsptt, rts, rtscts, status);
#if SERIAL_DEBUG
		fprintf(serlog,"PTT %d, DTRptt %d, DTR %d, RTSptt %d, RTS %d, RTSCTS %d, status %2X\n",
			  ON, dtrptt, dtr, rtsptt, rts, rtscts, status);
#endif
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
	
// Some serial drivers force RTS and DTR high immediately upon
// opening the port, so our origstatus will indicate those bits
// high (though the lines weren't actually high before we opened).
// But then when we "restore" RTS and DTR from origstatus here
// it can result in PTT activation upon program exit!  To avoid
// this possibility, we ignore the apparentl initial settings, and
// instead force RTS and DTR low before closing the port.  (Just
// omitting the ioctl(TIOCMSET) would also resolve the problem).
// Kamal Mostafa <kamal@whence.com>

	origstatus &= ~(TIOCM_RTS|TIOCM_DTR);
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
// Function name	: Cserial::WriteByte
// Description	  : Writes a Byte to the selected port
// Return type	  : bool
// Argument		 : BYTE by
///////////////////////////////////////////////////////
bool Cserial::WriteByte(char by)
{
	if (fd < 0) return false;
	static char buff[2];
	buff[0] = by; buff[1] = 0;
	return (write(fd, buff, 1) == 1);
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

	if(hTest == INVALID_HANDLE_VALUE) return false;

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
	if (device.empty()) return false;

	string COMportname = "//./";
	COMportname.append(device);

	hComm = CreateFile(COMportname.c_str(),
			  GENERIC_READ | GENERIC_WRITE,
			  0,
			  0,
			  OPEN_EXISTING,
			  0,
			  0);

LOG_ERROR("Open Comm port %s ; hComm = %d", COMportname.c_str(), (int)hComm);
#if SERIAL_DEBUG
	fl_alert("Open Comm port %s ; hComm = %d", COMportname.c_str(), (int)hComm);
	fprintf(serlog, "Open Comm port %s ; hComm = %d\n", COMportname.c_str(), (int)hComm);
#endif

	if (hComm == INVALID_HANDLE_VALUE) return false;

	ConfigurePort( baud, 8, false, NOPARITY, stopbits);
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
	if (hComm != INVALID_HANDLE_VALUE) {
		bPortReady = SetCommTimeouts (hComm, &CommTimeoutsSaved);
		CloseHandle(hComm);
	}
	hComm = INVALID_HANDLE_VALUE;
	return;
}

bool Cserial::IsOpen()
{
	if (hComm == INVALID_HANDLE_VALUE) return false;
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

	if (hComm == INVALID_HANDLE_VALUE) return false;

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
#define TX_CLEAR 0x0004L
#define RX_CLEAR 0x0008L

	if (hComm == INVALID_HANDLE_VALUE) return;

	PurgeComm(hComm, RX_CLEAR);
}

///////////////////////////////////////////////////////
// Function name	: Cserial::WriteByte
// Description	  : Writes a Byte to teh selected port
// Return type	  : bool
// Argument		 : BYTE by
///////////////////////////////////////////////////////
bool Cserial::WriteByte(char by)
{
	if (hComm == INVALID_HANDLE_VALUE) return false;

	nBytesWritten = 0;
	if (WriteFile(hComm,&by,1,&nBytesWritten,NULL)==0) return false;

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
	if (hComm == INVALID_HANDLE_VALUE) return 0;

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
	LOG_ERROR("\n\
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
#if SERIAL_DEBUG
	fprintf(serlog, "\
Read Interval Timeout............... %ld\n\
Read Total Timeout Multiplier....... %ld\n\
Read Total Timeout Constant Timeout. %ld\n\
Write Total Timeout Constant........ %ld\n\
Write Total Timeout Multiplier...... %ld\n",
	CommTimeoutsSaved.ReadIntervalTimeout,
	CommTimeoutsSaved.ReadTotalTimeoutMultiplier,
	CommTimeoutsSaved.ReadTotalTimeoutConstant,
	CommTimeoutsSaved.WriteTotalTimeoutConstant,
	CommTimeoutsSaved.WriteTotalTimeoutMultiplier);
#endif

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
		0,		// Read Interval Timeout
		0,		// Read Total Timeout Multiplier
		50,		// Read Total Timeout Constant
		50,		// Write Total Timeout Constant
		0		// Write Total Timeout Multiplier
		);
//	CommTimeouts.ReadIntervalTimeout = 50;
//	CommTimeouts.ReadTotalTimeoutConstant = 50;
//	CommTimeouts.ReadTotalTimeoutMultiplier=10;
//	CommTimeouts.WriteTotalTimeoutMultiplier=10;
//	CommTimeouts.WriteTotalTimeoutConstant = 50;

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
	if (hComm == INVALID_HANDLE_VALUE) return false;

//	memset(dcb, 0, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);

	if((bPortReady = GetCommState(hComm, &dcb)) == 0) {
	  LOG_ERROR("GetCommState Error on %s", device.c_str());
#if SERIAL_DEBUG
	fl_alert("GetCommState Error on %s", device.c_str());
	fprintf(serlog, "GetCommState Error on %s\n", device.c_str());
#endif
		CloseHandle(hComm);
		return false;
	}
#if SERIAL_DEBUG
	fprintf(serlog, "\
\n\
Get Comm State:\n\
DCB.DCBlength       %d\n\
DCB.Baudrate        %d\n\
DCB.ByteSize        %d\n\
DCB.Parity          %d\n\
DCB.StopBits        %d\n\
DCB.Binary          %d\n\
DCB.fDtrControl     %d\n\
DCB.fRtsControl     %d\n\
DCB.fDsrSensitivity %d\n\
DCB.fParity         %d\n\
DCB.fOutX           %d\n\
DCB.fInX            %d\n\
DCB.fNull           %d\n\
DCB.XonChar         %d\n\
DCB.XoffChar        %d\n\
DCB.fAbortOnError   %d\n\
DCB.fOutxCtsFlow    %d\n\
DCB.fOutxDsrFlow    %d\n",
	(int)dcb.DCBlength,
	(int)dcb.BaudRate,
	(int)dcb.ByteSize,
	(int)dcb.Parity,
	(int)dcb.StopBits,
	(int)dcb.fBinary,
	(int)dcb.fDtrControl,
	(int)dcb.fRtsControl,
	(int)dcb.fDsrSensitivity,
	(int)dcb.fParity,
	(int)dcb.fOutX,
	(int)dcb.fInX,
	(int)dcb.fNull,
	(int)dcb.XonChar,
	(int)dcb.XoffChar,
	(int)dcb.fAbortOnError,
	(int)dcb.fOutxCtsFlow,
	(int)dcb.fOutxDsrFlow);
#endif

	dcb.DCBlength			= sizeof (dcb);
	dcb.BaudRate			= BaudRate;
	dcb.ByteSize			= ByteSize;
	dcb.Parity				= Parity ;
	if (dcb.StopBits) // corrects a driver malfunction in the Yaesu SCU-17
		dcb.StopBits			= (StopBits == 1 ? ONESTOPBIT : TWOSTOPBITS);
	dcb.fBinary				= true;
	dcb.fDsrSensitivity		= false;
	dcb.fParity				= false;
	dcb.fOutX				= false;
	dcb.fInX				= false;
	dcb.fNull				= false;
	dcb.fAbortOnError		= false;
	dcb.fOutxCtsFlow		= false;
	dcb.fOutxDsrFlow		= false;
	dcb.fErrorChar			= false;

//PortDCB.fTXContinueOnXoff = TRUE;                // XOFF continues Tx

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

#if SERIAL_DEBUG
	fprintf(serlog, "\
\n\
Set Comm State:\n\
DCB.DCBlength       %d\n\
DCB.Baudrate        %d\n\
DCB.ByteSize        %d\n\
DCB.Parity          %d\n\
DCB.StopBits        %d\n\
DCB.Binary          %d\n\
DCB.fDtrControl     %d\n\
DCB.fRtsControl     %d\n\
DCB.fDsrSensitivity %d\n\
DCB.fParity         %d\n\
DCB.fOutX           %d\n\
DCB.fInX            %d\n\
DCB.fNull           %d\n\
DCB.XonChar         %d\n\
DCB.XoffChar        %d\n\
DCB.fAbortOnError   %d\n\
DCB.fOutxCtsFlow    %d\n\
DCB.fOutxDsrFlow    %d\n",
	(int)dcb.DCBlength,
	(int)dcb.BaudRate,
	(int)dcb.ByteSize,
	(int)dcb.Parity,
	(int)dcb.StopBits,
	(int)dcb.fBinary,
	(int)dcb.fDtrControl,
	(int)dcb.fRtsControl,
	(int)dcb.fDsrSensitivity,
	(int)dcb.fParity,
	(int)dcb.fOutX,
	(int)dcb.fInX,
	(int)dcb.fNull,
	(int)dcb.XonChar,
	(int)dcb.XoffChar,
	(int)dcb.fAbortOnError,
	(int)dcb.fOutxCtsFlow,
	(int)dcb.fOutxDsrFlow);
#endif

	bPortReady = SetCommState(hComm, &dcb);

//	if(bPortReady == 0) {
#if SERIAL_DEBUG
	long err = GetLastError();
	fl_alert("SetCommState handle %d, returned %d\nError = %d", (int)hComm, bPortReady, (int)err);
	fprintf(serlog, "SetCommState handle %d, returned %d, error = %d\n", (int)hComm, bPortReady, (int)err);
#endif
if (bPortReady == 0) {
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
	if (hComm == INVALID_HANDLE_VALUE) {
#if SERIAL_DEBUG
	fl_alert("SetPTT failed, invalid handle");
	fprintf(serlog, "SetPTT failed, invalid handle\n");
#endif
		return;
	}
	if ( !(dtrptt || rtsptt) )
		return;

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

	LOG_ERROR("PTT %d, DTRptt %d, DTR %d, RTSptt %d, RTS %d, RTSCTS %d, %2x %2x",
		ON, dtrptt, dtr, rtsptt, rts, rtscts, 
		static_cast<unsigned int>(dcb.fDtrControl), 
		static_cast<unsigned int>(dcb.fRtsControl) );
#if SERIAL_DEBUG
	fprintf(serlog, "\
PTT %d, DTRptt %d, DTR %d, RTSptt %d, RTS %d, RTSCTS %d, DtrControl %2x, RtsControl %2x\n",
		ON, dtrptt, dtr, rtsptt, rts, rtscts, 
		static_cast<unsigned int>(dcb.fDtrControl), 
		static_cast<unsigned int>(dcb.fRtsControl) );
#endif

	SetCommState(hComm, &dcb);
}

void Cserial::setRTS(bool b)
{
	if (hComm == INVALID_HANDLE_VALUE) return;

	if (b == true)
		dcb.fRtsControl = RTS_CONTROL_ENABLE;
	else
		dcb.fRtsControl = RTS_CONTROL_DISABLE;

#if SERIAL_DEBUG
	int retval = SetCommState(hComm, &dcb);
	fprintf(serlog, "\
Set RTS %d, DtrControl %2x, RtsControl %2x ==> %d\n",
		b,
		static_cast<unsigned int>(dcb.fDtrControl), 
		static_cast<unsigned int>(dcb.fRtsControl),
		retval );
	return;
#endif

	SetCommState(hComm, &dcb);
}

void Cserial::setDTR(bool b)
{
	if (hComm == INVALID_HANDLE_VALUE) return;

	if (b == true)
		dcb.fDtrControl = DTR_CONTROL_ENABLE;
	else
		dcb.fDtrControl = DTR_CONTROL_DISABLE;

#if SERIAL_DEBUG
	int retval = SetCommState(hComm, &dcb);
	fprintf(serlog, "\
Set DTR %d, DtrControl %2x, RtsControl %2x ==> %d\n",
		b,
		static_cast<unsigned int>(dcb.fDtrControl), 
		static_cast<unsigned int>(dcb.fRtsControl),
		retval );
	return;
#endif

	SetCommState(hComm, &dcb);
}

Cserial::Cserial() {
	rts = dtr = false;
	rtsptt = dtrptt = false;
	rtscts = false;
	baud = CBR_9600;
	stopbits = 2;
	hComm = 0;
}

Cserial::Cserial( std::string portname) {
	device = portname;
	Cserial();
}

Cserial::~Cserial() {
	ClosePort();
}

#endif
