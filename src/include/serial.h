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

// SerialComm.h: interface for the Cserial class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SERIAL_H
#define SERIAL_H

#include <string>

#ifndef __WIN32__

#include <termios.h>

class Cserial  {
public:
	Cserial();
	~Cserial();

//Methods
	bool OpenPort();
	bool CheckPort(std::string);

	bool IsOpen() { return fd < 0 ? 0 : 1; };
	void ClosePort();

	void Device (std::string dev) { device = dev;};
	std::string Device() { return device;};

	void Baud(int b) { baud = b;};
	int  Baud() { return baud;};

	void Timeout(int tm) { timeout = tm;}
	int  Timeout() { return timeout; }

	void Retries(int r) { retries = r;}
	int  Retries() { return retries;}

	void RTS(bool r){rts = r;}
	bool RTS(){return rts;}
	void setRTS(bool r);

	void RTSptt(bool b){rtsptt = b;}
	bool RTSptt();

	void DTR(bool d){ dtr = d; }
	bool DTR(){return dtr;}
	void setDTR(bool d);

	void DTRptt(bool b){dtrptt = b;}
	bool DTRptt();

	void RTSCTS(bool b){rtscts = b;}
	bool RTSCTS(){return rtscts;}

	void SetPTT(bool b);
	bool getPTT();

	void Stopbits(int n) {stopbits = (n == 1 ? 1 : 2);}
	int  Stopbits() { return stopbits;}

	int  ReadBuffer (std::string &buffer, int nbr, std::string find1 = "", std::string find2 = "");
	int  WriteBuffer(const char *str, int nbr);
	bool WriteByte(char bybyte);
	void FlushBuffer();

	int  failed() { return failed_; }
	void failed(int n) {
		switch (n) {
			case -1 : if (failed_) --failed_;
					  break;
			case 0 : failed_ = 0;
					 break;
			case 1 : ++failed_;
					 break;
			default: break;
		}
	}

private:
//Members
	std::string	device;
	int		fd;
	int		baud;
	int		speed;
	struct	termios oldtio, newtio;
	int		timeout;
	int		retries;
	int		state, origstate;
	bool	dtr;
	bool	dtrptt;
	bool	rts;
	bool	rtsptt;
	bool	rtscts;
	bool	serptt;
	int		stopbits;
	char	bfr[2048];

	int		failed_;

//Methods
	bool	IOselect();

};

//=============================================================================
// MINGW serial port implemenation
//=============================================================================

#else //__WIN32__

#include <windows.h>

class Cserial  {
public:
	Cserial();
	Cserial(std::string portname);
	~Cserial();

//Methods
	bool OpenPort();
	bool IsOpen();
	bool CheckPort(std::string);
	void ClosePort();
	bool ConfigurePort(DWORD BaudRate,BYTE ByteSize,DWORD fParity,BYTE  Parity,BYTE StopBits);

	bool IsBusy() { return busyflag; };
	void IsBusy(bool val) { busyflag = val; };

	bool ReadByte(char &resp);
	DWORD GetBytesRead();

	bool WriteByte(char bybyte);
	DWORD GetBytesWritten();

	int  ReadBuffer (std::string &buffer, int nbr, std::string find1 = "", std::string find2 = "");
	int WriteBuffer(const char *str, int nbr);

	bool SetCommunicationTimeouts(DWORD ReadIntervalTimeout,DWORD ReadTotalTimeoutMultiplier,DWORD ReadTotalTimeoutConstant,DWORD WriteTotalTimeoutMultiplier,DWORD WriteTotalTimeoutConstant);
	bool SetCommTimeout();

	void Timeout(int tm) { timeout = tm; return; };
	int  Timeout() { return timeout; };
	void FlushBuffer();

	void Device (std::string dev) { device = dev;};
	std::string Device() { return device;};

	void Baud(int b) { baud = b;};
	int  Baud() { return baud;};

	void Retries(int r) { retries = r;}
	int  Retries() { return retries;}

	void RTS(bool r){rts = r;}
	bool RTS(){return rts;}
	void setRTS(bool b);

	void RTSptt(bool b){rtsptt = b;}
	bool RTSptt();

	void DTR(bool d){dtr = d;}
	bool DTR(){return dtr;}
	void setDTR(bool b);

	void DTRptt(bool b){dtrptt = b;}
	bool DTRptt();

	void RTSCTS(bool b){rtscts = b;}
	bool RTSCTS(){return rtscts;}
	void SetPTT(bool b);
	bool getPTT();

	void Stopbits(int n) {stopbits = (n == 1 ? 1 : 2);}
	int  Stopbits() { return stopbits;}

	int  failed() { return failed_; }
	void failed(int n) {
		switch (n) {
			case -1 : if (failed_) --failed_;
					  break;
			case 0 : failed_ = 0;
					 break;
			case 1 : ++failed_;
					 break;
			default: break;
		}
	}

//Members
private:
	std::string device;
	//For use by CreateFile
	HANDLE			hComm;

	//DCB Defined in WinBase.h
	DCB				dcb;
	COMMTIMEOUTS	CommTimeoutsSaved;
	COMMTIMEOUTS	CommTimeouts;

	//Is the Port Ready?
	bool			bPortReady;

	//Number of Bytes Written to port
	DWORD			nBytesWritten;

	//Number of Bytes Read from port
	DWORD			nBytesRead;

	//Number of bytes Transmitted in the cur session
	DWORD			nBytesTxD;

	int  timeout;
	bool busyflag;

	int		baud;
	int		retries;

	bool	dtr;
	bool	dtrptt;
	bool	rts;
	bool	rtsptt;
	bool	rtscts;
	bool	serptt;
	int		stopbits;

	int		failed_;
};

#endif // __WIN32__

#endif // SERIAL_H
