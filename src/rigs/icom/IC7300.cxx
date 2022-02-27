// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
// Modified: January 2017
//              Andy Stewart, KB1OIQ
// Updated: June 2018
//              Cliff Scott, AE5ZA
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
// aunsigned long int with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include <string>
#include <sstream>
#include <iostream>

#include "icom/IC7300.h"
#include "status.h"
#include "trace.h"

#include "support.h"

//=============================================================================
// IC-7300

const char IC7300name_[] = "IC-7300";

// these are only defined in this file
// undef'd at end of file
#define NUM_FILTERS 3
#define NUM_MODES  12

static int mode_filterA[NUM_MODES] = {1,1,1,1,1,1,1,1,1,1,1,1};
static int mode_filterB[NUM_MODES] = {1,1,1,1,1,1,1,1,1,1,1,1};

static int mode_bwA[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static int mode_bwB[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

static const char *szfilter[NUM_FILTERS] = {"1", "2", "3"};

enum { 
LSB7300, USB7300, AM7300, FM7300, CW7300, CWR7300, RTTY7300, RTTYR7300, 
LSBD7300, USBD7300, AMD7300, FMD7300
};

const char *IC7300modes_[] = {
	"LSB", "USB", "AM", "FM",
	"CW", "CW-R", "RTTY", "RTTY-R",
	"LSB-D", "USB-D", "AM-D", "FM-D", NULL};

char IC7300_mode_type[] = {
	'L', 'U', 'U', 'U',
	'L', 'U', 'L', 'U',
	'L', 'U', 'U', 'U' };

const char IC7300_mode_nbr[] = {
	0x00, 0x01, 0x02, 0x05, 0x03, 0x07, 0x04, 0x08,
	0x00, 0x01, 0x02, 0x05 };

const char *IC7300_ssb_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000", "3100", "3200", "3300", "3400", "3500",
"3600", NULL };
static int IC7300_bw_vals_SSB[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,
40, WVALS_LIMIT};

const char *IC7300_rtty_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", NULL };
static int IC7300_bw_vals_RTTY[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31, WVALS_LIMIT};

const char *IC7300_am_bws[] = {
"200",   "400",  "600",  "800", "1000", "1200", "1400", "1600", "1800", "2000",
"2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000",
"4200", "4400", "4600", "4800", "5000", "5200", "5400", "5600", "5800", "6000",
"6200", "6400", "6600", "6800", "7000", "7300", "7400", "7300", "7800", "8000",
"8200", "8400", "8600", "8800", "9000", "9200", "9400", "9600", "9800", "10000", NULL };
static int IC7300_bw_vals_AM[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,
40,41,42,43,44,45,46,47,48,49
WVALS_LIMIT};

const char *IC7300_fm_bws[] = { "FIXED", NULL };
static int IC7300_bw_vals_FM[] = { 1, WVALS_LIMIT};

static GUI IC7300_widgets[]= {
	{ btnVol,        2, 125,  50 },	//0
	{ sldrVOLUME,   54, 125, 156 },	//1
	{ btnAGC,        2, 145,  50 },	//2
	{ sldrRFGAIN,   54, 145, 156 },	//3
	{ sldrSQUELCH,  54, 165, 156 },	//4
	{ btnNR,         2, 185,  50 },	//5
	{ sldrNR,       54, 185, 156 },	//6
	{ btnLOCK,     214, 105,  50 },	//7
	{ sldrINNER,   266, 105, 156 },	//8
	{ btnCLRPBT,   214, 125,  50 },	//9
	{ sldrOUTER,   266, 125, 156 },	//10
	{ btnNotch,    214, 145,  50 },	//11
	{ sldrNOTCH,   266, 145, 156 },	//12
	{ sldrMICGAIN, 266, 165, 156 },	//13
	{ sldrPOWER,   266, 185, 156 },	//14
	{ NULL, 0, 0, 0 }
};

void RIG_IC7300::initialize()
{
	IC7300_widgets[0].W = btnVol;
	IC7300_widgets[1].W = sldrVOLUME;
	IC7300_widgets[2].W = btnAGC;
	IC7300_widgets[3].W = sldrRFGAIN;
	IC7300_widgets[4].W = sldrSQUELCH;
	IC7300_widgets[5].W = btnNR;
	IC7300_widgets[6].W = sldrNR;
	IC7300_widgets[7].W = btnLOCK;
	IC7300_widgets[8].W = sldrINNER;
	IC7300_widgets[9].W = btnCLRPBT;
	IC7300_widgets[10].W = sldrOUTER;
	IC7300_widgets[11].W = btnNotch;
	IC7300_widgets[12].W = sldrNOTCH;
	IC7300_widgets[13].W = sldrMICGAIN;
	IC7300_widgets[14].W = sldrPOWER;

	btn_icom_select_11->deactivate();
	btn_icom_select_12->deactivate();
	btn_icom_select_13->deactivate();

	choice_rTONE->activate();
	choice_tTONE->activate();

}

RIG_IC7300::RIG_IC7300() {
	defaultCIV = 0x94;
	adjustCIV(defaultCIV);

	name_ = IC7300name_;
	modes_ = IC7300modes_;
	bandwidths_ = IC7300_ssb_bws;
	bw_vals_ = IC7300_bw_vals_SSB;

	_mode_type = IC7300_mode_type;

	comm_baudrate = BR115200;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_echo = true;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;

	widgets = IC7300_widgets;

	def_freq = A.freq = 14070000;
	def_mode = A.imode = 9;
	def_bw = A.iBW = 34;

	B.freq = 7070000;
	B.imode = 9;
	B.iBW = 34;

	has_extras = true;

	has_cw_wpm = true;
	has_cw_spot_tone = true;
	has_cw_qsk = true;
	has_cw_break_in = true;

	has_vox_onoff = true;
	has_vox_gain = true;
	has_vox_anti = true;
	has_vox_hang = true;

	has_compON = true;
	has_compression = true;

	has_split = true;
	has_split_AB = true;

	has_micgain_control = true;
	has_bandwidth_control = true;

	has_smeter = true;
	has_voltmeter = true;

	has_power_out = true;
	has_swr_control = true;
	has_alc_control = true;
	has_sql_control = true;
	has_agc_control = true;

	has_power_control = true;
	has_volume_control = true;
	has_mode_control = true;

	has_attenuator_control = true;
	has_preamp_control = true;

	has_noise_control = true;
	has_nb_level = true;

	has_noise_reduction = true;
	has_noise_reduction_control = true;

	has_auto_notch = true;
	has_notch_control = true;

	has_pbt_controls = true;
	has_FILTER = true;

	has_rf_control = true;

	has_ptt_control = true;
	has_tune_control = true;

	has_band_selection = true;

	has_xcvr_auto_on_off = true;

	precision = 1;
	ndigits = 8;

	has_vfo_adj = true;

	can_change_alt_vfo = true;
	has_a2b = true;

	inuse = onA;

	can_synch_clock = true;

	CW_sense = 0; // CW is LSB

//	progStatus.gettrace   = 
//	progStatus.settrace   = 
//	progStatus.serialtrace =
//	progStatus.rigtrace    =
//	progStatus.xmltrace    =
//	progStatus.trace       = false;

//	progStatus.gettrace   = true;
//	progStatus.settrace   = true;
//	progStatus.serialtrace = true;
//	progStatus.rigtrace    = true;
//	progStatus.xmltrace    = true;
//	progStatus.trace       = true;

};

static inline void minmax(int min, int max, int &val)
{
	if (val > max) val = max;
	if (val < min) val = min;
}

void RIG_IC7300::selectA()
{
	cmd.assign(pre_to).append("\x07");
	cmd += '\x00';
	cmd.append(post);

	set_trace(1, "select A");
	waitFB("select A");
	seth();
	inuse = onA;
}

void RIG_IC7300::selectB()
{
	cmd.assign(pre_to).append("\x07");
	cmd += '\x01';
	cmd.append(post);

	set_trace(1, "select B");
	waitFB("select B");
	seth();
	inuse = onB;
}

//======================================================================
// IC7300 unique commands
//======================================================================

void RIG_IC7300::swapAB()
{
	cmd = pre_to;
	cmd += 0x07; cmd += 0xB0;
	cmd.append(post);

	set_trace(1, "swap AB");
	waitFB("Exchange vfos");
	seth();

	get_modeA(); // get mode to update the filter A / B usage
	get_modeB();
}

static int ret = 0;
static bool xcvr_is_on = false;

void RIG_IC7300::set_xcvr_auto_on()
{
	cmd = pre_to;
	cmd += '\x19'; cmd += '\x00';

	get_trace(1, "getID()");

	cmd.append(post);
	RigSerial->failed(0);

	if (waitFOR(8, "get ID") == false) {
		cmd.clear();
		int fes[] = { 2, 2, 2, 3, 7, 13, 25, 50, 75, 150, 150, 150 };
		if (progStatus.comm_baudrate >= 0 && progStatus.comm_baudrate <= 11) {
			cmd.append( fes[progStatus.comm_baudrate], '\xFE');
		}
		RigSerial->WriteBuffer(cmd.c_str(), cmd.length());

		cmd.assign(pre_to);
		cmd += '\x18'; cmd += '\x01';
//		set_trace(1, "power_on()");
		cmd.append(post);
		RigSerial->failed(0);

		set_trace(1, "power ON");
		if (waitFB("Power ON")) {
			seth();
			xcvr_is_on = true;
			cmd = pre_to; cmd += '\x19'; cmd += '\x00';
			get_trace(1, "getID()");
			cmd.append(post);
			int i = 0;
			for (i = 0; i < 100; i++) { // 10 second total timeout
				if (waitFOR(8, "get ID", 100) == true) {
					RigSerial->failed(0);
					return;
				}
				update_progress(i / 2);
				Fl::awake();
			}
			RigSerial->failed(0);
			return;
		}

		set_trace(1, "power_ON failed");
		RigSerial->failed(1);
		xcvr_is_on = false;
		return;
	}
	xcvr_is_on = true;
}

void RIG_IC7300::set_xcvr_auto_off()
{
	cmd.clear();
	cmd.append(pre_to);
	cmd += '\x18'; cmd += '\x00';
	cmd.append(post);
	set_trace(1, "set xcvr auto OFF");
	waitFB("Power OFF", 200);
	seth();
}

// ---------------------------------------------------------------------
// set date and time
// 1A 05 00 95 HH MM : set time
// 1A 05 00 94 YY YY MM DD : set date
// ---------------------------------------------------------------------
// dt formated as YYYYMMDD
// ---------------------------------------------------------------------
void RIG_IC7300::sync_date(char *dt)
{
	cmd.clear();
	cmd.append(pre_to);
	cmd += '\x1A'; cmd += '\x05';
	cmd += '\x00'; cmd += '\x94';
	unsigned char val;
	val = ((dt[0] - '0') << 4) + (dt[1] - '0');
	cmd += (val & 0xFF);
	val = ((dt[2] - '0') << 4) + (dt[3] - '0');
	cmd += (val & 0xFF);
	val = ((dt[4] - '0') << 4) + (dt[5] - '0');
	cmd += (val & 0xFF);
	val = ((dt[6] - '0') << 4) + (dt[7] - '0');
	cmd += (val & 0xFF);
	cmd.append(post);
	set_trace(1, "set xcvr clock");
	waitFB("set xcvr clock", 200);
	seth();
}

// ---------------------------------------------------------------------
// tm formated as HH:MM:SS
// ---------------------------------------------------------------------
void RIG_IC7300::sync_clock(char *tm)
{
	cmd.clear();
	cmd.append(pre_to);
	cmd += '\x1A'; cmd += '\x05';
	cmd += '\x00'; cmd += '\x95';
	unsigned char val;
	val = ((tm[0] - '0') << 4) + (tm[1] - '0');
	cmd += (val & 0xFF);
	val = ((tm[3] - '0') << 4) + (tm[4] - '0');
	cmd += (val & 0xFF);
	cmd.append(post);
	set_trace(1, "set xcvr clock");
	waitFB("set xcvr clock", 200);
	seth();
}


bool RIG_IC7300::check ()
{
	if (xcvr_is_on) return true;
	cmd = pre_to;
	cmd += '\x19'; cmd += '\x00';
	cmd.append(post);
	return (xcvr_is_on = waitFOR(8, "get ID"));
}

unsigned long int RIG_IC7300::get_vfoA ()
{
	std::string resp;

	cmd.assign(pre_to).append("\x25");
	resp.assign(pre_fm).append("\x25");

	if (inuse == onB) {
		cmd  += '\x01';
		resp += '\x01';
	} else {
		cmd  += '\x00';
		resp += '\x00';
	}

	cmd.append(post);

	ret = waitFOR(12, "get vfo A");
//	igett("vfo A");

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+6] == -1)
				A.freq = 0;
			else
				A.freq = fm_bcd_be(replystr.substr(p+6), 10);
		}
	}

	return A.freq;
}

void RIG_IC7300::set_vfoA (unsigned long int freq)
{
//	set_trace(1, "set_vfoA()");
	A.freq = freq;

	cmd.assign(pre_to).append("\x25");
	if (inuse == onB) cmd += '\x01';
	else      cmd += '\x00';

	cmd.append( to_bcd_be( freq, 10) );
	cmd.append( post );

	set_trace(1, "set_vfoA");
	waitFB("set vfo A");
	seth();
}

unsigned long int RIG_IC7300::get_vfoB ()
{
	std::string resp;

	cmd.assign(pre_to).append("\x25");
	resp.assign(pre_fm).append("\x25");

	if (inuse == onB) {
		cmd  += '\x00';
		resp += '\x00';
	} else {
		cmd  += '\x01';
		resp += '\x01';
	}

	cmd.append(post);

	ret = waitFOR(12, "get vfo B");
//	igett("vfo B");

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+6] == -1)
				A.freq = 0;
			else
				B.freq = fm_bcd_be(replystr.substr(p+6), 10);
		}
	}

	return B.freq;
}

void RIG_IC7300::set_vfoB (unsigned long int freq)
{
//	set_trace(1, "set_vfoB()");
	B.freq = freq;

	cmd.assign(pre_to).append("\x25");
	if (inuse == onB) cmd += '\x00';
	else      cmd += '\x01';

	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );

	set_trace(1, "set_vfoB");
	waitFB("set vfo B");
	seth();
}

// expecting
//  0  1  2  3  4  5  6  7  8  9
// FE FE E0 94 26 NN NN NN NN FD
//                |  |  |  |
//                |  |  |  |__filter setting, 01, 02, 03
//                |  |  |_____data mode, 00 - off, 01 - on
//                |  |________Mode 00 - LSB
//                |                01 - USB
//                |                02 - AM
//                |                03 - CW
//                |                04 - RTTY
//                |                05 - FM
//                |                07 - CW-R
//                |                08 - RTTY-R
//                |___________selected vfo, 00 - active, 01 - inactive

int RIG_IC7300::get_modeA()
{
	int md = 0;
	size_t p;

	std::string resp;
	cmd.assign(pre_to).append("\x26");
	resp.assign(pre_fm).append("\x26");

	if (inuse == onB)
		cmd += '\x01';
	else
		cmd += '\x00';

	cmd.append(post);

	ret = waitFOR(10, "get mode A");
	igett("mode A");

	if (ret) {
		p = replystr.rfind(resp);
		if (p == std::string::npos)
			goto end_wait_modeA;

		if (replystr[p+6] == -1) { md = A.imode = 0; }
		else {
			for (md = 0; md < LSBD7300; md++) {
				if (replystr[p+6] == IC7300_mode_nbr[md]) {
					A.imode = md;
					if (replystr[p+7] == 0x01 && A.imode < 4)
						A.imode += 8;
					if (A.imode > 11)
						A.imode = 1;
					break;
				}
			}
			A.filter = replystr[p+8];
			if (A.filter > 0 && A.filter < 4)
				mode_filterA[A.imode] = A.filter;
		}
	}

end_wait_modeA:

	get_trace(4, "get mode A [", IC7300modes_[A.imode], "] ", str2hex(replystr.c_str(), replystr.length()));

	if (A.imode == CW7300 || A.imode == CWR7300) {
		cmd.assign(pre_to).append("\x1A\x05");
		cmd += '\x00'; cmd += '\x53';
		cmd.append(post);
		resp.assign(pre_fm).append("\x1A\x05");
		resp += '\x00'; resp += '\x53';

		get_trace(1, "get_CW_sideband");
		ret = waitFOR(10, "get CW sideband");
		geth();

		if (ret) {
			p = replystr.rfind(resp);
			CW_sense = replystr[p+8];
			if (CW_sense) {
				IC7300_mode_type[CW7300] = 'U';
				IC7300_mode_type[CWR7300] = 'L';
			} else {
				IC7300_mode_type[CW7300] = 'L';
				IC7300_mode_type[CWR7300] = 'U';
			}
		}
	}

	return A.imode;
}

// LSB  USB  AM   CW  RTTY  FM  CW-R  RTTY-R  LSB-D  USB-D
//  0    1    2    3   4     5    6     7      8      9

void RIG_IC7300::set_modeA(int val)
{
	A.imode = val;
	cmd.assign(pre_to);
	cmd += '\x26';
	if (inuse == onB)
		cmd += '\x01';					// unselected vfo
	else
		cmd += '\x00';					// selected vfo
	cmd += IC7300_mode_nbr[A.imode];	// operating mode
	if (A.imode >= LSBD7300)
		cmd += '\x01';					// data mode
	else
		cmd += '\x00';
	cmd += mode_filterA[A.imode];		// filter
	cmd.append( post );

	set_trace(1, "set mode A");
	waitFB("set mode A");
	seth();
}

int RIG_IC7300::get_modeB()
{
	int md = 0;
	size_t p;

	std::string resp;
	cmd.assign(pre_to).append("\x26");
	resp.assign(pre_fm).append("\x26");

	if (inuse == onB)
		cmd += '\x00';   // active vfo
	else
		cmd += '\x01';   // inactive vfo
	cmd.append(post);

	ret = waitFOR(10, "get mode B");
	igett("mode B");

	if (ret) {
		p = replystr.rfind(resp);
		if (p == std::string::npos)
			goto end_wait_modeB;

		if (replystr[p+6] == -1) { md = filA = 0; }
		else {
			for (md = 0; md < LSBD7300; md++) {
				if (replystr[p+6] == IC7300_mode_nbr[md]) {
					B.imode = md;
					if (replystr[p+7] == 0x01 && B.imode < 4)
						B.imode += 8;
					if (B.imode > 11)
						B.imode = 1;
					break;
				}
			}
			B.filter = replystr[p+8];
		}
	}

end_wait_modeB:

	get_trace(4, "get mode B [", IC7300modes_[B.imode], "] ", str2hex(replystr.c_str(), replystr.length()));

	if (B.filter > 0 && B.filter < 4)
		mode_filterB[B.imode] = B.filter;

	if (B.imode == CW7300 || B.imode == CWR7300) {
		cmd.assign(pre_to).append("\x1A\x05");
		cmd += '\x00'; cmd += '\x53';
		cmd.append(post);
		resp.assign(pre_fm).append("\x1A\x05");
		resp += '\x00'; resp += '\x53';

		get_trace(1, "get_CW_sideband");
		ret = waitFOR(10, "get CW sideband");
		geth();

		if (ret) {
			p = replystr.rfind(resp);
			CW_sense = replystr[p+8];
			if (CW_sense) {
				IC7300_mode_type[CW7300] = 'U';
				IC7300_mode_type[CWR7300] = 'L';
			} else {
				IC7300_mode_type[CW7300] = 'L';
				IC7300_mode_type[CWR7300] = 'U';
			}
		}
	}

	return B.imode;
}

void RIG_IC7300::set_modeB(int val)
{
	B.imode = val;
	cmd.assign(pre_to);
	cmd += '\x26';
	if (inuse == onB)
		cmd += '\x00';					// selected vfo
	else
		cmd += '\x01';					// unselected vfo
	cmd += IC7300_mode_nbr[B.imode];	// operating mode
	if (B.imode >= LSBD7300)
		cmd += '\x01';					// data mode
	else
		cmd += '\x00';
	cmd += mode_filterB[B.imode];		// filter
	cmd.append( post );

	set_trace(1, "set mode B");
	waitFB("set mode B");
	seth();
}

int RIG_IC7300::get_FILT(int mode)
{
	if (inuse == onB) return mode_filterB[mode];
	return mode_filterA[mode];
}

void RIG_IC7300::set_FILT(int filter)
{
	if (filter < 1 || filter > 3)
		return;

	if (inuse == onB) {
		B.filter = filter;
		mode_filterB[B.imode] = filter;
		cmd.assign(pre_to);
		cmd += '\x26';
		cmd += '\x00';						// selected vfo
		cmd += IC7300_mode_nbr[B.imode];	// operating mode
		if (B.imode >= LSBD7300) cmd += '\x01';	// data mode
		else cmd += '\x00';
		cmd += filter;						// filter
		cmd.append( post );

		set_trace(1, "set mode/filter B");
		waitFB("set mode/filter B");
		seth();
	} else {
		A.filter = filter;
		mode_filterA[A.imode] = filter;
		cmd.assign(pre_to);
		cmd += '\x26';
		cmd += '\x00';						// selected vfo
		cmd += IC7300_mode_nbr[A.imode];	// operating mode
		if (A.imode >= LSBD7300) cmd += '\x01';	// data mode
		else cmd += '\x00';
		cmd += filter;						// filter
		cmd.append( post );

		set_trace(1, "set mode/filter A");
		waitFB("set mode/filter A");
		seth();

	}
}

const char *RIG_IC7300::FILT(int val)
{
	if (val < 1) val = 1;
	if (val > 3) val = 3;
	return(szfilter[val - 1]);
}

const char * RIG_IC7300::nextFILT()
{
	int val = A.filter;
	if (inuse == onB) val = B.filter;
	val++;
	if (val > 3) val = 1;
	set_FILT(val);
	return szfilter[val - 1];
}

void RIG_IC7300::set_FILTERS(std::string s)
{
	std::stringstream strm;
	strm << s;
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_filterA[i];
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_filterB[i];
	for (int i = 0; i < NUM_MODES; i++) {
		if (mode_filterA[i] < 1) mode_filterA[i] = 1;
		if (mode_filterA[i] > 3) mode_filterA[i] = 3;
		if (mode_filterB[i] < 1) mode_filterB[i] = 1;
		if (mode_filterB[i] > 3) mode_filterB[i] = 3;
	}
}

std::string RIG_IC7300::get_FILTERS()
{
	std::stringstream s;
	for (int i = 0; i < NUM_MODES; i++) {
		if (mode_filterA[i] < 1) mode_filterA[i] = 1;
		if (mode_filterA[i] > 3) mode_filterA[i] = 3;
		if (mode_filterB[i] < 1) mode_filterB[i] = 1;
		if (mode_filterB[i] > 3) mode_filterB[i] = 3;
	}
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_filterA[i] << " ";
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_filterB[i] << " ";
	return s.str();
}

std::string RIG_IC7300::get_BANDWIDTHS()
{
	std::stringstream s;
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwA[i] << " ";
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwB[i] << " ";
	return s.str();
}

void RIG_IC7300::set_BANDWIDTHS(std::string s)
{
	std::stringstream strm;
	strm << s;
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwA[i];
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwB[i];
}

bool RIG_IC7300::can_split()
{
	return true;
}

void RIG_IC7300::set_split(bool val)
{
//	set_trace(1, "set_split()");
	split = val;
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);

	set_trace(1, "set_split");
	waitFB(val ? "set split ON" : "set split OFF");
	seth();
}

int RIG_IC7300::get_split()
{
	int read_split = 0;
	cmd.assign(pre_to);
	cmd.append("\x0F");
	cmd.append( post );

	ret = waitFOR(7, "get split");
	igett("split");

	if (ret) {
		std::string resp = pre_fm;
		resp.append("\x0F");
		size_t p = replystr.find(resp);
		if (p != std::string::npos)
			read_split = replystr[p+5];
		if (read_split != 0xFA) // fail byte
			split = read_split;
	}

	return split;
}

int RIG_IC7300::get_bwA()
{
	if (A.imode == 3 || A.imode == 11) return 0; // FM, FM-D

	int current_vfo = inuse;
	if (current_vfo == onB) selectA();

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	int bwval = A.iBW;

	ret = waitFOR(8, "get_bwA");
	igett("bw A");

	if (ret) {
		std::string resp = pre_fm;
		resp.append("\x1A\x03");
		size_t p = replystr.find(resp);
		if (p != std::string::npos)
			bwval = fm_bcd(replystr.substr(p+6), 2);
	}
	if (bwval != A.iBW) {
		A.iBW = bwval;
		mode_bwA[A.imode] = bwval;
	}

	if (current_vfo == onB) selectB();

	return A.iBW;
}

void RIG_IC7300::set_bwA(int val)
{
//	set_trace(1, "set_bwA()");

	if (A.imode == 3 || A.imode == 11) return; // FM, FM-D

	A.iBW = val;

	int current_vfo = inuse;
	if (current_vfo == onB) selectA();

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(A.iBW, 2));
	cmd.append(post);
	set_trace(1, "set bw A");
	waitFB("set bwA");
	seth();

	mode_bwA[A.imode] = val;

	if (current_vfo == onB) selectB();
}

int RIG_IC7300::get_bwB()
{
	if (B.imode == 3 || B.imode == 11) return 0; // FM, FM-D

	int current_vfo = inuse;
	if (current_vfo == onA) selectB();

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	int bwval = B.iBW;

	ret = waitFOR(8, "get_bwB");
	igett("bw B");

	if (ret) {
		std::string resp = pre_fm;
		resp.append("\x1A\x03");
		size_t p = replystr.find(resp);
		if (p != std::string::npos)
			bwval = fm_bcd(replystr.substr(p+6), 2);
	}
	if (bwval != B.iBW) {
		B.iBW = bwval;
		mode_bwB[B.imode] = bwval;
	}

	if (current_vfo == onA) selectA();

	return B.iBW;
}

void RIG_IC7300::set_bwB(int val)
{
//	set_trace(1, "set_bwB()");
	if (B.imode == 3 || B.imode == 11) return; // FM, FM-D
	B.iBW = val;

	int current_vfo = inuse;
	if (current_vfo == onA) selectB();

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(B.iBW, 2));
	cmd.append(post);
	set_trace(1, "set bw B");
	waitFB("set bwB");
	seth();

	mode_bwB[B.imode] = val;

	if (current_vfo == onA) selectA();
}

// LSB  USB  AM   FM   CW  CW-R  RTTY  RTTY-R  LSB-D  USB-D  AM-D  FM-D
//  0    1    2    3   4     5    6     7      8      9       10    11

int RIG_IC7300::adjust_bandwidth(int m)
{
	int bw = 0;
	switch (m) {
		case 2: case 10: // AM, AM-D
			bandwidths_ = IC7300_am_bws;
			bw_vals_ = IC7300_bw_vals_AM;
			bw = 19;
			break;
		case 3: case 11: // FM, FM-D
			bandwidths_ = IC7300_fm_bws;
			bw_vals_ = IC7300_bw_vals_FM;
			bw = 0;
			break;
		case 6: case 7: // RTTY, RTTY-R
			bandwidths_ = IC7300_rtty_bws;
			bw_vals_ = IC7300_bw_vals_RTTY;
			bw = 12;
			break;
		case 4: case 5: // CW, CW -R
			bandwidths_ = IC7300_ssb_bws;
			bw_vals_ = IC7300_bw_vals_SSB;
			bw = 12;
			break;
		case 0: case 1: // LSB, USB
		case 8: case 9: // LSB-D, USB-D
		default:
			bandwidths_ = IC7300_ssb_bws;
			bw_vals_ = IC7300_bw_vals_SSB;
			bw = 34;
	}
	return bw;
}

const char ** RIG_IC7300::bwtable(int m)
{
	const char **table;
	switch (m) {
		case 2: case 10: // AM, AM-D
			table = IC7300_am_bws;
			break;
		case 3: case 11: // FM, FM-D
			table = IC7300_fm_bws;
			break;
		case 6: case 7: // RTTY, RTTY-R
			table = IC7300_rtty_bws;
			break;
		case 4: case 5: // CW, CW -R
		case 0: case 1: // LSB, USB
		case 8: case 9: // LSB-D, USB-D
		default:
			table = IC7300_ssb_bws;
	}
	return table;
}

int RIG_IC7300::def_bandwidth(int m)
{
	int bw = adjust_bandwidth(m);
	if (inuse == onB) {
		if (mode_bwB[m] == -1)
			mode_bwB[m] = bw;
		return mode_bwB[m];
	}
	if (mode_bwA[m] == -1)
		mode_bwA[m] = bw;
	return mode_bwA[m];
}

int RIG_IC7300::get_mic_gain()
{
	int val = 0;
	std::string cstr = "\x14\x0B";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);

	ret = waitFOR(9, "get mic");
	igett("mic");

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p + 6));
	}
	return val;
}

void RIG_IC7300::set_mic_gain(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(bcd255(val));
	cmd.append( post );
	set_trace(1, "set mic gain");
	waitFB("set mic gain");
	seth();
}

void RIG_IC7300::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

static int comp_level[] = {11,34,58,81,104,128,151,174,197,221,244};
void RIG_IC7300::set_compression(int on, int val)
{
	cmd = pre_to;
	cmd.append("\x16\x44");
	if (on) cmd += '\x01';
	else cmd += '\x00';
	cmd.append(post);
	set_trace(1, "set comp on/off");
	waitFB("set Comp ON/OFF");
	seth();

	if (val < 0) return;
	if (val > 10) return;

	cmd.assign(pre_to).append("\x14\x0E");
	cmd.append(to_bcd(comp_level[val], 3));
	cmd.append( post );
	set_trace(1, "set comp");
	waitFB("set comp");
	seth();
}

void RIG_IC7300::get_compression(int &on, int &val)
{
	std::string resp;

	cmd.assign(pre_to).append("\x16\x44").append(post);
	resp.assign(pre_fm).append("\x16\x44");

	get_trace(1, "get comp on/off");
	ret = waitFOR(8, "get comp on/off");
	geth();

	if (ret) {
		size_t p = replystr.find(resp);
		if (p != std::string::npos)
			on = (replystr[p+6] == 0x01);
	}

	cmd.assign(pre_to).append("\x14\x0E").append(post);
	resp.assign(pre_fm).append("\x14\x0E");

	get_trace(1, "get_compression");
	ret = waitFOR(9, "get comp level");
	geth();

	if (ret) {
		size_t p = replystr.find(resp);
		int level = 0;
		if (p != std::string::npos) {
			level = fm_bcd(replystr.substr(p+6), 3);
			for (val = 0; val < 11; val++)
				if (level <= comp_level[val]) break;
		}
	}
}

void RIG_IC7300::set_vox_onoff()
{
	if (progStatus.vox_onoff) {
		cmd.assign(pre_to).append("\x16\x46\x01");
		cmd.append( post );
		set_trace(1, "set vox on");
		waitFB("set vox ON");
		seth();
	} else {
		cmd.assign(pre_to).append("\x16\x46");
		cmd += '\x00';
		cmd.append( post );
		set_trace(1, "set vox off");
		waitFB("set vox OFF");
		seth();
	}
}

// Xcvr values range 0...255 step 1
void RIG_IC7300::get_vox_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_IC7300::set_vox_gain()
{
	int vox_gain = round((progStatus.vox_gain * 255 / 100 + 0.5));
	minmax(0, 255, vox_gain);
	cmd.assign(pre_to).append("\x14\x16");
	cmd.append(to_bcd(vox_gain, 3));
	cmd.append( post );
	set_trace(1, "set vox gain");
	waitFB("SET vox gain");
	seth();
}

// Xcvr values range 0...255 step 1
void RIG_IC7300::get_vox_anti_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_IC7300::set_vox_anti()
{
	int vox_anti = round((progStatus.vox_anti * 255 / 100 + 0.5));
	minmax(0, 255, vox_anti);
	cmd.assign(pre_to).append("\x14\x17");
	cmd.append(to_bcd(vox_anti, 3));
	cmd.append( post );
	set_trace(1, "set anti vox");
	waitFB("SET anti-vox");
	seth();
}

// VOX hang 0.0 - 2.0, step 0.1
// Xcvr values 0..20 step 1
void RIG_IC7300::get_vox_hang_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 20; step = 1;
}

void RIG_IC7300::set_vox_hang()
{
	cmd.assign(pre_to).append("\x1A\x05\x01\x91");
	cmd.append(to_bcd(progStatus.vox_hang, 2));
	cmd.append( post );
	set_trace(1, "set vox hang");
	waitFB("SET vox hang");
	seth();
}

//----------------------------------------------------------------------
// CW controls

void RIG_IC7300::get_cw_wpm_min_max(int &min, int &max)
{
	min = 6; max = 48;
}

void RIG_IC7300::set_cw_wpm()
{
	int iwpm = round((progStatus.cw_wpm - 6) * 255 / 42 + 0.5);
	minmax(0, 255, iwpm);

	cmd.assign(pre_to).append("\x14\x0C");
	cmd.append(to_bcd(iwpm, 3));
	cmd.append( post );
	set_trace(1, "set cw wpm");
	waitFB("SET cw wpm");
	seth();
}

void RIG_IC7300::set_break_in()
{
// 16 47 00 break-in off
// 16 47 01 break-in semi
// 16 47 02 break-in full

	cmd.assign(pre_to).append("\x16\x47");

	switch (progStatus.break_in) {
		case 2: cmd += '\x02'; break_in_label("FULL"); break;
		case 1: cmd += '\x01'; break_in_label("SEMI");  break;
		case 0:
		default: cmd += '\x00'; break_in_label("BK-IN");
	}
	cmd.append(post);
	set_trace(1, "set QSK");
	waitFB("SET break-in");
	seth();
}

int RIG_IC7300::get_break_in()
{
	cmd.assign(pre_to).append("\x16\x47").append(post);
	std::string resp;
	resp.assign(pre_fm);

	get_trace(1, "get_break_in()");
	ret = waitFOR(8, "get break in");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			progStatus.break_in = replystr[p+6];
			if (progStatus.break_in == 0) break_in_label("qsk");
			else  if (progStatus.break_in == 1) break_in_label("SEMI");
			else  break_in_label("FULL");
		}
	}
	return progStatus.break_in;
}

void RIG_IC7300::get_cw_qsk_min_max_step(double &min, double &max, double &step)
{
	min = 2.0; max = 13.0; step = 0.1;
}

void RIG_IC7300::set_cw_qsk()
{
	int qsk = round ((progStatus.cw_qsk - 2.0) * 255.0 / 11.0 + 0.5);
	minmax(0, 255, qsk);

	cmd.assign(pre_to).append("\x14\x0F");
	cmd.append(to_bcd(qsk, 3));
	cmd.append(post);
	set_trace(1, "set cw QSK delay");
	waitFB("Set cw qsk delay");
	seth();
}

void RIG_IC7300::get_cw_spot_tone_min_max_step(int &min, int &max, int &step)
{
	min = 300; max = 900; step = 5;
}

void RIG_IC7300::set_cw_spot_tone()
{
	cmd.assign(pre_to).append("\x14\x09"); // values 0=300Hz 255=900Hz
	int n = round((progStatus.cw_spot_tone - 300) * 255.0 / 600.0 + 0.5);
	minmax(0, 255, n);

	cmd.append(to_bcd(n, 3));
	cmd.append( post );
	set_trace(1, "set cw spot tone");
	waitFB("SET cw spot tone");
	seth();
}

void RIG_IC7300::set_cw_vol()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x24';
	cmd.append(to_bcd((int)(progStatus.cw_vol * 2.55), 3));
	cmd.append( post );
	set_trace(1, "set cw sideband volume");
	waitFB("SET cw sidetone volume");
	seth();
}

// Tranceiver PTT on/off
void RIG_IC7300::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	set_trace(1, "set PTT");
	waitFB("set ptt");
	seth();
	ptt_ = val;
}

int RIG_IC7300::get_PTT()
{
	cmd = pre_to;
	cmd += '\x1c'; cmd += '\x00';
	std::string resp = pre_fm;
	resp += '\x1c'; resp += '\x00';
	cmd.append(post);

	ret = waitFOR(8, "get PTT");
//	igett("ptt");

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			ptt_ = replystr[p + 6];
	}
	return ptt_;
}

// Volume control val 0 ... 100
void RIG_IC7300::set_volume_control(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(bcd255(val));
	cmd.append( post );
	set_trace(1, "set volume control");
	waitFB("set vol");
	seth();
}

/*

I:12:20:22: get vol ans in 0 ms, OK
cmd FE FE 7A E0 14 01 FD
ans FE FE 7A E0 14 01 FD
FE FE E0 7A 14 01 00 65 FD
 0  1  2  3  4  5  6  7  8
*/
int RIG_IC7300::get_volume_control()
{
	int val = 0;
	std::string cstr = "\x14\x01";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	get_trace(1, "get_volume_control()");
	ret = waitFOR(9, "get vol");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p + 6));
	}
	return (val);
}

void RIG_IC7300::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_IC7300::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(bcd255(val));
	cmd.append( post );

	waitFB("set power");

	isett("set pwr");

}

double RIG_IC7300::get_power_control()
{
	int val = progStatus.power_level;
	std::string cstr = "\x14\x0A";
	std::string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);

	ret = waitFOR(9, "get power");
//	igett("power control");

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p+6));
	}
	return val;
}

void RIG_IC7300::get_pc_min_max_step(double &min, double &max, double &step)
{
	min = 0; pmax = max = 100; step = 1;
}

int RIG_IC7300::get_smeter()
{
	std::string cstr = "\x15\x02";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;

	get_trace(1, "get_smeter()");
	ret = waitFOR(9, "get smeter");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr / 2.41);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

double RIG_IC7300::get_voltmeter()
{
	std::string cstr = "\x15\x15";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	int mtr = 0;
	double val = 0;

	ret = waitFOR(9, "get voltmeter");
	igett("voltmeter");

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			val = 0.026 * mtr + 9.782;
			return val;
		}
	}
	return -1;
}

struct pwrpair {int mtr; float pwr;};

static pwrpair pwrtbl[] = { 
{0, 0},
{2, 0.5},
{3, 1.1},
{4, 1.7},
{6, 2.2},
{12, 3},
{15, 3.6},
{21, 5},
{25, 5.6},
{31, 7},
{34, 7.9},
{40, 9.6},
{42, 9.9},
{43, 10},
{46, 11},
{49, 12},
{65, 15},
{83, 20},
{95, 25},
{105, 30},
{118, 35},
{126, 40},
{143, 50},
{183, 75},
{220, 100},
{255, 120} };

int RIG_IC7300::get_power_out(void)
{
	std::string cstr = "\x15\x11";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= 0;
	int val = 0;

	ret = waitFOR(9, "get power out");
	igett("power out");

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = hex2val(replystr.substr(p+6, 2));
			mtr = std::max(0, std::min(mtr, 255));
			size_t i = 0;
			for (i = 0; i < sizeof(pwrtbl) / sizeof(*pwrtbl) - 1; i++)
				if (mtr >= pwrtbl[i].mtr && mtr < pwrtbl[i+1].mtr)
					break;
			val = (int)ceil(pwrtbl[i].pwr + 
				(pwrtbl[i+1].pwr - pwrtbl[i].pwr)*(mtr - pwrtbl[i].mtr)/(pwrtbl[i+1].mtr - pwrtbl[i].mtr));
			if (val > 100) val = 100;
		}
	}
	return val;
}

struct swrpair {int mtr; float swr;};

// Table entries below correspond to SWR readings of 1.1, 1.5, 2.0, 2.5, 3.0 and infinity.
// Values are also tweaked to fit the display of the SWR meter.


static swrpair swrtbl[] = { 
	{0, 0.0},
	{48, 10.5},
	{80, 23.0}, 
	{103, 35.0},
	{120, 48.0},
	{255, 100.0 } };



int RIG_IC7300::get_swr(void)
{
	std::string cstr = "\x15\x12";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;

	ret = waitFOR(9, "get swr");
	igett("swr");

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			size_t i = 0;
			for (i = 0; i < sizeof(swrtbl) / sizeof(swrpair) - 1; i++)
				if (mtr >= swrtbl[i].mtr && mtr < swrtbl[i+1].mtr)
					break;
			if (mtr < 0) mtr = 0;
			if (mtr > 255) mtr = 255;
			mtr = (int)ceil(swrtbl[i].swr + 
				(swrtbl[i+1].swr - swrtbl[i].swr)*(mtr - swrtbl[i].mtr)/(swrtbl[i+1].mtr - swrtbl[i].mtr));

			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

int RIG_IC7300::get_alc(void)
{
	get_trace(1, "get_alc()");
	std::string cstr = "\x15\x13";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;

	ret = waitFOR(9, "get alc");
	igett("alc");

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr /1.2);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

void RIG_IC7300::set_rf_gain(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(bcd255(val));
	cmd.append( post );
	set_trace(1, "set rf gain");
	waitFB("set RF");
	seth();
}

int RIG_IC7300::get_rf_gain()
{
	int val = progStatus.rfgain;
	std::string cstr = "\x14\x02";
	std::string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);

	get_trace(1, "get_rf_gain");
	ret = waitFOR(9, "get RF");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p + 6));
	}
	return val;
}

void RIG_IC7300::get_rf_min_max_step(double &min, double &max, double &step)
{
	min = 0; max = 100; step = 1;
}

int RIG_IC7300::next_preamp()
{
	if (atten_level == 1)
		return preamp_level;
	switch (preamp_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 0;
	}
	return 0;
}

void RIG_IC7300::set_preamp(int val)
{
	if (val) {
		atten_level = 0;
		atten_label("ATT", false);
	}

	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';

	preamp_level = val;
	switch (val) {
		case 1: 
			preamp_label("Amp 1", true);
			break;
		case 2:
			preamp_label("Amp 2", true);
			break;
		case 0:
		default:
			preamp_label("PRE", false);
	}

	cmd += (unsigned char)preamp_level;
	cmd.append( post );
	set_trace(1, "set_preamp");
	waitFB(	(preamp_level == 0) ? "set Preamp OFF" :
			(preamp_level == 1) ? "set Preamp Level 1" :
			"set Preamp Level 2");
	seth();
}

int RIG_IC7300::get_preamp()
{
	std::string cstr = "\x16\x02";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	get_trace(1, "get_preamp()");
	ret = waitFOR(8, "get Preamp Level");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			preamp_level = replystr[p+6];
			if (preamp_level == 1) {
				preamp_label("Amp 1", preamp_level);
			} else if (preamp_level == 2) {
				preamp_label("Amp 2", preamp_level);
			} else {
				preamp_level = 0;
				preamp_label("PRE", preamp_level);
			}
		}
	}
	return preamp_level;
}

void RIG_IC7300::set_attenuator(int val)
{
	if (val) {
		atten_level = 1;
		atten_label("20 dB", atten_level);
		preamp_level = 0;
		preamp_label("PRE", 0);
	} else {
		atten_level = 0;
		atten_label("ATT", atten_level);
	}

	cmd = pre_to;
	cmd += '\x11';
	cmd += atten_level ? '\x20' : '\x00';
	cmd.append( post );
	set_trace(1, "set attenuator");
	waitFB("set att");
	seth();
}

int RIG_IC7300::next_attenuator()
{
	if (atten_level) return 0;
	return 1;
}

int RIG_IC7300::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	std::string resp = pre_fm;
	resp += '\x11';

	get_trace(1, "get_attenuator()");
	ret = waitFOR(7, "get ATT");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+5] == 0x20) {
				atten_level = 1;
				atten_label("20 dB", atten_level);
			} else {
				atten_level = 0;
				atten_label("ATT", atten_level);
			}
		}
	}
	return atten_level;
}

void RIG_IC7300::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	set_trace(1, "set noise");
	waitFB("set noise");
	seth();
}

int RIG_IC7300::get_noise()
{
	int val = progStatus.noise;
	std::string cstr = "\x16\x22";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);

	get_trace(1, "get_noice()");
	ret = waitFOR(8, "get noise");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			val = replystr[p+6];
		}
	}
	return val;
}

void RIG_IC7300::set_nb_level(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x12");
	cmd.append(bcd255(val));
	cmd.append( post );
	set_trace(1, "set nb level");
	waitFB("set NB level");
	seth();
}

int  RIG_IC7300::get_nb_level()
{
	int val = progStatus.nb_level;
	std::string cstr = "\x14\x12";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);

	get_trace(1, "get_nb_level()");
	ret = waitFOR(9, "get NB level");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p+6));
	}
	return val;
}

void RIG_IC7300::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	set_trace(1, "set noise reduction");
	waitFB("set NR");
	seth();
}

int RIG_IC7300::get_noise_reduction()
{
	std::string cstr = "\x16\x40";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);

	get_trace(1, "get_noise_reduction()");
	ret = waitFOR(8, "get NR");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			return (replystr[p+6] ? 1 : 0);
	}
	return progStatus.noise_reduction;
}

/*

I:12:06:50: get NR ans in 0 ms, OK
cmd FE FE 7A E0 16 40 FD
ans FE FE 7A E0 16 40 FD
FE FE E0 7A 16 40 01 FD
 0  1  2  3  4  5  6  7

I:12:06:50: get NRval ans in 0 ms, OK
cmd FE FE 7A E0 14 06 FD
ans FE FE 7A E0 14 06 FD
FE FE E0 7A 14 06 00 24 FD
 0  1  2  3  4  5  6  7  8

*/

void RIG_IC7300::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	val *= 16;
	val += 8;
	cmd.append(to_bcd(val, 3));
	cmd.append(post);
	set_trace(1, "set noise reduction val");
	waitFB("set NRval");
	seth();
}

int RIG_IC7300::get_noise_reduction_val()
{
	int val = progStatus.noise_reduction_val;
	std::string cstr = "\x14\x06";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);

	get_trace(1, "get_noise_reduction_val()");
	ret = waitFOR(9, "get NRval");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			val = fm_bcd(replystr.substr(p+6),3);
			val -= 8;
			val /= 16;
		}
	}
	return val;
}

void RIG_IC7300::set_squelch(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(bcd255(val));
	cmd.append( post );
	set_trace(1, "set squelch");
	waitFB("set Sqlch");
	seth();
}

#include <iostream>
int  RIG_IC7300::get_squelch()
{
	int val = progStatus.squelch;
	std::string cstr = "\x14\x03";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);

	get_trace(1, "get_squelch()");
	ret = waitFOR(9, "get squelch");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p+6));
	}
	return val;
}

void RIG_IC7300::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd += (unsigned char)val;
	cmd.append( post );
	set_trace(1, "set auto notch");
	waitFB("set AN");
	seth();
}

int RIG_IC7300::get_auto_notch()
{
	std::string cstr = "\x16\x41";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	get_trace(1, "get_auto_notch()");
	ret = waitFOR(8, "get AN");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+6] == 0x01) {
				auto_notch_label("AN", true);
				return true;
			} else {
				auto_notch_label("AN", false);
				return false;
			}
		}
	}
	return progStatus.auto_notch;
}

void RIG_IC7300::set_notch(bool on, int freq)
{
	int hexval;
	switch (vfo->imode) {
		default: case USB7300: case USBD7300: case RTTYR7300:
			hexval = freq - 1500;
			break;
		case LSB7300: case LSBD7300: case RTTY7300:
			hexval = 1500 - freq;
			break;
		case CW7300:
			if (CW_sense)
				hexval = freq - progStatus.cw_spot_tone;
			else
				hexval = progStatus.cw_spot_tone - freq;
			break;
		case CWR7300:
			if (CW_sense)
				hexval = progStatus.cw_spot_tone - freq;
			else
				hexval = freq - progStatus.cw_spot_tone;
			break;
	}

	hexval /= 20;
	hexval += 128;
	if (hexval < 0) hexval = 0;
	if (hexval > 255) hexval = 255;

	cmd = pre_to;
	cmd.append("\x16\x48");
	cmd += on ? '\x01' : '\x00';
	cmd.append(post);
	set_trace(1, "set_notch");
	waitFB("set notch");
	seth();

	cmd = pre_to;
	cmd.append("\x14\x0D");
	cmd.append(to_bcd(hexval,3));
	cmd.append(post);
	set_trace(1, "set notch value");
	waitFB("set notch val");
	seth();
}

bool RIG_IC7300::get_notch(int &val)
{
	bool on = false;
	val = 1500;

	std::string cstr = "\x16\x48";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	get_trace(1, "get_notch()");
	ret = waitFOR(8, "get notch");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);

		if (p == std::string::npos) {
			return 0;
		}
		on = replystr[p + 6];

		cmd = pre_to;
		resp = pre_fm;
		cstr = "\x14\x0D";
		cmd.append(cstr);
		resp.append(cstr);
		cmd.append(post);

		get_trace(1, "notch val");
		ret = waitFOR(9, "notch val");
		geth();

		if (ret) {
			size_t p = replystr.rfind(resp);
			if (p == std::string::npos) {
				return 0;
			} else {
				val = (int)ceil(fm_bcd(replystr.substr(p+6),3));
				val -= 128;
				val *= 20;
				switch (vfo->imode) {
					default: case USB7300: case USBD7300: case RTTYR7300:
						val = 1500 + val;
						break;
					case LSB: case LSBD7300: case RTTY7300:
						val = 1500 - val;
						break;
					case CW7300:
						if (CW_sense)
							val = progStatus.cw_spot_tone + val;
						else
							val = progStatus.cw_spot_tone - val;
						break;
					case CWR7300:
						if (CW_sense)
							val = progStatus.cw_spot_tone - val;
						else
							val = progStatus.cw_spot_tone + val;
						break;
				}
			}
		}
	}
	return on;
}

void RIG_IC7300::get_notch_min_max_step(int &min, int &max, int &step)
{
	switch (vfo->imode) {
		default:
		case USB7300: case USBD7300: case RTTYR7300:
		case LSB7300: case LSBD7300: case RTTY7300:
			min = 0; max = 3000; step = 20; break;
		case CW7300: case CWR7300:
			min = progStatus.cw_spot_tone - 500;
			max = progStatus.cw_spot_tone + 500;
			step = 20;
			break;
	}
}
static int agcval = 3;
int  RIG_IC7300::get_agc()
{
	std::string cstr = "\x16\x12";
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);

	std::string retstr = pre_fm;
	retstr.append(cstr);
	get_trace(1, "get_agc");
	ret = waitFOR(8, "get AGC");
	geth();
	if (ret) {
		size_t p = replystr.find(retstr);
		if (p != std::string::npos)
			agcval = replystr[p+6]; // 1 == FAST, 2 = MED, 3 = SLOW
	}
	return agcval;
}

int RIG_IC7300::incr_agc()
{
	agcval++;
	if (agcval == 4) agcval = 1;
	cmd = pre_to;
	cmd.append("\x16\x12");
	cmd += agcval;
	cmd.append(post);
	set_trace(1, "set AGC");
	waitFB("set AGC");
	seth();
	return agcval;
}


static const char *agcstrs[] = {"AGC", "FST", "MED", "SLO"};
const char *RIG_IC7300::agc_label()
{
	return agcstrs[agcval];
}

int  RIG_IC7300::agc_val()
{
	return (agcval);
}

void RIG_IC7300::set_if_shift(int val)
{
	int shift;
	sh_ = val;
	if (val == 0) sh_on_ = false;
	else sh_on_ = true;

	shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x07");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	set_trace(1, "set IF on/off");
	waitFB("set IF on/off");
	seth();

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	set_trace(1, "set IF val");
	waitFB("set IF val");
	seth();
}

bool RIG_IC7300::get_if_shift(int &val) {
	val = sh_;
	return sh_on_;
}

void RIG_IC7300::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -50;
	max = +50;
	step = 1;
}

void RIG_IC7300::set_pbt_inner(int val)
{
//	set_trace(1, "set_pbt_inner()");
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x07");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	set_trace(1, "set_pbt_inner()");
	waitFB("set PBT inner");
	seth();
}

void RIG_IC7300::set_pbt_outer(int val)
{
//	set_trace(1, "set_pbt_outer()");
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	set_trace(1, "set_pbt_outer()");
	waitFB("set PBT outer");
	seth();
}

int RIG_IC7300::get_pbt_inner()
{
	int val = 0;
	std::string cstr = "\x14\x07";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	get_trace(1, "get_pbt_inner()");
	ret = waitFOR(9, "get pbt inner");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			val = num100(replystr.substr(p+6));
			val -= 50;
		}
	}
	return val;
}

int RIG_IC7300::get_pbt_outer()
{
	int val = 0;
	std::string cstr = "\x14\x08";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	get_trace(1, "get_pbt_outer");
	ret = waitFOR(9, "get pbt outer");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			val = num100(replystr.substr(p+6));
			val -= 50;
		}
	}
	return val;
}

void RIG_IC7300::setVfoAdj(double v)
{
	vfo_ = v;
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x58';
	int vint = roundf(v * 2.55);
	cmd += (vint / 100);
	vint %= 100;
	cmd += ((vint % 10) + ((vint / 10) << 4));
	cmd.append(post);
	set_trace(1, "set VFO adjust");
	waitFB("SET vfo adjust");
	seth();

	MilliSleep(100);
	getVfoAdj();
}

double RIG_IC7300::getVfoAdj()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x58';
	cmd.append(post);

	get_trace(1, "getVFOadj");
	ret = waitFOR(11, "get vfo adj");
	geth();

	if (ret) {
		size_t p = replystr.find(pre_fm);
		if (p != std::string::npos) {
			vfo_ = 100 * (replystr[p+8] & 0x0F) + 10 * ((replystr[p+9] & 0xF0) >> 4) + (replystr[p+9] & 0x0F);
			vfo_ /= 2.55;
		}
	}
	return vfo_;
}

void RIG_IC7300::get_vfoadj_min_max_step(double &min, double &max, double &step)
{
	min = 0; 
	max = 100; 
	step = 0.3922;
}

// Read/Write band stack registers
//
// Read 23 bytes
//
//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22
// FE FE nn E0 1A 01 bd rn f5 f4 f3 f2 f1 mo fi fg t1 t2 t3 r1 r2 r3 FD
// Write 23 bytes
//
// FE FE E0 nn 1A 01 bd rn f5 f4 f3 f2 f1 mo fi fg t1 t2 t3 r1 r2 r3 FD
//
// nn - CI-V address
// bd - band selection 1/2/3
// rn - register number 1/2/3
// f5..f1 - frequency BCD reverse
// mo - mode
// fi - filter #
// fg flags: x01 use Tx tone, x02 use Rx tone, x10 data mode
// t1..t3 - tx tone BCD fwd
// r1..r3 - rx tone BCD fwd
//
// FE FE E0 94 1A 01 06 01 70 99 08 18 00 01 03 10 00 08 85 00 08 85 FD
//
// band 6; freq 0018,089,970; USB; data mode; t 88.5; r 88.5

void RIG_IC7300::get_band_selection(int v)
{
	std::string cstr = "\x1A\x01";
	std::string retstr = pre_fm;
	retstr.append(cstr);
	cmd.assign(pre_to);
	cmd.append(cstr);
	cmd += to_bcd_be( v, 2 );
	cmd += '\x01';
	cmd.append( post );

	get_trace(1, "get_band_selection()");
	ret = waitFOR(23, "get band stack");
	geth();

	if (ret) {
		size_t p = replystr.rfind(retstr);
		if (p != std::string::npos) {
			unsigned long int bandfreq = fm_bcd_be(replystr.substr(p+8, 5), 10);
			int bandmode = replystr[p+13];
			int bandfilter = replystr[p+14];
			int banddata = replystr[p+15] & 0x10;
			if ((bandmode < 4) && banddata) bandmode += 8;
			int tone = fm_bcd(replystr.substr(p+16, 3), 6);
			size_t index = 0;
			for (index = 0; index < sizeof(PL_tones) / sizeof(*PL_tones); index++)
				if (tone == PL_tones[index]) break;
			tTONE = index;
			tone = fm_bcd(replystr.substr(p+19, 3), 6);
			for (index = 0; index < sizeof(PL_tones) / sizeof(*PL_tones); index++)
				if (tone == PL_tones[index]) break;
			rTONE = index;
			if (inuse == onB) {
				set_vfoB(bandfreq);
				set_modeB(bandmode);
				set_FILT(bandfilter);
			} else {
				set_vfoA(bandfreq);
				set_modeA(bandmode);
				set_FILT(bandfilter);
			}
		}
	}
}

void RIG_IC7300::set_band_selection(int v)
{
//	set_trace(1, "set_band_selection()");
	unsigned long int freq = (inuse == onB ? B.freq : A.freq);
	int fil = (inuse == onB ? B.filter : A.filter);
	int mode = (inuse == onB ? B.imode : A.imode);

	cmd.assign(pre_to);
	cmd.append("\x1A\x01");
	cmd += to_bcd_be( v, 2 );
	cmd += '\x01';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd += IC7300_mode_nbr[mode];
	cmd += fil;
	if (mode >= 7)
		cmd += '\x10';
	else
		cmd += '\x00';
	cmd.append(to_bcd(PL_tones[tTONE], 6));
	cmd.append(to_bcd(PL_tones[rTONE], 6));
	cmd.append(post);
	set_trace(1, "set_band_selection");
	waitFB("set_band_selection");
	seth();
}

/*

rn - register number 1/2/3
f5..f1 - frequency BCD reverse
mo - mode
fi - filter #
fg flags: x01 use Tx tone, x02 use Rx tone, x10 data mode
t1..t3 - tx tone BCD fwd
r1..r3 - rx tone BCD fwd

FE FE 94 E0 1A 01 05 01 FD 

FE FE E0 94 1A 01 
05 
01 
00 00 07 14 00 
01 
03
10 
00 10 00
00 08 85 
FD

*/
