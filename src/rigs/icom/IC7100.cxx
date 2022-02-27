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
// aunsigned long int with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include "icom/IC7100.h"

#include <iostream>
#include <sstream>

#include "tod_clock.h"
#include "status.h"

#include "support.h"

//=============================================================================
// IC-7100
/*

*/


const char IC7100name_[] = "IC-7100";

static int nummodes = 14;

// these are only defined in this file
// undef'd at end of file
#define NUM_FILTERS 3
#define NUM_MODES  14

static int mode_filterA[NUM_MODES] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1};
static int mode_filterB[NUM_MODES] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1};

static int mode_bwA[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static int mode_bwB[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

static const char *szfilter[NUM_FILTERS] = {"1", "2", "3"};

enum {
	LSB7100, USB7100, AM7100, CW7100, RTTY7100,
	FM7100, WFM7100, CWR7100, RTTYR7100, DV7100,
	LSBD7100, USBD7100, AMD7100, FMD7100 };

static const char mdval[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 17, 0, 1, 2, 5};

const char *IC7100modes_[] = {
"LSB",    "USB",    "AM",   "CW", 
"RTTY",   "FM",    "WFM",   "CW-R",
"RTTY-R", "DV",  "LSB-D",  "USB-D",
"AM-D",   "FM-D", NULL};

static char IC7100_mode_type[] = {
'L',      'U',      'U',      'L',
'L',      'U',      'U',      'U',
'U',      'U',      'L',      'U',
'U',      'U' };

const char IC7100_mode_nbr[] = {
	0x00, // Select the LSB mode
	0x01, // Select the USB mode
	0x02, // Select the AM mode
	0x03, // Select the CW mode
	0x04, // Select the RTTY mode
	0x05, // Select the FM mode
	0x06, // Select the WFM mode
	0x07, // Select the CW-R mode
	0x08, // Select the RTTY-R mode
	0x17, // Select the DV mode
	0x00, // LSB-D
	0x01, // USB-D
	0x02, // AM-D
	0x05  // FM-D
};

const char *IC7100_ssb_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000", "3100", "3200", "3300", "3400", "3500",
"3600",
NULL };
static int IC7100_vals_ssb_bws[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,
41, WVALS_LIMIT};

const char *IC7100_am_bws[] = {
"200",   "400",  "600",  "800", "1000", "1200", "1400", "1600", "1800", "2000",
"2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000",
"4200", "4400", "4600", "4800", "5000", "5200", "5400", "5600", "5800", "6000",
"6200", "6400", "6600", "6800", "7000", "7100", "7400", "7100", "7800", "8000",
"8200", "8400", "8600", "8800", "9000", "9200", "9400", "9600", "9800", "10000", 
NULL };
static int IC7100_bw_vals_AM[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,
41,42,43,44,45,46,47,48,49,50,
 WVALS_LIMIT};

const char *IC7100_rtty_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700",
NULL };
static int IC7100_bw_vals_RTTY[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32, WVALS_LIMIT};

const char *IC7100_fixed_bws[] = { "FIXED", NULL };
static int IC7100_fixed_bw_vals[] = { 1, WVALS_LIMIT};

static GUI IC7100_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },	//0
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },	//1
	{ (Fl_Widget *)btnAGC,        2, 145,  50 },	//2
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 },	//3
	{ (Fl_Widget *)sldrSQUELCH,  54, 165, 156 },	//4
	{ (Fl_Widget *)btnNR,         2, 185,  50 },	//5
	{ (Fl_Widget *)sldrNR,       54, 185, 156 },	//6
	{ (Fl_Widget *)btnLOCK,     214, 105,  50 },	//7
	{ (Fl_Widget *)sldrINNER,   266, 105, 156 },	//8
	{ (Fl_Widget *)btnCLRPBT,   214, 125,  50 },	//9
	{ (Fl_Widget *)sldrOUTER,   266, 125, 156 },	//10
	{ (Fl_Widget *)btnNotch,    214, 145,  50 },	//11
	{ (Fl_Widget *)sldrNOTCH,   266, 145, 156 },	//12
	{ (Fl_Widget *)sldrMICGAIN, 266, 165, 156 },	//13
	{ (Fl_Widget *)sldrPOWER,   266, 185, 156 },	//14
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};

void RIG_IC7100::initialize()
{
	IC7100_widgets[0].W = btnVol;
	IC7100_widgets[1].W = sldrVOLUME;
	IC7100_widgets[2].W = btnAGC;
	IC7100_widgets[3].W = sldrRFGAIN;
	IC7100_widgets[4].W = sldrSQUELCH;
	IC7100_widgets[5].W = btnNR;
	IC7100_widgets[6].W = sldrNR;
	IC7100_widgets[7].W = btnLOCK;
	IC7100_widgets[8].W = sldrINNER;
	IC7100_widgets[9].W = btnCLRPBT;
	IC7100_widgets[10].W = sldrOUTER;
	IC7100_widgets[11].W = btnNotch;
	IC7100_widgets[12].W = sldrNOTCH;
	IC7100_widgets[13].W = sldrMICGAIN;
	IC7100_widgets[14].W = sldrPOWER;

	btn_icom_select_11->activate();
	btn_icom_select_12->activate();
	btn_icom_select_13->deactivate();

	choice_rTONE->activate();
	choice_tTONE->activate();

}

RIG_IC7100::RIG_IC7100() {
	defaultCIV = 0x88;
	name_ = IC7100name_;
	modes_ = IC7100modes_;
	bandwidths_ = IC7100_ssb_bws;
	bw_vals_ = IC7100_vals_ssb_bws;
	_mode_type = IC7100_mode_type;
	adjustCIV(defaultCIV);

	comm_retries = 2;
	comm_wait = 20;
	comm_timeout = 2;
	comm_echo = true;
	comm_rtscts = false;
	comm_rtsplus = true;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;

	widgets = IC7100_widgets;

	has_smeter = true;

	has_voltmeter = true;

	has_power_out = true;
	has_swr_control = true;
	has_alc_control = true;

	has_split = true;
	has_split_AB = true;

	has_mode_control = true;
	has_bandwidth_control = true;

	has_micgain_control = true;
	has_ifshift_control = true;
	has_pbt_controls = true;
	has_FILTER = true;
	has_power_control = true;
	has_volume_control = true;
	has_rf_control = true;
	has_sql_control = true;

	has_noise_control = true;
	has_noise_reduction = true;
	has_noise_reduction_control = true;

	has_auto_notch = true;
	has_notch_control = true;

	has_attenuator_control = true;
	has_preamp_control = true;

	has_ptt_control = true;
	has_tune_control = true;

	has_extras = true;

	has_cw_wpm = true;
	has_cw_spot_tone = true;
	has_cw_qsk = true;
	has_cw_break_in = true;
	has_cw_vol = true;

	has_vox_onoff = true;
	has_vox_gain = true;
	has_vox_anti = true;
	has_vox_hang = true;

	has_band_selection = true;

	has_compON = true;
	has_compression = true;
	has_vfo_adj = true;

	has_nb_level = true;

	restore_mbw = true;

	has_xcvr_auto_on_off = true;

	can_synch_clock = true;

	precision = 1;
	ndigits = 9;
	A.filter = B.filter = 1;

	def_freq = A.freq = 14070000;
	def_mode = A.imode = 1;
	def_bw = A.iBW = 34;

	B.freq = 7070000;
	B.imode = 1;
	B.iBW = 34;

	CW_sense = 0;

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

//======================================================================
// IC7100 unique commands
//======================================================================

static bool xcvr_is_on = false;

void RIG_IC7100::set_xcvr_auto_on()
{
	int nr = progStatus.comm_baudrate == 6 ? 25 :
			 progStatus.comm_baudrate == 5 ? 13 :
			 progStatus.comm_baudrate == 4 ? 7 :
			 progStatus.comm_baudrate == 3 ? 3 : 2;

	cmd.assign(pre_to);
	cmd += '\x19'; cmd += '\x00';
	cmd.append(post);

	get_trace(1, "getID()");

	if (waitFOR(8, "get ID", 100) == false) {  // xcvr must be OFF
		cmd.append(nr, '\xFE');
		cmd.append(pre_to);
		cmd += '\x18'; cmd += '\x01';
		cmd.append(post);
		RigSerial->failed(0);
		set_trace(1, "set_xcvr_auto_on()");
		if (waitFB("Power ON")) {
			seth();
			cmd = pre_to; cmd += '\x19'; cmd += '\x00';
			get_trace(1, "getID()");
			cmd.append(post);
			int i = 0;
			for (i = 0; i < 100; i++) {
				MilliSleep(50);
				if (waitFOR(8, "get ID") == true) {
					RigSerial->failed(0);
					xcvr_is_on = true;
					return;
				}
				update_progress(i);
				Fl::awake();
			}
			xcvr_is_on = false;
			RigSerial->failed(1);
			return;
		}
		seth();
		RigSerial->failed(1);
		xcvr_is_on = false;
		return;
	}
	xcvr_is_on = true;
}

void RIG_IC7100::set_xcvr_auto_off()
{
	cmd.clear();
	cmd.append(pre_to);
	cmd += '\x18'; cmd += '\x00';
	cmd.append(post);
	set_trace(1, "set_xcvr_auto_off()");
	waitFB("Power OFF", 200);
	seth();
}

bool RIG_IC7100::check ()
{
	if (xcvr_is_on) return true;
	cmd = pre_to;
	cmd += '\x19'; cmd += '\x00';
	cmd.append(post);
	set_trace(1, "get ID");
	xcvr_is_on = waitFOR(8, "get ID");
	seth();
	return xcvr_is_on;
}

static int ret = 0;

void RIG_IC7100::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x00';
	cmd.append(post);
	set_trace(1, "selectA()");
	waitFB("select A");
	seth();
	inuse = onA;
}

void RIG_IC7100::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x01';
	cmd.append(post);
	set_trace(1, "selectB()");
	waitFB("select B");
	seth();
	inuse = onB;
}

unsigned long int RIG_IC7100::get_vfoA ()
{
	if (inuse == onB) return A.freq;
	std::string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );

	get_trace(1,"get_vfoA()");
	ret = waitFOR(11, "get vfo A");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+5] == -1)
				A.freq = 0;
			else
				A.freq = fm_bcd_be(replystr.substr(p+5), 10);
		}
	}
	return A.freq;
}

void RIG_IC7100::set_vfoA (unsigned long int freq)
{
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	set_trace(1, "set_vfoA()");
	waitFB("set vfo A");
	seth();
}

unsigned long int RIG_IC7100::get_vfoB ()
{
	if (inuse == onA) return B.freq;
	std::string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );

	get_trace(1,"get_vfoB()");
	ret = waitFOR(11, "get vfo B");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+5] == -1)
				A.freq = 0;
			else
				B.freq = fm_bcd_be(replystr.substr(p+5), 10);
		}
	}
	return B.freq;
}

void RIG_IC7100::set_vfoB (unsigned long int freq)
{
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	set_trace(1, "set_vfoB()");
	waitFB("set vfo B");
	seth();
}

bool RIG_IC7100::can_split()
{
	return true;
}

void RIG_IC7100::set_split(bool val)
{
	split = val;
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	set_trace(1, "set_split()");
	waitFB(val ? "set split ON" : "set split OFF");
	seth();
}

int RIG_IC7100::get_split()
{
	int read_split = 0;
	cmd.assign(pre_to);
	cmd.append("\x0F");
	cmd.append( post );

	get_trace(1,"get_split()");
	ret = waitFOR(7, "get split");
	geth();

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

// Tranceiver PTT on/off
void RIG_IC7100::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	set_trace(1, "set_PTT()");
	waitFB("set ptt");
	ptt_ = val;
	seth();
}

int RIG_IC7100::get_PTT()
{
	cmd = pre_to;
	cmd += '\x1c'; cmd += '\x00';
	std::string resp = pre_fm;
	resp += '\x1c'; resp += '\x00';
	cmd.append(post);

	get_trace(1,"get_PTT()");
	ret = waitFOR(8, "get PTT");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			ptt_ = replystr[p + 6];
	}
	return ptt_;
}

void RIG_IC7100::set_modeA(int val)
{
	A.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC7100_mode_nbr[val];
	if (val < LSBD7100)
		cmd += A.filter;
	cmd.append( post );
	set_trace(1, "set_mode_A()");
	waitFB("set mode A");
	seth();

	if (val >= LSBD7100) {
		cmd = pre_to;
		cmd += '\x1A'; cmd += '\x06';
		cmd += '\x01';
		cmd += A.filter;
		cmd.append( post);
		set_trace(1, "set_date_mode_A()");
		waitFB("set data mode A");
		seth();
	}
}

int RIG_IC7100::get_modeA()
{
	int md = 0;
	size_t p = 0;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	std::string resp = pre_fm;
	resp += '\x04';

	get_trace(1,"get_modeA()");
	ret = waitFOR(8, "get mode A");
	geth();

	if (ret) {
		p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+5] == -1) { md = filA = 0; }
			else {
				for (md = 0; md < nummodes; md++)
					if (replystr[p+5] == IC7100_mode_nbr[md]) 
						break;
				A.filter = replystr[p+6];
				if (md == nummodes) {
					checkresponse();
					return A.imode;
				}
			}
		}
	} else {
		checkresponse();
		return A.imode;
	}

	if (md == LSB7100 || md == USB7100 || md == AM7100 || md == FM7100) {
		cmd = pre_to;
		cmd.append("\x1a\x06");
		cmd.append(post);
		resp = pre_fm;
		resp.append("\x1a\x06");

		get_trace(1,"get_digital_setting()");
		ret = waitFOR(9, "get digital setting");
		geth();

		if (ret) {
			p = replystr.rfind(resp);
			if (p != std::string::npos) {
				if ((replystr[p+6] & 0x01) == 0x01) {
					if (md == USB7100) md = USBD7100;
					else if (md == LSB7100) md = LSBD7100;
					else if (md == AM7100) md = AMD7100;
					else if (md == FM7100) md = FMD7100;
				}
			}
			if (replystr[p+7]) A.filter = replystr[p+7];
		}
	}
	A.imode = md;
	if (A.filter > 0 && A.filter < 4)
		mode_filterA[A.imode] = A.filter;

	get_trace(4, "mode_filterA[", IC7100modes_[md], "] = ", szfilter[A.filter-1]);

	if (A.imode == CW7100 || A.imode == CWR7100) {
		cmd.assign(pre_to).append("\x1A\x05");
		cmd += '\x00'; cmd += '\x32';
		cmd.append(post);
		resp.assign(pre_fm).append("\x1A\x05");
		resp += '\x00'; resp += '\x32';

		get_trace(1,"get_CW_sideband()");
		ret = waitFOR(10, "get CW sideband");
		geth();

		if (ret) {
			p = replystr.rfind(resp);
			CW_sense = replystr[p + 8];
			if (CW_sense) {
				IC7100_mode_type[CW7100] = 'U';
				IC7100_mode_type[CWR7100] = 'L';
			} else {
				IC7100_mode_type[CW7100] = 'L';
				IC7100_mode_type[CWR7100] = 'U';
			}
		}
	}

	return A.imode;
}

void RIG_IC7100::set_modeB(int val)
{
	B.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC7100_mode_nbr[val];
	if (val < LSBD7100)
		cmd += B.filter;
	cmd.append( post );
	set_trace(1, "set_mode_B()");
	waitFB("set mode B");
	seth();

	if (val >= LSBD7100) {
		cmd = pre_to;
		cmd += '\x1A'; cmd += '\x06';
		cmd += '\x01';
		cmd += B.filter;
		cmd.append( post);
		set_trace(1, "set_data_modeB()");
		waitFB("set data mode B");
		seth();
	}
}

int RIG_IC7100::get_modeB()
{
	int md = 0;
	size_t p = 0;

	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	std::string resp = pre_fm;
	resp += '\x04';

	get_trace(1,"get_modeB()");
	ret = waitFOR(8, "get mode B");
	geth();

	if (ret) {
		p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+5] == -1) { md = filB = 0; }
			else {
				for (md = 0; md < nummodes; md++)
					if (replystr[p+5] == IC7100_mode_nbr[md]) 
						break;
				if (md > 6) md -= 2;
				B.filter = replystr[p+6];
				if (md == nummodes) {
					checkresponse();
					return A.imode;
				}
			}
		}
	} else {
		checkresponse();
		return B.imode;
	}

	if (md == LSB7100 || md == USB7100 || md == AM7100 || md == FM7100) {
		cmd = pre_to;
		cmd.append("\x1a\x06");
		cmd.append(post);
		resp = pre_fm;
		resp.append("\x1a\x06");

		get_trace(1,"get_digital_setting()");
		ret = waitFOR(9, "get digital setting");
		geth();

		if (ret) {
			p = replystr.rfind(resp);
			if (p != std::string::npos) {
				if ((replystr[p+6] & 0x01) == 0x01) {
					if (md == USB7100) md = USBD7100;
					else if (md == LSB7100) md = LSBD7100;
					else if (md == AM7100) md = AMD7100;
					else if (md == FM7100) md = FMD7100;
				}
			}
			if (replystr[p+7]) B.filter = replystr[p+7];
		}
	}
	B.imode = md;
	if (B.filter > 0 && B.filter < 4)
		mode_filterA[B.imode] = B.filter;

	get_trace(4, "mode_filterB[", IC7100modes_[md], "] = ", szfilter[A.filter-1]);

	if (B.imode == CW7100 || B.imode == CWR7100) {
		cmd.assign(pre_to).append("\x1A\x05");
		cmd += '\x00'; cmd += '\x32';
		cmd.append(post);
		resp.assign(pre_fm).append("\x1A\x05");
		resp += '\x00'; resp += '\x32';

		get_trace(1,"get_CW_sideband()");
		ret = waitFOR(10, "get CW sideband");
		geth();

		if (ret) {
			p = replystr.rfind(resp);
			CW_sense = replystr[p + 8];
			if (CW_sense) {
				IC7100_mode_type[CW7100] = 'U';
				IC7100_mode_type[CWR7100] = 'L';
			} else {
				IC7100_mode_type[CW7100] = 'L';
				IC7100_mode_type[CWR7100] = 'U';
			}
		}
	}

	return B.imode;
}

int RIG_IC7100::get_bwA()
{
	if (A.imode == DV7100 ||
		A.imode == FM7100 || A.imode == WFM7100 || A.imode == FMD7100)
		return 0;

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	std::string resp = pre_fm;
	resp.append("\x1a\x03");

	get_trace(1,"get_bwA()");
	ret = waitFOR(8, "get bw A");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		A.iBW = fm_bcd(replystr.substr(p+6), 2);
	}
	return A.iBW;
}

void RIG_IC7100::set_bwA(int val)
{
	A.iBW = val;
	if (A.imode == DV7100 ||
		A.imode == FM7100 ||
		A.imode == WFM7100 ||
		A.imode == FMD7100) return;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(A.iBW, 2));
	cmd.append(post);
	waitFB("set bw A");
}

int RIG_IC7100::get_bwB()
{
	if (B.imode == DV7100 ||
		B.imode == FM7100 || B.imode == WFM7100 || B.imode == FMD7100)
		return 0;

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	std::string resp = pre_fm;
	resp.append("\x1a\x03");

	get_trace(1,"get_bwB()");
	ret = waitFOR(8, "get bw B");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		B.iBW = fm_bcd(replystr.substr(p+6), 2);
	}
	return B.iBW;
}

void RIG_IC7100::set_bwB(int val)
{
	B.iBW = val;
	if (B.imode == DV7100 ||
		B.imode == FM7100 ||
		B.imode == WFM7100 ||
		B.imode == FMD7100) return;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(B.iBW, 2));
	cmd.append(post);
	waitFB("set bw B");
}

std::string RIG_IC7100::get_BANDWIDTHS()
{
	std::stringstream s;
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwA[i] << " ";
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwB[i] << " ";
	return s.str();
}

void RIG_IC7100::set_BANDWIDTHS(std::string s)
{
	std::stringstream strm;
	strm << s;
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwA[i];
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwB[i];
}

int RIG_IC7100::adjust_bandwidth(int m)
{
	switch (m) {
		case AM7100: case AMD7100:
			bandwidths_ = IC7100_am_bws;
			bw_vals_ = IC7100_bw_vals_AM;
			return 29;
			break;
		case USB7100: case LSB7100: case USBD7100: case LSBD7100:
			bandwidths_ = IC7100_ssb_bws;
			bw_vals_ = IC7100_vals_ssb_bws;
			return 34;
			break;
		case DV7100:
		case FM7100: case WFM7100: case FMD7100:
			bandwidths_ = IC7100_fixed_bws;
			bw_vals_ = IC7100_fixed_bw_vals;
			return 0;
			break;
		case RTTY7100: case RTTYR7100:
			bandwidths_ = IC7100_rtty_bws;
			bw_vals_ = IC7100_bw_vals_RTTY;
			return 12;
			break;
		case CW7100: case CWR7100:
			bandwidths_ = IC7100_ssb_bws;
			bw_vals_ = IC7100_vals_ssb_bws;
			return 10;
			break;
		default:
			bandwidths_ = IC7100_ssb_bws;
			bw_vals_ = IC7100_vals_ssb_bws;
			return 34;
	}
	return 0;
}

const char ** RIG_IC7100::bwtable(int m)
{
	const char **table;
	switch (m) {
		case AM7100: case AMD7100:
			table = IC7100_am_bws;
			break;
		case DV7100:
		case FM7100: case WFM7100: case FMD7100:
			table = IC7100_fixed_bws;
			break;
		case RTTY7100: case RTTYR7100:
			table = IC7100_rtty_bws;
			break;
		case CW7100: case CWR7100:
		case USB7100: case LSB7100: case USBD7100: case LSBD7100:
		default:
			table = IC7100_ssb_bws;
	}
	return table;
}

int RIG_IC7100::def_bandwidth(int m)
{
	return adjust_bandwidth(m);
}

void RIG_IC7100::set_mic_gain(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(bcd255(val));
	cmd.append( post );
	set_trace(1, "set_mic_gain()");
	waitFB("set mic gain");
	seth();
}

int RIG_IC7100::get_mic_gain()
{
	int val = progStatus.volume;
	std::string cstr = "\x14\x0B";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	get_trace(1,"get_mic_gain()");
	ret = waitFOR(9, "get mic gain");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p+6));
	}
	return val;
}

void RIG_IC7100::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}
void RIG_IC7100::set_attenuator(int val)
{
   if (val) {
		atten_label("12 dB", true);
		atten_level = 1;
		preamp_label("PRE", false);
	} else {
		atten_level = 0;
		atten_label("ATT", false);
	}

	cmd = pre_to;
	cmd += '\x11';
	cmd += atten_level ? '\x12' : '\x00';
	cmd.append( post );
	set_trace(1, "set_attenuator()");
	waitFB("set att");
	seth();
}

int RIG_IC7100::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	std::string resp = pre_fm;
	resp += '\x11';

	get_trace(1,"get_attenuator()");
	ret = waitFOR(7, "get ATT");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (replystr[p+5] == 0x12) {
			atten_level = 1;
			atten_label("12 dB", true);
		} else {
			atten_level = 0;
			atten_label("ATT", false);
		}
	}
	return atten_level;
}

int RIG_IC7100::next_preamp()
{
	if (inuse == onA) {
		if (A.freq > 100000000) {
			if (preamp_level) return 0;
			return 1;
		} else {
			switch (preamp_level) {
			case 0: return 1;
			case 1: return 2;
			case 2: return 0;
			}
		}
	} else {
		if (B.freq > 100000000) {
			if (preamp_level) return 0;
			return 1;
		} else {
			switch (preamp_level) {
			case 0: return 1;
			case 1: return 2;
			case 2: return 0;
			}
		}
	}
	return 0;
}

void RIG_IC7100::set_preamp(int val)
{
	if (val) {
		atten_level = 0;
		atten_label("ATT", false);
	}

	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';

	preamp_level = val;
	if (inuse == onA && A.freq > 100000000 && val > 1) val = 1;
	if (inuse == onB && B.freq > 100000000 && val > 1) val = 1;
	switch (val) {
		case 1: 
			preamp_label("Pre 1", true);
			break;
		case 2:
			preamp_label("Pre 2", true);
			break;
		case 0:
		default:
			preamp_label("Pre", false);
	}

	cmd += (unsigned char)preamp_level;
	cmd.append( post );
	set_trace(1, "set_preamp()");
	waitFB(	(preamp_level == 0) ? "set Preamp OFF" :
			(preamp_level == 1) ? "set Preamp Level 1" :
			"set Preamp Level 2");
	seth();
}

int RIG_IC7100::get_preamp()
{
	std::string cstr = "\x16\x02";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	get_trace(1,"get_preamp()");
	ret = waitFOR(8, "get preamp");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			preamp_level = replystr[p+6];
		if (preamp_level == 1) {
			preamp_label("Pre 1", true);
		} else if (preamp_level == 2) {
			preamp_label("Pre 2", true);
		} else {
			preamp_label("Pre", false);
			preamp_level = 0;
		}
	}
	return preamp_level;
}

void RIG_IC7100::set_compression(int on, int val)
{
	if (on) {
		cmd.assign(pre_to).append("\x14\x0E");
		cmd.append(to_bcd(val * 255 / 100, 3));
		cmd.append( post );
		waitFB("set comp");

		cmd = pre_to;
		cmd.append("\x16\x44");
		cmd += '\x01';
		cmd.append(post);
		waitFB("set Comp ON");

	} else{
		cmd.assign(pre_to).append("\x16\x44");
		cmd += '\x00';
		cmd.append(post);
		waitFB("set Comp OFF");
	}
}

void RIG_IC7100::set_vox_onoff()
{
	if (progStatus.vox_onoff) {
		cmd.assign(pre_to).append("\x16\x46\x01");
		cmd.append( post );
		waitFB("set vox ON");
	} else {
		cmd.assign(pre_to).append("\x16\x46");
		cmd += '\x00';
		cmd.append( post );
		waitFB("set vox OFF");
	}
}

void RIG_IC7100::set_vox_gain()
{
	cmd.assign(pre_to).append("\x1A\x05"); // values 0-255 = 0 - 100%
	cmd +='\x01';
	cmd +='\x63';
	cmd.append(to_bcd((int)(progStatus.vox_gain * 2.55), 3));
	cmd.append( post );
	waitFB("SET vox gain");
}

void RIG_IC7100::set_vox_anti()
{
	cmd.assign(pre_to).append("\x1A\x05");	// values 0-255 = 0 - 100%
	cmd +='\x01';
	cmd +='\x64';
	cmd.append(to_bcd((int)(progStatus.vox_anti * 2.55), 3));
	cmd.append( post );
	waitFB("SET anti-vox");
}

void RIG_IC7100::set_vox_hang()
{
	cmd.assign(pre_to).append("\x1A\x05");	// values 00-20 = 0.0 - 2.0 sec
	cmd +='\x01';
	cmd +='\x65';
	cmd.append(to_bcd((int)(progStatus.vox_hang / 10 ), 2));
	cmd.append( post );
	waitFB("SET vox hang");
}

// CW controls

void RIG_IC7100::set_cw_wpm()
{
	cmd.assign(pre_to).append("\x14\x0C"); // values 0-255 = 6 to 48 WPM
	cmd.append(to_bcd(round((progStatus.cw_wpm - 6) * 255 / (48 - 6)), 3));
	cmd.append( post );
	waitFB("SET cw wpm");
}

void RIG_IC7100::set_cw_qsk()
{
	int n = round(progStatus.cw_qsk * 10); // values 0-255
	cmd.assign(pre_to).append("\x14\x0F");
	cmd.append(to_bcd(n, 3));
	cmd.append(post);
	waitFB("Set cw qsk delay");
}

void RIG_IC7100::set_cw_spot_tone()
{
	cmd.assign(pre_to).append("\x14\x09"); // values 0=300Hz 255=900Hz
	int n = round((progStatus.cw_spot_tone - 300) * 255.0 / 600.0);
	if (n > 255) n = 255;
	if (n < 0) n = 0;
	cmd.append(to_bcd(n, 3));
	cmd.append( post );
	waitFB("SET cw spot tone");
}

void RIG_IC7100::set_cw_vol()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x01';
	cmd += '\x32';
	cmd.append(to_bcd((int)(progStatus.cw_vol * 2.55), 3));
	cmd.append( post );
	set_trace(1, "set_cw_vol()");
	waitFB("SET cw sidetone volume");
	seth();
}

void RIG_IC7100::set_break_in()
{
// 16 47 00 break-in off
// 16 47 01 break-in on

	cmd.assign(pre_to).append("\x16\x47");

	switch (progStatus.break_in) {
		case 1: cmd += '\x01'; break_in_label("QSK ON");  break;
		case 0:
		default: cmd += '\x00'; break_in_label("qsk");
	}
	cmd.append(post);
	waitFB("SET break-in");
}

int RIG_IC7100::get_break_in()
{
	cmd.assign(pre_to).append("\x16\x47").append(post);
	std::string resp;
	resp.assign(pre_fm);

	get_trace(1,"get_break_in()");
	ret = waitFOR(8, "get break in");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			progStatus.break_in = replystr[p+6];
			if (progStatus.break_in == 0) break_in_label("qsk");
			else  break_in_label("QSK ON");
		}
	}
	return progStatus.break_in;
}

void RIG_IC7100::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(bcd255(val));
	cmd.append( post );
	set_trace(1, "set_power_control()");
	waitFB("set power");
	seth();
}

double RIG_IC7100::get_power_control()
{
	int val = progStatus.power_level;
	std::string cstr = "\x14\x0A";
	std::string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);

	get_trace(1,"get_power_control()");
	ret = waitFOR(9, "get power");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p+6));
	}
	return val;
}

void RIG_IC7100::get_pc_min_max_step(double &min, double &max, double &step)
{
	min = 0; pmax = max = 100; step = 1;
}

void RIG_IC7100::set_rf_gain(int val)
{
	if (inuse == onA && (A.imode == DV7100 ||
		A.imode == FM7100 || A.imode == WFM7100 || A.imode == FMD7100))
		return;
	if (inuse == onB && (A.imode == DV7100 ||
		A.imode == FM7100 || A.imode == WFM7100 || A.imode == FMD7100))
		return;

	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(bcd255(val));
	cmd.append( post );
	set_trace(1, "set_rf_gain()");
	waitFB("set RF");
	seth();
}

int RIG_IC7100::get_rf_gain()
{
	if (inuse == onA && (A.imode == DV7100 ||
		A.imode == FM7100 || A.imode == WFM7100 || A.imode == FMD7100))
		return progStatus.rfgain;
	if (inuse == onB && (A.imode == DV7100 ||
		A.imode == FM7100 || A.imode == WFM7100 || A.imode == FMD7100))
		return progStatus.rfgain;

	int val = progStatus.rfgain;
	std::string cstr = "\x14\x02";
	std::string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);

	get_trace(1,"get_rf_gain()");
	ret = waitFOR(9, "get RF");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p + 6));
	}
	return val;
}

void RIG_IC7100::get_rf_min_max_step(double &min, double &max, double &step)
{
	min = 0; max = 100; step = 1;
}

// Volume control val 0 ... 100
void RIG_IC7100::set_volume_control(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(bcd255(val));
	cmd.append( post );
	set_trace(1, "set_volume_control()");
	waitFB("set vol");
	seth();
}

int RIG_IC7100::get_volume_control()
{
	int val = progStatus.volume;
	std::string cstr = "\x14\x01";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	get_trace(1,"get_volume_control()");
	ret = waitFOR(9, "get vol");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p+6));
	}
	return val;
}

void RIG_IC7100::set_squelch(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(bcd255(val));
	cmd.append( post );
	set_trace(1, "set_squelch()");
	waitFB("set Sqlch");
	seth();
}

int  RIG_IC7100::get_squelch()
{
	int val = progStatus.squelch;
	std::string cstr = "\x14\x03";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);

	get_trace(1,"get_squelch()");
	ret = waitFOR(9, "get squelch");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p+6));
	}
	return val;
}

void RIG_IC7100::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

struct meterpair {int mtr; float val;};

static meterpair smtrtbl[] = {
{0, 0},
{120, 50},
{241, 100} };

int RIG_IC7100::get_smeter()
{
	std::string cstr = "\x15\x02";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;

	get_trace(1,"get_smeter()");
	ret = waitFOR(9, "get smeter");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			size_t i = 0;
			for (i = 0; i < sizeof(smtrtbl) / sizeof(meterpair) - 1; i++)
				if (mtr >= smtrtbl[i].mtr && mtr < smtrtbl[i+1].mtr)
					break;
			if (mtr < 0) mtr = 0;
			if (mtr > 255) mtr = 255;
			mtr = (int)ceil(smtrtbl[i].val + 
				(smtrtbl[i+1].val - smtrtbl[i].val)*(mtr - smtrtbl[i].mtr)/(smtrtbl[i+1].mtr - smtrtbl[i].mtr));
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

double RIG_IC7100::get_voltmeter()
{
	std::string cstr = "\x15\x15";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	int mtr = 0;
	double val = 0;

	get_trace(1, "get_voltmeter()");
	ret = waitFOR(9, "get voltmeter");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			val = (2.0 * mtr / 77.0) + 9.82;
			return val;
		}
	}
	return -1;
}


static meterpair pwrtbl[] = { 
{0, 0.0},
{40, 10.0},
{76, 20.0},
{92, 25.0},
{103, 30.0},
{124, 40.0},
{143, 50.0},
{183, 75.0},
{213, 100.0},
{255, 140.0} };

int RIG_IC7100::get_power_out(void)
{
	std::string cstr = "\x15\x11";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr = 0;
	int val = 0;

	get_trace(1,"get_power_out()");
	ret = waitFOR(9, "get power out");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = hex2val(replystr.substr(p+6, 2));
			mtr = std::max(0, std::min(mtr, 255));
			size_t i = 0;
			for (i = 0; i < sizeof(pwrtbl) / sizeof(meterpair) - 1; i++)
				if (mtr >= pwrtbl[i].mtr && mtr < pwrtbl[i+1].mtr)
					break;
			val = (int)ceil(pwrtbl[i].val + 
				(pwrtbl[i+1].val - pwrtbl[i].val)*(mtr - pwrtbl[i].mtr)/(pwrtbl[i+1].mtr - pwrtbl[i].mtr));
			if (val > 100) val = 100;
		}
	}
	return val;
}

int RIG_IC7100::get_swr()
{
	std::string cstr = "\x15\x12";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;

	get_trace(1,"get_swr()");
	ret = waitFOR(9, "get SWR");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr /2.40);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

int RIG_IC7100::get_alc()
{
	std::string cstr = "\x15\x13";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;

	get_trace(1,"get_alc()");
	ret = waitFOR(9, "get alc");
	geth();

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

void RIG_IC7100::set_notch(bool on, int freq)
{
	int hexval;
	switch (vfo->imode) {
		default: case USB7100: case USBD7100: case RTTYR7100:
			hexval = freq - 1500;
			break;
		case LSB7100: case LSBD7100: case RTTY7100:
			hexval = 1500 - freq;
			break;
		case CW7100:
			if (CW_sense)
				hexval = freq - progStatus.cw_spot_tone;
			else
				hexval = progStatus.cw_spot_tone - freq;
			break;
		case CWR7100:
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
	set_trace(1, "set_notch()");
	waitFB("set notch");
	seth();

	cmd = pre_to;
	cmd.append("\x14\x0D");
	cmd.append(to_bcd(hexval,3));
	cmd.append(post);
	set_trace(1, "set_notch_val()");
	waitFB("set notch val");
	seth();
}

bool RIG_IC7100::get_notch(int &val)
{
	bool on = false;
	val = 1500;

	std::string cstr = "\x16\x48";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	get_trace(1,"get_notch()");
	ret = waitFOR(8, "get notch");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			on = replystr[p + 6];
		cmd = pre_to;
		resp = pre_fm;
		cstr = "\x14\x0D";
		cmd.append(cstr);
		resp.append(cstr);
		cmd.append(post);

		get_trace(1, "get_notch_val()");
		ret = waitFOR(9, "notch val");
		geth();

		if (ret) {
			size_t p = replystr.rfind(resp);
			if (p != std::string::npos) {
				val = (int)ceil(fm_bcd(replystr.substr(p+6),3));
				val -= 128;
				val *= 20;
				switch (vfo->imode) {
					default: case USB7100: case USBD7100: case RTTYR7100:
						val = 1500 + val;
						break;
					case LSB: case LSBD7100: case RTTY7100:
						val = 1500 - val;
						break;
					case CW7100:
						if (CW_sense)
							val = progStatus.cw_spot_tone + val;
						else
							val = progStatus.cw_spot_tone - val;
						break;
					case CWR7100:
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

void RIG_IC7100::get_notch_min_max_step(int &min, int &max, int &step)
{
	switch (vfo->imode) {
		default:
		case USB7100: case USBD7100: case RTTYR7100:
		case LSB7100: case LSBD7100: case RTTY7100:
			min = 0; max = 3000; step = 20; break;
		case CW7100: case CWR7100:
			min = progStatus.cw_spot_tone - 500;
			max = progStatus.cw_spot_tone + 500;
			step = 20;
			break;
	}
}

void RIG_IC7100::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd += val ? 0x01 : 0x00;
	cmd.append( post );
	waitFB("set AN");
}

int RIG_IC7100::get_auto_notch()
{
	std::string cstr = "\x16\x41";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	get_trace(1,"get_auto_notch()");
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

void RIG_IC7100::set_if_shift(int val)
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
	waitFB("set IF inner");

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set IF outer");
}

bool RIG_IC7100::get_if_shift(int &val)
{
	val = sh_;
	return sh_on_;
}

void RIG_IC7100::set_pbt_inner(int val)
{
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x07");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set PBT inner");
}

void RIG_IC7100::set_pbt_outer(int val)
{
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set PBT outer");
}

int RIG_IC7100::get_pbt_inner()
{
	int val = 0;
	std::string cstr = "\x14\x07";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	get_trace(1,"get_pbt_inner()");
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

int RIG_IC7100::get_pbt_outer()
{
	int val = 0;
	std::string cstr = "\x14\x08";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	get_trace(1,"get_pbt_outer()");
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

void RIG_IC7100::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -50;
	max = +50;
	step = 1;
}

void RIG_IC7100::set_noise(bool val)
{  
   cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	set_trace(1, "set)noise()");
	waitFB("set noise");
	seth();
}

int RIG_IC7100::get_noise()
{
	std::string cstr = "\x16\x22";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);

	get_trace(1,"get_noise()");
	ret = waitFOR(8, "get noise");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			return (replystr[p+6] ? 1 : 0);
	}
	return progStatus.noise;
}


void RIG_IC7100::set_nb_level(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x12");
	cmd.append(bcd255(val));
	cmd.append( post );
	set_trace(1, "set_nb_level()");
	waitFB("set NB level");
	seth();
}

int  RIG_IC7100::get_nb_level()
{
	int val = progStatus.nb_level;
	std::string cstr = "\x14\x12";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);

	get_trace(1,"get_nb_level()");
	ret = waitFOR(9, "get NB level");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p+6));
	}
	return val;
}

void RIG_IC7100::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	set_trace(1, "set_noise_reduction()");
	waitFB("set NR");
	seth();
}

int RIG_IC7100::get_noise_reduction()
{
	std::string cstr = "\x16\x40";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);

	get_trace(1,"get_noise_reduction()");
	ret = waitFOR(8, "get NR");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			return (replystr[p+6] ? 1 : 0);
	}
	return progStatus.noise_reduction;
}

void RIG_IC7100::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	val *= 16;
	val += 8;
	cmd.append(to_bcd(val, 3));
	cmd.append(post);
	set_trace(1, "set_noise_reduction_val");
	waitFB("set NRval");
	seth();
}

int RIG_IC7100::get_noise_reduction_val()
{
	int val = 0;
	std::string cstr = "\x14\x06";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);

	get_trace(1,"get_noise_reduction_val()");
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

// Read/Write band stack registers
//
//
// ( 0) xFE xFE x88 xE0 x1A x01 band stack command
// ( 6) x01                 frequency band code
// ( 7) x01                 register code
// ( 8) x00 x00 x91 x01 x00 frequency
// (13) x03 x02             operating mode / filter setting
// (15) x00                 data mode 0x00 / 0x01
// (16) x00                 duplex tone setting
// (17) x00                 digital squelch setting
// (18) x00 x08 x85         repeater tone frequency
// (21) x00 x08 x85         tone squelch frequency
// (24) x00 x00 x23         dtcs code
// (27) x00                 digital squelch code
// (28) x00 x50 x00         duplex offset frequency
// (31) x43 x51 x43 x51 x43 x51 x20 x20     destination call, 8 chars "CKCKCK  "
// (39) x20 x20 x20 x20 x20 x20 x20 x20 x20 x20 x20 x20 x20 x20 x20 x20   memory name, 16 chars
// (55) xFD

void RIG_IC7100::get_band_selection(int v)
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x01");
	cmd += to_bcd_be( v, 2 );
	cmd += '\x01';
	cmd.append( post );

	get_trace(1,"get_band_selection()");
	ret = waitFOR(56, "get band stack");
	geth();

	if (ret) {
		size_t p = replystr.rfind(pre_fm);
		if (p != std::string::npos) {
			unsigned long int bandfreq = fm_bcd_be(replystr.substr(p + 8, 5), 10);
			int bandmode = replystr[p+13];
			int bandfilter = replystr[p+14];
			int banddata = replystr[p+15] & 0x10;
			if ((bandmode == 0) && banddata) bandmode = 10;
			if ((bandmode == 1) && banddata) bandmode = 11;
			if ((bandmode == 2) && banddata) bandmode = 12;
			if ((bandmode == 5) && banddata) bandmode = 13;
			int tone = fm_bcd(replystr.substr(p + 18, 3), 6);
			tTONE = 0;
			for (size_t n = 0; n < sizeof(PL_tones) / sizeof(*PL_tones); n++) {
				if (tone == PL_tones[n]) {
					tTONE = n;
					break;
				}
			}
			tone = fm_bcd(replystr.substr(p + 21, 3), 6);
			rTONE = 0;
			for (size_t n = 0; n < sizeof(PL_tones) / sizeof(*PL_tones); n++) {
				if (tone == PL_tones[n]) {
					rTONE = n;
					break;
				}
			}
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

void RIG_IC7100::set_band_selection(int v)
{
	unsigned long int freq = (inuse == onB ? B.freq : A.freq);
	int fil = (inuse == onB ? B.filter : A.filter);
	int mode = (inuse == onB ? B.imode : A.imode);

	cmd.assign(pre_to);
	cmd.append("\x1A\x01");
	cmd += to_bcd_be( v, 2 );
	cmd += '\x01';
	cmd.append( to_bcd_be( freq, 10 ) );               // byte 8
	cmd += IC7100_mode_nbr[mode];                      // byte 13
	cmd += fil;                                        // byte 14
	if (mode >= 9)                                     // byte 15  data mode
		cmd += '\x01';
	else
		cmd += '\x00';
	cmd += '\x00';                                     // byte 16 duplex tone
	cmd += '\x00';                                     // byte 17 dig squelch
	cmd.append(to_bcd(PL_tones[tTONE], 6));            // byte 18 repeater tone
	cmd.append(to_bcd(PL_tones[rTONE], 6));            // byte 21 tone squelch freq
    cmd += '\x00'; cmd += '\x00'; cmd += '\x23';       // byte 24 dtcs tone
    cmd += '\x00';                                     // byte 27 digitital squelch code
    cmd += '\x00'; cmd += '\x60'; cmd += '\x00';       // byte 28 duplex offset frequency
    cmd.append("CKCKCK  ");                            // byte 31 destination call
    cmd.append("                ");                    // byte 39 memory name, 16 chars
	cmd.append(post);
	set_trace(1, "set band selection");
	waitFB("set_band_selection");
	seth();
}

void RIG_IC7100::setVfoAdj(double v)
{
	vfo_ = v;
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x47';
	cmd.append(bcd255(int(v)));
	cmd.append(post);
	set_trace(1, "set vfo adjust");
	waitFB("SET vfo adjust");
	seth();
}

double RIG_IC7100::getVfoAdj()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x47';
	cmd.append(post);

	get_trace(1, "getVfoAdj");
	ret = waitFOR(11, "get vfo adj");
	geth();

	if (ret) {
		size_t p = replystr.find(pre_fm);
		if (p != std::string::npos) {
			vfo_ = num100(replystr.substr(p+8));
		}
	}
	return vfo_;
}

int RIG_IC7100::get_FILT(int mode)
{
	if (inuse == onB) return mode_filterB[mode];
	return mode_filterA[mode];
}

void RIG_IC7100::set_FILT(int filter)
{
	if (filter < 1 || filter > 3)
		return;

	if (inuse == onB) {
		B.filter = filter;
		mode_filterB[B.imode] = filter;

		cmd = pre_to;
		cmd += '\x06';
		cmd += mdval[B.imode];
		if (B.imode < LSBD7100)
			cmd += filter;
		cmd.append( post );
		set_trace(1, "set mode/filter B");
		waitFB("set mode/filter B");
		seth();

		if (B.imode >= LSBD7100) {
			cmd = pre_to;
			cmd += '\x1A'; cmd += '\x06';
			cmd += '\x01';
			cmd += filter;
			cmd.append( post);
			set_trace(1, "set mode B");
			waitFB("set data mode B");
			seth();
		}
	} else {
		A.filter = filter;
		mode_filterA[A.imode] = filter;

		cmd = pre_to;
		cmd += '\x06';
		cmd += mdval[A.imode];
		if (A.imode < LSBD7100)
			cmd += filter;
		cmd.append( post );
		set_trace(1, "set filter A");
		waitFB("set filter A ");
		seth();

		if (A.imode >= LSBD7100) {
			cmd = pre_to;
			cmd += '\x1A'; cmd += '\x06';
			cmd += '\x01';
			cmd += filter;
			cmd.append( post);
			set_trace(1, "set data mode A");
			waitFB("set data mode A");
			seth();
		}
	}
}

const char *RIG_IC7100::FILT(int val)
{
	if (val < 1) val = 1;
	if (val > 3) val = 3;
	return(szfilter[val - 1]);
}

const char *RIG_IC7100::nextFILT()
{
	int val = A.filter;
	if (inuse == onB) val = B.filter;
	val++;
	if (val > 3) val = 1;
	set_FILT(val);
	return szfilter[val - 1];
}

void RIG_IC7100::set_FILTERS(std::string s)
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

std::string RIG_IC7100::get_FILTERS()
{
	for (int i = 0; i < NUM_MODES; i++) {
		if (mode_filterA[i] < 1) mode_filterA[i] = 1;
		if (mode_filterA[i] > 3) mode_filterA[i] = 3;
		if (mode_filterB[i] < 1) mode_filterB[i] = 1;
		if (mode_filterB[i] > 3) mode_filterB[i] = 3;
	}
	std::stringstream s;
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_filterA[i] << " ";
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_filterB[i] << " ";
	return s.str();
}

// ---------------------------------------------------------------------
// set date and time
// 1A 05 00 95 HH MM : set time
// 1A 05 00 94 YY YY MM DD : set date
// ---------------------------------------------------------------------
// dt formated as YYYYMMDD
// ---------------------------------------------------------------------
void RIG_IC7100::sync_date(char *dt)
{
	cmd.clear();
	cmd.append(pre_to);
	cmd += '\x1A'; cmd += '\x05';
	cmd += '\x01'; cmd += '\x20';
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
void RIG_IC7100::sync_clock(char *tm)
{
	cmd.clear();
	cmd.append(pre_to);
	cmd += '\x1A'; cmd += '\x05';
	cmd += '\x01'; cmd += '\x21';
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

