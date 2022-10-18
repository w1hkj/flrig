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

#include <iostream>

#include "rigbase.h"
#include "util.h"
#include "debug.h"
#include "rig_io.h"
#include "support.h"
#include "socket_io.h"
#include "tod_clock.h"
#include "serial.h"

#include "rigs.h"

const char *szNORIG = "NONE";
const char *szNOMODES[] = {"LSB", "USB", NULL};
const char *szNOBWS[] = {"NONE", NULL};
const char *szDSPLO[] = {"NONE", NULL};
const char *szDSPHI[] = {"NONE", NULL};
const char *szdsptooltip = "dsp tooltip";
const char *szbtnlabel = " ";
const int  ibw_val = -1;

static GUI basewidgets[] = { {NULL, 0, 0} };

rigbase::rigbase()
{
	IDstr = "";
	name_ = szNORIG;
	modes_ = szNOMODES;
	bandwidths_ = szNOBWS;
	dsp_SL = szDSPLO;
	SL_tooltip = szdsptooltip;
	SL_label = szbtnlabel;
	dsp_SH = szDSPHI;
	SH_tooltip = szdsptooltip;
	SH_label = szbtnlabel;
	bw_vals_ = &ibw_val;

	io_class = SERIAL;

	widgets = basewidgets;

	stopbits = 2;

	serial_write_delay = 0;
	serial_post_write_delay = 0;

	serloop_timing = 200; // msec, 5x / second

	CIV = 0;
	defaultCIV = 0;
	USBaudio = false;

	has_xcvr_auto_on_off =
	serial_echo =
	has_vfo_adj =
	has_rit =
	has_xit =
	has_bfo =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_bandwidth_control =
	has_dsp_controls =
	has_micgain_control =
	has_mic_line_control =
	has_auto_notch =
	has_notch_control =
	has_noise_control =
	has_noise_reduction_control =
	has_noise_reduction =
	has_attenuator_control =
	has_preamp_control =
	has_ifshift_control =
	has_pbt_controls =
	has_FILTER =
	has_ptt_control =
	has_tune_control =
	has_swr_control =
	has_alc_control =
	has_idd_control =
	has_agc_control =
	has_rf_control =
	has_sql_control =
	has_data_port =
	restore_mbw =

	has_extras =
	has_nb_level =
	has_agc_level =
	has_cw_wpm =
	has_cw_vol =
	has_cw_spot =
	has_cw_spot_tone =
	has_cw_qsk =
	has_cw_break_in =
	has_cw_delay =
	has_cw_weight =
	has_cw_keyer =
	has_vox_onoff =
	has_vox_gain =
	has_vox_anti =
	has_vox_hang =
	has_vox_on_dataport =
	has_compression =
	has_compON =
	use_line_in =
	has_bpf_center =
	has_special =
	has_ext_tuner =
	has_smeter =
	has_power_out =
	has_line_out =
//	has_split =
	has_split_AB =
	has_band_selection =
	has_get_info =
	has_getvfoAorB =
	has_voltmeter =
	ICOMrig =
	ICOMmainsub =
	can_synch_clock =
	has_a2b =
	has_vfoAB = false;

	data_type = DT_BINARY;

	A.freq = 14070000ULL;
	A.imode = 1;
	A.iBW = 0;
	B.freq = 14070000ULL;
	B.imode = 1;
	B.iBW = 0;
	inuse = onA;
	precision = 1;
	ndigits = 10;
	can_change_alt_vfo = false;

	freqA = 14070000ULL;
	modeA = 1;
	bwA = 0;
	freqB = 14070000ULL;
	modeB = 1;
	bwB = 0;

	def_freq = 14070000ULL;
	def_mode = 1;
	def_bw = 0;
	bpf_center = 0;
	pbt = 0;

	ptt_ = tune_ = 0;

	rTONE = tTONE = 8;

	max_power = 100;

	active_mode = 0; // wbx

	if_shift_min = -1500;
	if_shift_max = 1500;
	if_shift_step = 10;
	if_shift_mid = 0;

	atten_level = 0;
	preamp_level = 0;
}

std::string rigbase::to_bcd_be(unsigned long long freq, int len)
{
	unsigned char a;
	int numchars = len / 2;
	std::string bcd = "";
	if (len & 1) numchars ++;
	for (int i = 0; i < numchars; i++) {
		a = 0;
		a |= freq % 10;
		freq /= 10;
		a |= (freq % 10)<<4;
		freq /= 10;
		bcd += a;
	}
	return bcd;
}

std::string rigbase::to_bcd(unsigned long long freq, int len)
{
	std::string bcd_be = to_bcd_be(freq, len);
	std::string bcd = "";
	int bcdlen = bcd_be.size();
	for (int i = bcdlen - 1; i >= 0; i--)
		bcd += bcd_be[i];
	return bcd;
}

unsigned long long rigbase::fm_bcd (std::string bcd, int len)
{
	int i;
	unsigned long long f = 0;
	int numchars = len/2;
	if (len & 1) numchars ++;
	for (i = 0; i < numchars; i++) {
		f *=10;
		f += (bcd[i] >> 4) & 0x0F;
		f *= 10;
		f += bcd[i] & 0x0F;
	}
	return f;
}


unsigned long long rigbase::fm_bcd_be(std::string bcd, int len)
{
	char temp;
	int numchars = len/2;
	if (len & 1) numchars++;
	for (int i = 0; i < numchars / 2; i++) {
		temp = bcd[i];
		bcd[i] = bcd[numchars -1 - i];
		bcd[numchars -1 - i] = temp;
	}
	return fm_bcd(bcd, len);
}

std::string rigbase::to_binary_be(unsigned long long freq, int len)
{
	static std::string bin = "";
	for (int i = 0; i < len; i++) {
		bin += freq & 0xFF;
		freq >>= 8;
	}
	return bin;
}

std::string rigbase::to_binary(unsigned long long freq, int len)
{
	static std::string bin = "";
	std::string bin_be = to_binary_be(freq, len);
	int binlen = bin_be.size();
	for (int i = binlen - 1; i >= 0; i--)
		bin += bin_be[i];
	return bin;
}

unsigned long long rigbase::fm_binary(std::string binary, int len)
{
	int i;
	unsigned long long f = 0;
	for (i = 0; i < len; i++) {
		f *= 256;
		f += (unsigned char)binary[i];
	}
	return f;
}

unsigned long long rigbase::fm_binary_be(std::string binary_be, int len)
{
	unsigned char temp;
	int numchars = len/2;
	if (len & 1) numchars++;
	for (int i = 0; i < numchars / 2; i++) {
		temp = binary_be[i];
		binary_be[i] = binary_be[numchars -1 - i];
		binary_be[numchars -1 - i] = temp;
	}
	return fm_binary(binary_be, len);
}

std::string rigbase::to_decimal_be(unsigned long long d, int len)
{
	static std::string sdec_be;
	sdec_be.clear();
	for (int i = 0; i < len; i++) {
		sdec_be += (char)((d % 10) + '0');
		d /= 10;
	}
	return sdec_be;
}

std::string rigbase::to_decimal(unsigned long long d, int len)
{
	static std::string sdec;
	sdec.clear();
	std::string sdec_be = to_decimal_be(d, len);
	int bcdlen = sdec_be.size();
	for (int i = bcdlen - 1; i >= 0; i--)
		sdec += sdec_be[i];
	return sdec;
}

unsigned long long rigbase::fm_decimal(std::string decimal, int len)
{
	unsigned long long d = 0;
	for (int i = 0; i < len; i++) {
		d *= 10;
		d += decimal[i] - '0';
	}
	return d;
}

unsigned long long rigbase::fm_decimal_be(std::string decimal_be, int len)
{
	unsigned char temp;
	int numchars = len/2;
	if (len & 1) numchars++;
	for (int i = 0; i < numchars / 2; i++) {
		temp = decimal_be[i];
		decimal_be[i] = decimal_be[numchars -1 - i];
		decimal_be[numchars -1 - i] = temp;
	}
	return fm_decimal(decimal_be, len);
}

//======================================================================
// translation 0..255 <==> 0..100
// for Icom controls
//======================================================================

static int set100[] =
{  0,  3,  6,  8, 11, 13, 16, 18, 21, 23,
  26, 29, 31, 34, 36, 39, 41, 44, 46, 49,
  51, 54, 57, 59, 62, 64, 67, 69, 72, 74,
  77, 80, 82, 85, 87, 90, 92, 95, 97,100,
 102,105,108,110,113,115,118,120,123,125,
 128,131,133,136,138,141,143,146,148,151,
 153,156,159,161,164,166,169,171,174,176,
 179,182,184,187,189,192,194,197,199,202,
 204,207,210,212,215,217,220,222,225,227,
 230,233,235,238,240,243,245,248,250,253,255};

std::string rigbase::bcd255(int val)
{
	return to_bcd(set100[(int)(val)], 3);
}

int rigbase::num100(std::string bcd)
{
	int val = fm_bcd(bcd, 3);
	for (int n = 0; n < 101; n++) {
		if (set100[n] > val)  return n - 1;
		if (set100[n] == val) return n;
	}
	return 0;
}

int rigbase::hexval(int hex)
{
	int val = 0;
	val += 10 * ((hex >> 4) & 0x0F);
	val += hex & 0x0F;
	return val;
}

int rigbase::hex2val(std::string hexstr)
{
	return 100 * hexval(hexstr[0]) + hexval(hexstr[1]);
}


//======================================================================

int rigbase::waitN(size_t n, int timeout, const char *sz, int pr)
{
	guard_lock reply_lock(&mutex_replystr);

	size_t retnbr = 0;

	replystr.clear();

	if (progStatus.use_tcpip) {
		send_to_remote(cmd);
		MilliSleep(progStatus.tcpip_ping_delay);
		retnbr = read_from_remote(replystr);
		LOG_DEBUG ("%s: read %lu bytes, %s", sz, retnbr,
			(pr == HEX ? str2hex(replystr.c_str(), replystr.length()): replystr.c_str()));
		return (int)retnbr;
	}

	if(!RigSerial->IsOpen()) {
		LOG_DEBUG("TEST %s", sz);
		return 0;
	}

	RigSerial->FlushBuffer();

	RigSerial->WriteBuffer(cmd.c_str(), cmd.length());

	size_t tstart = zmsec();
	size_t tout = tstart + progStatus.serial_timeout; // minimum of 100 msec
	std::string tempstr;
	size_t nret;

	do {
		tempstr.clear();
		nret = RigSerial->ReadBuffer(tempstr, n - retnbr);
		if (nret) {
			for (size_t nc = 0; nc < nret; nc++)
				replystr += tempstr[nc];
			retnbr += nret;
			tout = zmsec() + progStatus.serial_timeout;
		}
		if (retnbr >= n)
			break;
		MilliSleep(1);
	} while  ( zmsec() < tout );

	static char ctrace[1000];
	memset(ctrace, 0, 1000);
	snprintf( ctrace, sizeof(ctrace), "%s: read %lu bytes in %d msec, %s", 
		sz, retnbr,
		(int)(zmsec() - tstart),
		(pr == HEX ? str2hex(replystr.c_str(), replystr.length()): replystr.c_str()) );

	if (SERIALDEBUG)
		ser_trace(1, ctrace);

	return retnbr;

}

int rigbase::wait_char(int ch, size_t n, int timeout, const char *sz, int pr)
{
	guard_lock reply_lock(&mutex_replystr);

	std::string wait_str = " ";
	wait_str[0] = ch;

	size_t retnbr = 0;

	replystr.clear();

	if (progStatus.use_tcpip) {
		send_to_remote(cmd);
		MilliSleep(progStatus.tcpip_ping_delay);
		retnbr = read_from_remote(replystr);
		LOG_DEBUG ("%s: read %lu bytes, %s", sz, retnbr, replystr.c_str());
		return retnbr;
	}

	if(!RigSerial->IsOpen()) {
		LOG_DEBUG("TEST %s", sz);
		return 0;
	}

	RigSerial->FlushBuffer();

	RigSerial->WriteBuffer(cmd.c_str(), cmd.length());

	size_t tstart = zmsec();
	size_t tout = tstart + progStatus.serial_timeout;
	std::string tempstr;
	size_t nret;

	do  {
		tempstr.clear();
		nret = RigSerial->ReadBuffer(tempstr, n - retnbr, wait_str);
		if (nret) {
			for (size_t nc = 0; nc < nret; nc++)
				replystr += tempstr[nc];
			retnbr += nret;
			tout = zmsec() + progStatus.serial_timeout;
		}
		if (retnbr >= n)
			break;

		if (replystr.find(wait_str) != std::string::npos)
			break;

		MilliSleep(1);
	} while ( zmsec() < tout );

	static char ctrace[1000];
	memset(ctrace, 0, 1000);
	snprintf( ctrace, sizeof(ctrace), "%s: read %lu bytes in %d msec, %s", 
		sz, retnbr,
		(int)(zmsec() - tstart),
		(pr == HEX ? str2hex(replystr.c_str(), replystr.length()): replystr.c_str()) );

	if (SERIALDEBUG)
		ser_trace(1, ctrace);

	LOG_DEBUG ("%s", ctrace);

	return retnbr;
}

int rigbase::wait_crlf(std::string cmd, std::string sz, int nr, int timeout, int pr)
{
	guard_lock reply_lock(&mutex_replystr);

	char crlf[3] = "\r\n";

	int retnbr = 0;

	replystr.clear();

	if (progStatus.use_tcpip) {
		send_to_remote(cmd);
		MilliSleep(progStatus.tcpip_ping_delay);
		retnbr = read_from_remote(replystr);
		LOG_DEBUG ("%s: read %d bytes, %s", sz.c_str(), retnbr, replystr.c_str());
		return retnbr;
	}

	if(!RigSerial->IsOpen()) {
		LOG_DEBUG("TEST %s", sz.c_str());
		return 0;
	}

	RigSerial->FlushBuffer();

	RigSerial->WriteBuffer(cmd.c_str(), cmd.length());

	size_t tstart = zmsec();
	size_t tout = zmsec() + timeout + progStatus.serial_timeout;
	std::string tempstr;
	int nret;

	do {
		tempstr.clear();
		nret = RigSerial->ReadBuffer(tempstr, nr - retnbr, crlf);
		if (nret) {
			replystr.append(tempstr);
			retnbr += nret;
			tout = zmsec() + timeout + progStatus.serial_timeout;
		}

		if (replystr.find(crlf) != std::string::npos)
			break;

		if (retnbr >= nr) break;

		MilliSleep(1);
	} while ( zmsec() < tout );

	static char ctrace[1000];
	memset(ctrace, 0, 1000);
	std::string srx = replystr;
	if (srx[0] == '\n') srx.replace(0,1,"<lf>");
	size_t psrx = srx.find("\r\n");
	if (psrx != std::string::npos)
		srx.replace(psrx, 2, "<cr><lf>");

	snprintf( ctrace, sizeof(ctrace), "%s: read %d bytes in %d msec, %s", 
		sz.c_str(), retnbr,
		(int)(zmsec() - tstart),
		srx.c_str());

	if (SERIALDEBUG)
		ser_trace(1, ctrace);

	LOG_DEBUG ("%s", ctrace);

	return retnbr;
}

// Yaesu transceiver - wait for response to identifier request
// return boolean state of response
// ID  - for most
// AI  - for FTdx9000
// wait - wait nnn milliseconds before declaring transceiver DOA
//        default 200 msec
// retry - number of retries, default
bool rigbase::id_OK(std::string ID, int wait)
{
	guard_lock reply_lock(&mutex_replystr);

	std::string buff;
	int retn = 0;
	size_t tout = 0;
	for (int n = 0; n < progStatus.serial_retries; n++) {

		RigSerial->FlushBuffer();
		RigSerial->WriteBuffer(cmd.c_str(), cmd.length());

		replystr.clear();
		tout = zmsec() + wait;

		do {
			MilliSleep(50);
			buff.clear();

			retn = RigSerial->ReadBuffer(buff, 10, ID, ";");
			if (retn) {
				replystr.append(buff);
				tout = zmsec() + wait;
			}
			if (replystr.rfind(ID)) {
				return true;
			}
			Fl::awake();

		} while (zmsec() < tout);
	}

	replystr.clear();
	return false;
}

void rigbase::sendOK(std::string cmd)
{
	if (IDstr.empty()) {
		sendCommand(cmd);
		return;
	}
	if (id_OK(IDstr, 100))
		sendCommand(cmd);
}

void rigbase::set_split(bool val)
{
	split = val;
}

int  rigbase::get_split()
{
	return split;
}

std::string rigbase::read_menu(int m1, int m2) { return ""; }

char bcdval[100] = {
'\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09',
'\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19',
'\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29',
'\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39',
'\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49',
'\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59',
'\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69',
'\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79',
'\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', '\x88', '\x89',
'\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', '\x98', '\x99'
};
