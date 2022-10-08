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
#include <errno.h>

#include "debug.h"
#include "rig.h"
#include "serial.h"
#include "status.h"
#include "trace.h"
#include "tod_clock.h"

LOG_FILE_SOURCE(debug::LOG_RIGCONTROL);

char traceinfo[500];

bool SERIALDEBUG = false;

bool check_hex(const void *s, size_t len)
{
	unsigned char *str = (unsigned char *)s;
	for (size_t n = 0; n < len; n++) {
		if (str[n] == '\r' || str[n] == '\n') continue;
		if (str[n] < 0x20) return true;
		if (str[n] > 0x7F) return true;
	}
	return false;
}

#ifndef __WIN32__
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include <memory>

Cserial::Cserial() {
	device = "/dev/ttyS0";
	baud = 1200;
	timeout = 100; //msec
	retries = 5;
	rts =
	dtr =
	rtsptt =
	dtrptt =
	rtscts =
	serptt = false;
	state = 0;
	stopbits = 2;
	fd = -1;
	failed_ = 0;
}

Cserial::~Cserial() {
	ClosePort();
}

///////////////////////////////////////////////////////
// Function name	: Cserial::OpenPort
// Description	  : Opens the port specified by strPortName
// Return type	  : bool
// Argument		 : c_std::string strPortName
///////////////////////////////////////////////////////
bool Cserial::CheckPort(std::string dev)  {
	int testfd = open( dev.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if (testfd < 0) {
		snprintf(traceinfo, sizeof(traceinfo), "%s checkPort() FAILED", dev.c_str());
		ser_trace(1, traceinfo);
		return false;
	}
	close(fd);
	return true;
}

void Cserial::set_attributes()
{
// save current port settings
	tcflush (fd, TCIFLUSH);

//	tcgetattr (fd, &oldtio);
//	newtio = oldtio;

//	ioctl(fd, TIOCMGET, &state);
//	origstate = state;

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
			timeout = 1024;
			break;
		case 1200:
			speed = B1200;
			timeout = 1024;
			break;
		case 2400:
			speed = B2400;
			timeout = 1024;
			break;
		case 4800:
			speed = B4800;
			timeout = 512;
			break;
		default:
		case 9600:
			speed = B9600;
			timeout = 256;
			break;
		case 19200:
			speed = B19200;
			timeout = 128;
			break;
		case 38400:
			speed = B38400;
			timeout = 64;
			break;
		case 57600:
			speed = B57600;
			timeout = 32;
			break;
		case 115200:
			speed = B115200;
			timeout = 16;
			break;
	}

	timeout = progStatus.serial_timeout;
	cfsetispeed(&newtio, speed);
	cfsetospeed(&newtio, speed);

	tcsetattr (fd, TCSANOW, &newtio);

	if (dtr)
		state |= TIOCM_DTR;		// set the DTR bit
	else
		state &= ~TIOCM_DTR;	   // clear the DTR bit

	if (rtscts == false) {		  // rts OK for ptt if RTSCTS not used
		if (rts)
			state |= TIOCM_RTS;		// set the RTS bit
		else
			state &= ~TIOCM_RTS;	   // clear the RTS bit
	}
	ioctl(fd, TIOCMSET, &state);

}

///////////////////////////////////////////////////////
// Function name	: Cserial::OpenPort
// Description	  : Opens the port specified by strPortName
// Return type	  : bool
// Argument		 : c_std::string strPortName
///////////////////////////////////////////////////////
bool Cserial::OpenPort()  {

	if (IsOpen()) ClosePort();
	if ((fd = open( device.c_str(), O_RDWR | O_NOCTTY )) < 0) { // | O_NDELAY )) < 0) {
		snprintf(traceinfo, sizeof(traceinfo), "%s OpenPort() FAILED", device.c_str());
		ser_trace(1, traceinfo);
		return false;
	}

	if (progStatus.serialtrace || SERIALDEBUG) {
		snprintf(traceinfo, sizeof(traceinfo),"%s opened: fd = %d", device.c_str(), fd);
		ser_trace(1, traceinfo);
	}

	MilliSleep(5); // K0OG

	tcflush (fd, TCIFLUSH);
	tcgetattr (fd, &oldtio);
	newtio = oldtio;
	ioctl(fd, TIOCMGET, &state);
	origstate = state;

	set_attributes();

	FlushBuffer();

	failed_ = 0;

	return true;
}

///////////////////////////////////////////////////////
// Function name	: Cserial::setPTT
// Return type	  : void
///////////////////////////////////////////////////////

bool Cserial::RTSptt(){
	return serptt;
}

bool Cserial::DTRptt(){
	return serptt;
}

bool Cserial::getPTT() {
	return serptt;
}

void Cserial::SetPTT(bool ON)
{
	if (fd < 0) {
		if (progStatus.serialtrace || SERIALDEBUG) {
			snprintf(traceinfo, sizeof(traceinfo), "SetPTT(...) fd < 0");
			ser_trace(1, traceinfo);
		}
		LOG_ERROR("ptt fd < 0");
		return;
	}

	if (dtrptt || rtsptt) {
		ioctl(fd, TIOCMGET, &state);
		if (ON) {								  // ptt enabled
			if (dtrptt && dtr)  state &= ~TIOCM_DTR;	 // toggle low
			if (dtrptt && !dtr) state |= TIOCM_DTR;	  // toggle high
			if (!rtscts) {
				if (rtsptt && rts)  state &= ~TIOCM_RTS; // toggle low
				if (rtsptt && !rts) state |= TIOCM_RTS;  // toggle high
			}
		} else {										  // ptt disabled
			if (dtrptt && dtr)  state |= TIOCM_DTR;	  // toggle high
			if (dtrptt && !dtr) state &= ~TIOCM_DTR;	 // toggle low
			if (!rtscts) {
				if (rtsptt && rts)  state |= TIOCM_RTS;  // toggle high
				if (rtsptt && !rts) state &= ~TIOCM_RTS; // toggle low
			}
		}
		LOG_INFO("PTT %d, DTRptt %d, DTR %d, RTSptt %d, RTS %d, RTSCTS %d, state %2X",
			  ON, dtrptt, dtr, rtsptt, rts, rtscts, state);
		if (progStatus.serialtrace || SERIALDEBUG) {
			snprintf(traceinfo, sizeof(traceinfo),"PTT %d, DTRptt %d, DTR %d, RTSptt %d, RTS %d, RTSCTS %d, state %2X",
				ON, dtrptt, dtr, rtsptt, rts, rtscts, state);
			ser_trace(1, traceinfo);
		}
		ioctl(fd, TIOCMSET, &state);
	}
	serptt = ON;
}

void Cserial::setRTS(bool b)
{
	if (fd < 0) {
		return;
	}

	ioctl(fd, TIOCMGET, &state);
	if (b == true)
		state |= TIOCM_RTS;  // toggle high
	else
		state &= ~TIOCM_RTS; // toggle low

	if (ioctl(fd, TIOCMSET, &state) == -1) {
		char errstr[50];
		snprintf(errstr, sizeof(errstr), "set RTS ioctl error: %d", errno);
		LOG_ERROR("%s", errstr);
		ser_trace(1, errstr);
	}
}

void Cserial::setDTR(bool b)
{
	if (fd < 0)
		return;

	ioctl(fd, TIOCMGET, &state);
	if (b == true)
		state |= TIOCM_DTR;	  // toggle high
	else
		state &= ~TIOCM_DTR;	 // toggle low
	if (ioctl(fd, TIOCMSET, &state) == -1) {
		char errstr[50];
		snprintf(errstr, sizeof(errstr), "set DTR ioctl error: %d", errno);
		LOG_ERROR("%s", errstr);
		ser_trace(1, errstr);
	}

}

///////////////////////////////////////////////////////
// Function name	: Cserial::ClosePort
// Description	  : Closes the Port
// Return type	  : void
///////////////////////////////////////////////////////
void Cserial::ClosePort()
{
	char msg[50];
	snprintf(msg, sizeof(msg),"ClosePort(): fd = %d", fd);
	ser_trace(1, msg);
	if (fd < 0) return;
	int myfd = fd;
	fd = -1;

// Some serial drivers force RTS and DTR high immediately upon
// opening the port, so our origstate will indicate those bits
// high (though the lines weren't actually high before we opened).
// But then when we "restore" RTS and DTR from origstate here
// it can result in PTT activation upon program exit!  To avoid
// this possibility, we ignore the apparentl initial settings, and
// instead force RTS and DTR low before closing the port.  (Just
// omitting the ioctl(TIOCMSET) would also resolve the problem).
// Kamal Mostafa <kamal@whence.com>

//	origstate &= ~(TIOCM_RTS|TIOCM_DTR);
//	ioctl(myfd, TIOCMSET, &origstate);
//	tcsetattr (myfd, TCSANOW, &oldtio);
	close(myfd);
	ser_trace(1,"serial port closed");
	fd = -1;
	failed_ = false;
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

	if (retval <= 0) { // no response from serial port or error returned
		return false;
	}
	return true;
}


///////////////////////////////////////////////////////
// Function name	: Cserial::ls dx
// Description	  : Reads upto nchars from the selected port
// Return type	  : # characters received
// Argument		 : pointer to buffer; # chars to read; std::string terminator
///////////////////////////////////////////////////////
int  Cserial::ReadBuffer (std::string &buf, int nchars, std::string find1, std::string find2)
{
	if (fd < 0) {
		ser_trace(1, "ReadBuffer(...) fd < 0");
		return 0;
	}

	bool hex = false;
	std::string s1, s2;

	if (find1.length())
		hex = hex || check_hex(find1.c_str(), find1.length());
	if (find2.length())
		hex = hex || check_hex(find2.c_str(), find2.length());
	if (hex) {
		s1 = str2hex(find1.c_str(), find1.length());
		s2 = str2hex(find2.c_str(), find2.length());
	} else {
		s1 = find1;
		size_t p = s1.find("\r\n");
		if (p != std::string::npos) s1.replace(p,2,"<cr><lf>");
		p = s1.find('\r');
		if (p != std::string::npos) s1.replace(p,1,"<cr>");
		p = s1.find('\n');
		if (p != std::string::npos) s1.replace(p,1,"<lf>");

		s2 = find2;
		p = s2.find("\r\n");
		if (p != std::string::npos) s2.replace(p,2,"<cr><lf>");
		p = s2.find('\r');
		if (p != std::string::npos) s2.replace(p,1,"<cr>");
		p = s2.find('\n');
		if (p != std::string::npos) s2.replace(p,1,"<lf>");
	}

	bool  find_two = (!find1.empty() && !find2.empty());
	bool  find_one = (!find1.empty() && find2.empty());

	if (find_two)
		snprintf(traceinfo, sizeof(traceinfo), "ReadBuffer(... %d, %s, %s)", nchars, s1.c_str(), s2.c_str());
	else if (find_one)
		snprintf(traceinfo, sizeof(traceinfo), "ReadBuffer(... %d, %s)", nchars, s1.c_str());
	else
		snprintf(traceinfo, sizeof(traceinfo), "ReadBuffer(... %d)", nchars);

	if (SERIALDEBUG)
		ser_trace(1, traceinfo);

	int retnum = 0;
	int nread = 0;
	int bytes = 0;

	size_t p1, p2;

	fd_set rfds;

	FD_ZERO (&rfds);
	FD_SET (fd, &rfds);

	size_t tnow = zusec();
	size_t start = tnow;

	do {

		ioctl( fd, FIONREAD, &bytes);
		if (bytes) {
			unsigned char tempbuf[bytes + 1];
			memset(tempbuf, 0, bytes + 1);

			snprintf(traceinfo, sizeof(traceinfo), "bytes: %d", bytes);
			if (SERIALDEBUG)
				ser_trace(1, traceinfo);

			retnum = read (fd, tempbuf, bytes);
			if (retnum > 0 && retnum <= bytes) {
				for (int nc = 0; nc < retnum; nc++)
					buf += tempbuf[nc];
				nread += retnum;
			} else if (progStatus.serialtrace || SERIALDEBUG) {
				char traceinfo[100];
				snprintf(traceinfo, sizeof(traceinfo), "read(fd %d, tempbuf, 200) error: %s", fd,
					(errno == EAGAIN ? "EAGAIN" :
					errno == EWOULDBLOCK ? "EWOULDBLOCK" :
					errno == EBADF ? "EBADF" :
					errno == EFAULT? "EFAULT" :
					errno == EINTR ? "EINTR" :
					errno == EINVAL ? "EINVAL" :
					errno == EIO ? "EIO" :
					errno == EISDIR ? "EISDRI" : "OTHER"));
					ser_trace(1, traceinfo);
			}

			if (find_two) {
				p1 = buf.rfind(find1);
				p2 = buf.rfind(find2);
				if (p1 != std::string::npos &&
					p2 != std::string::npos &&
					p2 > p1) {
					
					if (progStatus.serialtrace || SERIALDEBUG) {
					char traceinfo[100];
						std::string srx = buf;
						size_t p = srx.find("\r\n");
						if (p != std::string::npos) srx.replace(p,2,"<cr><lf>");
						p = srx.find('\r');
						if (p != std::string::npos) srx.replace(p,1,"<cr>");
						p = srx.find('\n');
						if (p != std::string::npos) srx.replace(p,1,"<lf>");
						bool hex = check_hex(buf.c_str(), buf.length());
						snprintf(traceinfo, sizeof(traceinfo), "READBUFFER TWO [%0.3f msec]: %s",
							(zusec() - start) * 1e-3,
							(hex ? str2hex(buf.c_str(), buf.length()) : srx.c_str()) );
						ser_trace(1, traceinfo);
					}
				}
				return nread;
			}

			else if (find_one) {
				p1 = buf.rfind(find1);
				if (p1 != std::string::npos) {

					if (progStatus.serialtrace || SERIALDEBUG) {
						char traceinfo[100];
						std::string srx = buf;
						size_t p = srx.find("\r\n");
						if (p != std::string::npos) srx.replace(p,2,"<cr><lf>");
						p = srx.find('\r');
						if (p != std::string::npos) srx.replace(p,1,"<cr>");
						p = srx.find('\n');
						if (p != std::string::npos) srx.replace(p,1,"<lf>");
						bool hex = check_hex(buf.c_str(), buf.length());
						snprintf(traceinfo, sizeof(traceinfo), "READBUFFER ONE [%0.3f msec]: %s",
							(zusec() - start) * 1e-3,
							(hex ? str2hex(buf.c_str(), buf.length()) : srx.c_str()) );
						ser_trace(1, traceinfo);
					}
					return nread;
				}
			}

			else if ( nread >= nchars ) {
				if (progStatus.serialtrace || SERIALDEBUG) {
					char traceinfo[100];
					std::string srx = buf;
					size_t p = srx.find("\r\n");
					if (p != std::string::npos) srx.replace(p,2,"<cr><lf>");
					p = srx.find('\r');
					if (p != std::string::npos) srx.replace(p,1,"<cr>");
					p = srx.find('\n');
					if (p != std::string::npos) srx.replace(p,1,"<lf>");
					bool hex = check_hex(buf.c_str(), buf.length());
					snprintf(traceinfo, sizeof(traceinfo), "READBUFFER NCHARS [%0.3f msec]: %s",
						(zusec() - start) * 1e-3,
						(hex ? str2hex(buf.c_str(), buf.length()) : srx.c_str()) );
					ser_trace(1, traceinfo);
				}
				return nread;
			}

			tnow = zusec();
		}

		MilliSleep(1);

	} while ( (zusec() - tnow) < (size_t)(progStatus.serial_timeout * 1000L));

	if (progStatus.serialtrace || SERIALDEBUG) {
		char traceinfo[100];
		std::string srx = buf;
		size_t p = srx.find("\r\n");
		if (p != std::string::npos) srx.replace(p,2,"<cr><lf>");
		p = srx.find('\r');
		if (p != std::string::npos) srx.replace(p,1,"<cr>");
		p = srx.find('\n');
		if (p != std::string::npos) srx.replace(p,1,"<lf>");
		bool hex = check_hex(buf.c_str(), buf.length());
		snprintf(traceinfo, sizeof(traceinfo), "ReadBuffer TIMED OUT [%0.3f msec]: %s",
			(zusec() - start) * 1e-3,
			(hex ? str2hex(buf.c_str(), buf.length()) : srx.c_str()) );
		ser_trace(1, traceinfo);
	}

	return nread;
}

///////////////////////////////////////////////////////
// Function name	: Cserial::WriteBuffer
// Description	  : Writes a std::string to the selected port
// Return type	  : bool
// Argument		 : BYTE by
///////////////////////////////////////////////////////
int Cserial::WriteBuffer(const char *buff, int n)
{
	if (fd < 0) {
		ser_trace(1, "WriteBuffer(...) fd < 0");
		return 0;
	}

	if (progStatus.serialtrace || SERIALDEBUG) {
		std::string sw = buff;
		size_t p = sw.rfind("\r\n");
		if (p == (sw.length() - 2) ) {
			sw.replace(p, 2, "<cr><lf>");
			ser_trace(2, "WRITE BUFFER: ", sw.c_str());
		} else  {
			bool hex = check_hex(sw.c_str(), sw.length());
			ser_trace(2, "WRITE BUFFER: ", (hex ? str2hex(sw.c_str(), sw.length()) : sw.c_str()));
		}
	}

	int ret = 0;
	if (progStatus.serial_write_delay) {
		for (int j = 0; j < n; j++) {
			ret = write (fd, &buff[j], 1);
			MilliSleep(progStatus.serial_write_delay);
			if (!ret) break;
		}
	} else
		ret = write (fd, buff, n);

	int timeout = progStatus.serial_post_write_delay;
	while (timeout > 0) {
		if (timeout > 10) MilliSleep(10);
		else MilliSleep(timeout);
		timeout -= 10;
		Fl::awake();
	}

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

///////////////////////////////////////////////////////
// Function name	: Cserial::CheckPort
// Description	  : Checks the port specified by strPortName
// Return type	  : bool
// Argument		 : c_std::string strPortName
///////////////////////////////////////////////////////
bool Cserial::CheckPort(std::string dev)  {
	static HANDLE hTest;
	std::string COMportname = "//./";

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
// Argument		 : Cstd::string strPortName
///////////////////////////////////////////////////////
bool Cserial::OpenPort()
{
	if (device.empty()) return false;

	if (IsOpen()) ClosePort();

	std::string COMportname = "//./";
	COMportname.append(device);

	hComm = CreateFile(COMportname.c_str(),
			  GENERIC_READ | GENERIC_WRITE,
			  0,
			  0,
			  OPEN_EXISTING,
			  0,
			  0);

	if (hComm == INVALID_HANDLE_VALUE) {
		LOG_ERROR("Open Comm port %s ; hComm = %u", COMportname.c_str(), (size_t)hComm);

		if (progStatus.serialtrace || SERIALDEBUG) {
			snprintf(traceinfo, sizeof(traceinfo),
				"INVALID_HANDLE_VALUE: Open Comm port %s ; hComm = %u\n",
				COMportname.c_str(), (size_t)hComm);
			ser_trace(1, traceinfo);
		}

		return false;
	}

	if (progStatus.serialtrace || SERIALDEBUG) {
		snprintf(traceinfo, sizeof(traceinfo),
			"Open Comm port %s ; hComm = %u\n",
			COMportname.c_str(), (size_t)hComm);
		ser_trace(1, traceinfo);
		}

	ConfigurePort( baud, 8, false, NOPARITY, stopbits);

	MilliSleep(5); // K0OG

	FlushBuffer();
	failed_ = false;

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
	failed_ = false;
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

int  Cserial::ReadBuffer (std::string &buf, int nchars, std::string find1, std::string find2)
{
	if (hComm == INVALID_HANDLE_VALUE) {
		if (progStatus.serialtrace || SERIALDEBUG) {
			snprintf(traceinfo, sizeof(traceinfo), "ReadBuffer, invalid handle\n");
			ser_trace(1, traceinfo);
		}
		return 0;
	}

	BOOL hex = false;
	std::string s1, s2;

	if (find1.length())
		hex = hex || check_hex(find1.c_str(), find1.length());
	if (find2.length())
		hex = hex || check_hex(find2.c_str(), find2.length());
	if (hex) {
		s1 = str2hex(find1.c_str(), find1.length());
		s2 = str2hex(find2.c_str(), find2.length());
	} else {
		s1 = find1;
		size_t p = s1.find("\r\n");
		if (p != std::string::npos) s1.replace(p,2,"<cr><lf>");
		p = s1.find('\r');
		if (p != std::string::npos) s1.replace(p,1,"<cr>");
		p = s1.find('\n');
		if (p != std::string::npos) s1.replace(p,1,"<lf>");

		s2 = find2;
		p = s2.find("\r\n");
		if (p != std::string::npos) s2.replace(p,2,"<cr><lf>");
		p = s2.find('\r');
		if (p != std::string::npos) s2.replace(p,1,"<cr>");
		p = s2.find('\n');
		if (p != std::string::npos) s2.replace(p,1,"<lf>");
	}

	bool  find_two = (!find1.empty() && !find2.empty());
	bool  find_one = (!find1.empty() && find2.empty());

	if (find_two)
		snprintf(traceinfo, sizeof(traceinfo), "ReadBuffer(... %d, %s, %s)", nchars, s1.c_str(), s2.c_str());
	else if (find_one)
		snprintf(traceinfo, sizeof(traceinfo), "ReadBuffer(... %d, %s)", nchars, s1.c_str());
	else
		snprintf(traceinfo, sizeof(traceinfo), "ReadBuffer(... %d)", nchars);

	if (progStatus.serialtrace || SERIALDEBUG)
		ser_trace(1, traceinfo);

	DWORD dwRead = 0;
	int   nread = 0;
	BOOL  retval = 0;
	unsigned char uctemp[nchars + 1];

	size_t tnow = zusec();
	size_t start = tnow;

	do {

		memset(uctemp, 0, nchars + 1);
		retval = ReadFile(hComm, uctemp, nchars - nread, &dwRead, NULL);

		if (progStatus.serialtrace || SERIALDEBUG) {
			int hex = check_hex(buf.c_str(), buf.length());
			snprintf(traceinfo, sizeof(traceinfo), 
				"ReadFile retval: %d, dwRead: %ld, %s",
				retval,
				dwRead,
				(hex ? str2hex(buf.c_str(), buf.length()) : buf.c_str()) );
			ser_trace(1, traceinfo);
		}

		if (retval == 0) {
			goto doloop;
		}

		if (dwRead == 0) {
			goto doloop;
		}

		for (int nc = 0; nc < (int)dwRead; nc++)
			buf += uctemp[nc];

		nread += (int)dwRead;

		if (find_two) {
			size_t p1 = buf.rfind(find1);
			size_t p2 = buf.rfind(find2);
			if (p1 != std::string::npos &&
				p2 != std::string::npos &&
				p2 > p1) {
				if (progStatus.serialtrace || SERIALDEBUG) {
					hex = check_hex(buf.c_str(), buf.length());
					snprintf(traceinfo, sizeof(traceinfo), "ReadBuffer TWO [%0.3f msec]: %s", (zusec() - start) * 1e-3,
						(hex ? str2hex(buf.c_str(), buf.length()) : buf.c_str()));
					ser_trace(1, traceinfo);
				}
				return nread;
			}
		}

		else if (find_one) {
			if (buf.rfind(find1) != std::string::npos) {
				if (progStatus.serialtrace || SERIALDEBUG) {
					hex = check_hex(buf.c_str(), buf.length());
					snprintf(traceinfo, sizeof(traceinfo), "ReadBuffer ONE [%0.3f msec]: %s", (zusec() - start) * 1e-3,
						(hex ? str2hex(buf.c_str(), buf.length()) : buf.c_str()) );
					ser_trace(1, traceinfo);
				}
				return nread;
			}
		}

		else if ( nread >= nchars ) {
			if (progStatus.serialtrace || SERIALDEBUG) {
				hex = check_hex(buf.c_str(), buf.length());
				snprintf(traceinfo, sizeof(traceinfo), "ReadBuffer NCHARS [%0.3f msec]: %s", (zusec() - start) * 1e-3,
					(hex ? str2hex(buf.c_str(), buf.length()) : buf.c_str()) );
				ser_trace(1, traceinfo);
			}
			return nread;
		}
		tnow = zusec();
doloop:
		MilliSleep(1);
	} while ( (zusec() - tnow) < (size_t)(progStatus.serial_timeout * 1000L));

	if (progStatus.serialtrace || SERIALDEBUG) {
		hex = check_hex(buf.c_str(), buf.length());
		snprintf(traceinfo, sizeof(traceinfo), "ReadBuffer TIMED OUT [%0.3f msec]: %s", (int)(zmsec() - start),
			(hex ? str2hex(buf.c_str(), buf.length()) : buf.c_str()) );
		ser_trace(1, traceinfo);
	}
	return nread;
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
// Description	  : Writes a std::string to the selected port
// Return type	  : bool
// Argument		 : BYTE by
///////////////////////////////////////////////////////
int Cserial::WriteBuffer(const char *buff, int n)
{
	if (hComm == INVALID_HANDLE_VALUE) return 0;

	if (progStatus.serialtrace || SERIALDEBUG) {
		std::string sw = buff;
		size_t p = sw.rfind("\r\n");
		if (p == (sw.length() - 2) ) {
			sw.replace(p, 2, "<cr><lf>");
			ser_trace(2, "WRITE BUFFER: ", sw.c_str());
		} else  {
			bool hex = check_hex(sw.c_str(), sw.length());
			ser_trace(2, "WRITE BUFFER: ", (hex ? str2hex(sw.c_str(), sw.length()) : sw.c_str()));
		}
	}

	int ret = 0;
	if (progStatus.serial_write_delay) {
		for (int j = 0; j < n; j++) {
			WriteFile (hComm, &buff[j], 1, &nBytesWritten, NULL);
			MilliSleep(progStatus.serial_write_delay);
			ret += nBytesWritten;
		}
	} else {
		WriteFile (hComm, buff, n, &nBytesWritten, NULL);
		ret = nBytesWritten;
	}

	int timeout = progStatus.serial_post_write_delay;
	while (timeout > 0) {
		if (timeout > 10) MilliSleep(10);
		else MilliSleep(timeout);
		timeout -= 10;
		Fl::awake();
	}

	return ret;
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

	if (progStatus.serialtrace || SERIALDEBUG) {
		snprintf(traceinfo, sizeof(traceinfo), "\
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
		ser_trace(1, traceinfo);
	}

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

		if (progStatus.serialtrace || SERIALDEBUG) {
			snprintf(traceinfo, sizeof(traceinfo), "GetCommState Error on %s\n", device.c_str());
			ser_trace(1, traceinfo);
		}

		CloseHandle(hComm);
		return false;
	}

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

	if (progStatus.serialtrace || SERIALDEBUG) {
	snprintf(traceinfo, sizeof(traceinfo), "\
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
		ser_trace(1, traceinfo);
	}

	bPortReady = SetCommState(hComm, &dcb);

	if (progStatus.serialtrace || SERIALDEBUG) {
		long err = GetLastError();
		snprintf(traceinfo, sizeof(traceinfo), "SetCommState handle %u, returned %d, error = %d\n", (size_t)hComm, bPortReady, (int)err);
		ser_trace(1, traceinfo);
	}

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
bool Cserial::RTSptt(){
		return rtsptt;
	}

bool Cserial::DTRptt(){
		return dtrptt;
	}

bool Cserial::getPTT() {
	return serptt;
}

void Cserial::SetPTT(bool ON)
{
	if (hComm == INVALID_HANDLE_VALUE) {
		if (progStatus.serialtrace || SERIALDEBUG) {
			snprintf(traceinfo, sizeof(traceinfo), "SetPTT failed, invalid handle\n");
			ser_trace(1, traceinfo);
		}
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

	if (progStatus.serialtrace || SERIALDEBUG) {
		snprintf(traceinfo, sizeof(traceinfo), "\
PTT %d, DTRptt %d, DTR %d, RTSptt %d, RTS %d, RTSCTS %d, DtrControl %2x, RtsControl %2x\n",
			ON, dtrptt, dtr, rtsptt, rts, rtscts,
			static_cast<unsigned int>(dcb.fDtrControl),
			static_cast<unsigned int>(dcb.fRtsControl) );
		ser_trace(1, traceinfo);
	}
	serptt = ON;
	SetCommState(hComm, &dcb);
}

void Cserial::setDTR(bool b)
{
	if(hComm == INVALID_HANDLE_VALUE) {
		LOG_PERROR("Invalid handle");
		return;
	}
	if (b) EscapeCommFunction(hComm, SETDTR);
	else   EscapeCommFunction(hComm, CLRDTR);
}

void Cserial::setRTS(bool b)
{
	if(hComm == INVALID_HANDLE_VALUE) {
		LOG_PERROR("Invalid handle");
		return;
	}
	if (b) EscapeCommFunction(hComm, SETRTS);
	else   EscapeCommFunction(hComm, CLRRTS);
}

Cserial::Cserial() {
	rts =
	dtr =
	rtsptt =
	dtrptt =
	rtscts =
	serptt = false;
	baud = CBR_9600;
	stopbits = 2;
	hComm = INVALID_HANDLE_VALUE;
}

Cserial::Cserial( std::string portname) {
	device = portname;
	Cserial();
}

Cserial::~Cserial() {
	ClosePort();

}

#endif
