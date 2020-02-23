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

#include "rigbase.h"
#include "util.h"
#include "debug.h"
#include "rig_io.h"
#include "support.h"


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

	widgets = basewidgets;

	serloop_timing = 200; // msec, 5x / second

	stopbits = 2;

	CIV = 0;
	defaultCIV = 0;
	USBaudio = false;

	has_xcvr_auto_on_off =
	comm_echo =
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
	ICOMrig = false;
	ICOMmainsub = false;

	has_a2b = false;
	has_vfoAB = false;

	data_type = BINARY;

	A.freq = 14070000L;
	A.imode = 1;
	A.iBW = 0;
	B.freq = 14070000L;
	B.imode = 1;
	B.iBW = 0;
	inuse = onA;
	precision = 1;
	ndigits = 9;
	can_change_alt_vfo = false;

	freqA = 14070000L;
	modeA = 1;
	bwA = 0;
	freqB = 14070000L;
	modeB = 1;
	bwB = 0;

	def_freq = 14070000L;
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

string rigbase::to_bcd_be(long freq, int len)
{
	unsigned char a;
	int numchars = len / 2;
	string bcd = "";
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

string rigbase::to_bcd(long freq, int len)
{
	string bcd_be = to_bcd_be(freq, len);
	string bcd = "";
	int bcdlen = bcd_be.size();
	for (int i = bcdlen - 1; i >= 0; i--)
		bcd += bcd_be[i];
	return bcd;
}

long rigbase::fm_bcd (string bcd, int len)
{
	int i;
	long f = 0;
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


long rigbase::fm_bcd_be(string bcd, int len)
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

string rigbase::to_binary_be(long freq, int len)
{
	static string bin = "";
	for (int i = 0; i < len; i++) {
		bin += freq & 0xFF;
		freq >>= 8;
	}
	return bin;
}

string rigbase::to_binary(long freq, int len)
{
	static string bin = "";
	string bin_be = to_binary_be(freq, len);
	int binlen = bin_be.size();
	for (int i = binlen - 1; i >= 0; i--)
		bin += bin_be[i];
	return bin;
}

long rigbase::fm_binary(string binary, int len)
{
	int i;
	long f = 0;
	for (i = 0; i < len; i++) {
		f *= 256;
		f += (unsigned char)binary[i];
	}
	return f;
}

long rigbase::fm_binary_be(string binary_be, int len)
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

string rigbase::to_decimal_be(long d, int len)
{
	static string sdec_be;
	sdec_be.clear();
	for (int i = 0; i < len; i++) {
		sdec_be += (char)((d % 10) + '0');
		d /= 10;
	}
	return sdec_be;
}

string rigbase::to_decimal(long d, int len)
{
	static string sdec;
	sdec.clear();
	string sdec_be = to_decimal_be(d, len);
	int bcdlen = sdec_be.size();
	for (int i = bcdlen - 1; i >= 0; i--)
		sdec += sdec_be[i];
	return sdec;
}

long rigbase::fm_decimal(string decimal, int len)
{
	long d = 0;
	for (int i = 0; i < len; i++) {
		d *= 10;
		d += decimal[i] - '0';
	}
	return d;
}

long rigbase::fm_decimal_be(string decimal_be, int len)
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

string rigbase::bcd255(int val)
{
	return to_bcd(set100[(int)(val)], 3);
}

int rigbase::num100(string bcd)
{
	int val = fm_bcd(bcd, 3);
	for (int n = 0; n < 101; n++) {
		if (set100[n] > val)  return n - 1;
		if (set100[n] == val) return n;
	}
	return 0;
}
//======================================================================

int rigbase::waitN(size_t n, int timeout, const char *sz, int pr)
{
	guard_lock reply_lock(&mutex_replystr);

	char sztemp[50];
	string returned = "";
	string tosend = cmd;
	int cnt = 0;
	int waited = 0;
	size_t num = n + cmd.length();
	int delay =  num * 11000.0 / RigSerial->Baud() +
			progStatus.use_tcpip ? progStatus.tcpip_ping_delay : 0;

	replystr.clear();
	RigSerial->FlushBuffer();

	if(!progStatus.use_tcpip && !RigSerial->IsOpen()) {
		snprintf(sztemp, sizeof(sztemp), "TEST %s", sz);
		showresp(WARN, pr, sztemp, tosend, replystr);
		return 0;
	}

	sendCommand(tosend, 0);
	MilliSleep(delay);

	returned = "";
	for ( cnt = 0; cnt < timeout / 10; cnt++) {
		readResponse();
		returned.append(respstr);
		if (returned.length() >= n) {
			replystr = returned;
			waited = cnt * 10 + delay;
			snprintf(sztemp, sizeof(sztemp), "%s OK %d ms", sz, waited);
			showresp(WARN, pr, sztemp, cmd, returned);
			return replystr.length();
		}
		MilliSleep(10);
		Fl::awake();
	}

	replystr = returned;
	waited = timeout + delay;
	snprintf(sztemp, sizeof(sztemp), "%s failed %d ms", sz, waited);
	showresp(ERR, pr, sztemp, cmd, returned);
	return 0;
}

int rigbase::wait_char(int ch, size_t n, int timeout, const char *sz, int pr)
{
	guard_lock reply_lock(&mutex_replystr);

	char sztemp[50];
	string returned = "";
	string tosend = cmd;
	int cnt = 0;
	int waited = 0;
	size_t num = n + cmd.length();

	int delay =  num * 11000.0 / RigSerial->Baud() +
			progStatus.use_tcpip ? progStatus.tcpip_ping_delay : 0;

	replystr.clear();
	RigSerial->FlushBuffer();

	if(!progStatus.use_tcpip && !RigSerial->IsOpen()) {
		snprintf(sztemp, sizeof(sztemp), "TEST %s", sz);
		showresp(DEBUG, pr, sztemp, tosend, replystr);
		return 0;
	}

	sendCommand(tosend, 0);
	MilliSleep(delay);

	returned = "";
	for ( cnt = 0; cnt < timeout / 10; cnt++) {
		readResponse();
		returned.append(respstr);
		if (returned.find(ch) != string::npos) {
			replystr = returned;
			waited = cnt * 10 + delay;
			snprintf(sztemp, sizeof(sztemp), "%s OK %d ms", sz, waited);
			showresp(DEBUG, pr, sztemp, cmd, returned);
			return replystr.length();
		}
		MilliSleep(10);
		Fl::awake();
	}

	replystr = returned;
	waited = timeout + delay;
	snprintf(sztemp, sizeof(sztemp), "%s failed %d ms", sz, waited);
	showresp(ERR, pr, sztemp, cmd, returned);
	return 0;
}

// Yaesu transceiver - wait for response to identifier request
// return boolean state of response
// ID  - for most
// AI  - for FTdx9000
// wait - wait nnn milliseconds before declaring transceiver DOA
//        default 200 msec
// retry - number of retries, default 
bool rigbase::id_OK(string ID, int wait)
{
	guard_lock reply_lock(&mutex_replystr);

	string returned;
	for (int n = 0; n < progStatus.comm_retries; n++) {
		sendCommand(string(ID).append(";", 0));
		returned = "";
		for (int cnt = 0; cnt < wait / 10; cnt++) {
			readResponse();
			returned.append(respstr);
			if (returned.find(ID)) {
				replystr = returned;
				return true;
			}
			MilliSleep(10);
			Fl::awake();
		}
	}
	replystr.clear();
	return false;
}

void rigbase::sendOK(string cmd)
{
	if (IDstr.empty()) {
		sendCommand(cmd,0);
		return;
	}
	if (id_OK(IDstr, 100))
		sendCommand(cmd, 0);
}

void rigbase::set_split(bool val) 
{
	split = val;
}

int  rigbase::get_split() 
{
	return split;
}

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
