// ----------------------------------------------------------------------------
// Copyright (C) 2018
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

#include "icom/IC7851.h"

#include "support.h"

bool IC7851_DEBUG = true;

//=============================================================================
// IC-_7851

const char IC7851name_[] = "IC-7851";

#define NUM_FILTERS 3
#define NUM_MODES  16

enum {
	LSB_7851, USB_7851, AM_7851, CW_7851, RTTY_7851,
	FM_7851,  CWR_7851, RTTYR_7851, PSK_7851, PSKR_7851,
	LSBD1_7851, LSBD2_7851, LSBD3_7851, 
	USBD1_7851, USBD2_7851, USBD3_7851 };

static int mode_filterA[NUM_MODES] = {
	1,1,1,1,1,
	1,1,1,1,1,
	1,1,1,
	1,1,1};

static int mode_filterB[NUM_MODES] = {
	1,1,1,1,1,
	1,1,1,1,1,
	1,1,1,
	1,1,1};

const char *IC7851modes_[] = {
	"LSB", "USB", "AM", "CW", "RTTY",
	"FM", "CW-R", "RTTY-R", "PSK", "PSK-R", 
	"LSB-D1", "LSB-D2", "LSB-D3",
	"USB-D1", "USB-D2", "USB-D3", NULL};

const char IC7851_mode_type[] = {
	'L', 'U', 'U', 'U', 'L',
	'U', 'L', 'U', 'U', 'L', 
	'L', 'L', 'L',
	'U', 'U', 'U' };

const char IC7851_mode_nbr[] = {
	0x00, 0x01, 0x02, 0x03, 0x04,
	0x05, 0x07, 0x08, 0x12, 0x13, 
	0x00, 0x00, 0x00,
	0x01, 0x01, 0x01 };

const char *IC7851_ssb_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000", "3100", "3200", "3300", "3400", "3500",
"3600", NULL };

static int IC7851_bw_vals_SSB[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,
40, WVALS_LIMIT};

const char *IC7851_am_bws[] = {
"200",   "400",  "600",  "800", "1000", "1200", "1400", "1600", "1800", "2000",
"2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000",
"4200", "4400", "4600", "4800", "5000", "5200", "5400", "5600", "5800", "6000",
"6200", "6400", "6600", "6800", "7000", "_7851", "7400", "_7851", "_7851", "8000",
"8200", "8400", "8600", "8800", "9000", "9200", "9400", "9600", "9800", "10000", NULL };

static int IC7851_bw_vals_AM[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,
40,41,42,43,44,45,46,47,48,49,
WVALS_LIMIT};

const char *IC7851_fm_bws[] = { "FIXED", NULL };
static int IC7851_bw_vals_FM[] = { 1, WVALS_LIMIT};

static GUI IC7851_widgets[]= {
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

void RIG_IC7851::initialize()
{
	IC7851_widgets[0].W = btnVol;
	IC7851_widgets[1].W = sldrVOLUME;
	IC7851_widgets[2].W = btnAGC;
	IC7851_widgets[3].W = sldrRFGAIN;
	IC7851_widgets[4].W = sldrSQUELCH;
	IC7851_widgets[5].W = btnNR;
	IC7851_widgets[6].W = sldrNR;
	IC7851_widgets[7].W = btnLOCK;
	IC7851_widgets[8].W = sldrINNER;
	IC7851_widgets[9].W = btnCLRPBT;
	IC7851_widgets[10].W = sldrOUTER;
	IC7851_widgets[11].W = btnNotch;
	IC7851_widgets[12].W = sldrNOTCH;
	IC7851_widgets[13].W = sldrMICGAIN;
	IC7851_widgets[14].W = sldrPOWER;

	btn_icom_select_11->deactivate();
	btn_icom_select_12->deactivate();
	btn_icom_select_13->deactivate();

	choice_rTONE->activate();
	choice_tTONE->activate();
}

RIG_IC7851::RIG_IC7851() {
	defaultCIV = 0x8E;
	name_ = IC7851name_;
	modes_ = IC7851modes_;
	bandwidths_ = IC7851_ssb_bws;
	bw_vals_ = IC7851_bw_vals_SSB;

	_mode_type = IC7851_mode_type;
	adjustCIV(defaultCIV);

	serial_baudrate = BR19200;
	stopbits = 1;
	serial_retries = 2;
//	serial_write_delay = 0;
//	serial_post_write_delay = 0;
	serial_timeout = 50;
	serial_echo = true;
	serial_rtscts = false;
	serial_rtsplus = true;
	serial_dtrplus = true;
	serial_catptt = true;
	serial_rtsptt = false;
	serial_dtrptt = false;

	widgets = IC7851_widgets;

	has_extras =

	has_cw_wpm =
	has_cw_spot_tone =
	has_cw_qsk =

	has_vox_onoff =
	has_vox_gain =
	has_vox_anti =
	has_vox_hang =

	has_compON =
	has_compression =

	has_voltmeter =
	has_idd_control =

	has_micgain_control =
	has_bandwidth_control =
	has_smeter =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_notch_control =
	has_attenuator_control =
	has_preamp_control =
	has_ifshift_control =
	has_pbt_controls =
	has_ptt_control =
	has_tune_control =
	has_noise_control =
	has_noise_reduction =
	has_noise_reduction_control =
	has_rf_control =
	has_sql_control =
	has_split_AB =
	has_split = true;

	ICOMmainsub = true;

	has_band_selection = true;

	has_xcvr_auto_on_off = true;

	can_synch_clock = true;

	can_change_alt_vfo = true;

	has_a2b = true;

	precision = 1;
	ndigits = 8;


};

//======================================================================
// IC7851 unique commands
//======================================================================

void RIG_IC7851::selectA()
{
	cmd = pre_to;
	cmd += '\x07'; cmd += '\xD2'; cmd += '\x00';
	cmd.append(post);
	sendICcommand(cmd, 6);
	waitFB("Select A");
	inuse = onA;
}

void RIG_IC7851::selectB()
{
	cmd = pre_to;
	cmd += '\x07'; cmd += '\xD2'; cmd += '\x01';
	cmd.append(post);
	sendICcommand(cmd, 6);
	waitFB("Select B");
	inuse = onB;
}

void RIG_IC7851::set_xcvr_auto_on()
{
	cmd = pre_to;
	cmd += '\x19'; cmd += '\x00';
	cmd.append(post);
	if (waitFOR(8, "get ID", 100) == false) {
		cmd.clear();
		int fes[] = { 2, 2, 2, 3, 7, 13, 25, 50, 75, 150, 150, 150 };
		if (progStatus.serial_baudrate >= 0 && progStatus.serial_baudrate <= 11) {
			cmd.append( fes[progStatus.serial_baudrate], '\xFE');
		}
		cmd.append(pre_to);
		cmd += '\x18'; cmd += '\x01';
		cmd.append(post);
		waitFB("Power ON", 200);
		for (int i = 0; i < 5000; i += 100) {
			MilliSleep(100);
			update_progress(100 * i / 5000);
			Fl::awake();
		}
	}
}

void RIG_IC7851::set_xcvr_auto_off()
{
	cmd.clear();
	cmd.append(pre_to);
	cmd += '\x18'; cmd += '\x00';
	cmd.append(post);
	waitFB("Power OFF", 200);
}

bool RIG_IC7851::check ()
{
	std::string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	bool ok = waitFOR(11, "check vfo");
	rig_trace(2, "check()", str2hex(replystr.c_str(), replystr.length()));
	return ok;
}

static int ret = 0;

unsigned long long RIG_IC7851::get_vfoA ()
{
	std::string resp;

	cmd.assign(pre_to).append("\x25");
	resp.assign(pre_fm).append("\x25");

	if (inuse == onA) {
		cmd  += '\x00';
		resp += '\x00';
	} else {
		cmd  += '\x01';
		resp += '\x01';
	}

	cmd.append(post);

	get_trace(1, "get_vfoA()");
	ret = waitFOR(12, "get vfo A");
	geth();

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

void RIG_IC7851::set_vfoA (unsigned long long freq)
{
	A.freq = freq;

	cmd.assign(pre_to).append("\x25");
	if (inuse == onA) cmd += '\x00';
	else      cmd += '\x01';

	cmd.append( to_bcd_be( freq, 10) );
	cmd.append( post );

	set_trace(1, "set_vfoA");
	waitFB("set vfo A");
	seth();
}

unsigned long long RIG_IC7851::get_vfoB ()
{
	std::string resp;

	cmd.assign(pre_to).append("\x25");
	resp.assign(pre_fm).append("\x25");

	if (inuse == onA) {
		cmd  += '\x01';
		resp += '\x01';
	} else {
		cmd  += '\x00';
		resp += '\x00';
	}

	cmd.append(post);

	get_trace(1, "get_vfoB()");
	ret = waitFOR(12, "get vfo B");
	geth();

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

void RIG_IC7851::set_vfoB (unsigned long long freq)
{
	B.freq = freq;

	cmd.assign(pre_to).append("\x25");
	if (inuse == onA) cmd += '\x01';
	else      cmd += '\x00';

	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );

	set_trace(1, "set_vfoB");
	waitFB("set vfo B");
	seth();
}

bool RIG_IC7851::can_split()
{
	return true;
}

void RIG_IC7851::set_split(bool val)
{
	split = val;
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB(val ? "set split ON" : "set split OFF");
}

int RIG_IC7851::get_split()
{
	int read_split = 0;
	cmd.assign(pre_to);
	cmd.append("\x0F");
	cmd.append( post );
	if (waitFOR(7, "get split")) {
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

void RIG_IC7851::set_modeA(int val)
{
	A.imode = val;

	cmd.assign(pre_to);
	cmd += '\x26';

	if (inuse == onA) {
		cmd  += '\x00';
	} else {
		cmd  += '\x01';
	}

	cmd += IC7851_mode_nbr[A.imode];	// operating mode
	if (A.imode >= LSBD3_7851)
		cmd += '\x03';					// data mode D1
	else if (A.imode >= LSBD2_7851)
		cmd += '\x02';					// data mode D2
	else if (A.imode >= LSBD3_7851)
		cmd += '\x01';					// data mode D1
	else
		cmd += '\x00';
	cmd += mode_filterA[A.imode];		// filter
	cmd.append( post );
	waitFB("set mode A");

	set_trace(4, 
		"set mode A[",
		IC7851modes_[A.imode], 
		"] ", 
		str2hex(cmd.c_str(), cmd.length()));
}

//  0  1  2  3  4  5  6  7  8  9
// FE FE E0 94 26 NN NN NN NN FD
//                |  |  |  |
//                |  |  |  |__filter setting, 01, 02, 03
//                |  |  |_____data mode, 00 - off, 01 - data mode 1
//                |  |  |_____02 - data mode 2, 03 - data mode 3
//                |  |________Mode 00 - LSB
//                |                01 - USB
//                |                02 - AM
//                |                03 - CW
//                |                04 - RTTY
//                |                05 - FM
//                |                07 - CW-R
//                |                08 - RTTY-R
//                |___________selected vfo, 00 - active, 01 - inactive

int RIG_IC7851::get_modeA()
{
	int md = 0;
	size_t p;

	std::string resp;
	resp.assign(pre_fm).append("\x26");

	cmd.assign(pre_to).append("\x26");
	if (inuse == onA) {
		cmd  += '\x00';
		resp += '\x00';
	} else {
		cmd  += '\x01';
		resp += '\x01';
	}
	cmd.append(post);

	if (waitFOR(10, "get mode A")) {
		p = replystr.rfind(resp);
		if (p == std::string::npos)
			goto end_wait_modeA;

		if (replystr[p+6] == -1) { A.imode = A.filter = 0; return A.imode; }

		for (md = 0; md < LSBD1_7851; md++) {
			if (replystr[p+6] == IC7851_mode_nbr[md]) {
				A.imode = md;
				if (replystr[p+7] == 0x01 && A.imode < 4)
					A.imode += 10;
				if (replystr[p+7] == 0x02 && A.imode < 4)
					A.imode += 14;
				if (replystr[p+7] == 0x03 && A.imode < 4)
					A.imode += 18;
				if (A.imode > 21)
					A.imode = 1;
				break;
			}
			A.filter = replystr[p+8];
		}
	}

end_wait_modeA:
	get_trace(4, 
		"get mode A[",
		IC7851modes_[A.imode], 
		"] ", 
		str2hex(replystr.c_str(), replystr.length()));

	mode_filterA[A.imode] = A.filter;

	return A.imode;
}

void RIG_IC7851::set_modeB(int val)
{
	B.imode = val;

	cmd.assign(pre_to);
	cmd += '\x26';
	if (inuse == onA) {
		cmd  += '\x01';
	} else {
		cmd  += '\x00';
	}

	cmd += IC7851_mode_nbr[B.imode];	// operating mode
	if (B.imode >= LSBD3_7851)
		cmd += '\x03';					// data mode D1
	else if (B.imode >= LSBD2_7851)
		cmd += '\x02';					// data mode D2
	else if (B.imode >= LSBD3_7851)
		cmd += '\x01';					// data mode D1
	else
		cmd += '\x00';
	cmd += mode_filterB[B.imode];		// filter
	cmd.append( post );
	waitFB("set mode B");

	set_trace(4, 
		"set mode B[",
		IC7851modes_[B.imode], 
		"] ", 
		str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC7851::get_modeB()
{
	int md = 0;
	size_t p;

	std::string resp;
	resp.assign(pre_fm).append("\x26");

	cmd.assign(pre_to).append("\x26");

	if (inuse == onA) {
		cmd  += '\x01';
		resp += '\x01';
	} else {
		cmd  += '\x00';
		resp += '\x00';
	}

	cmd.append(post);

	if (waitFOR(10, "get mode B")) {
		p = replystr.rfind(resp);
		if (p == std::string::npos)
			goto end_wait_modeB;

		if (replystr[p+6] == -1) { B.imode = B.filter = 0; return B.imode; }

		for (md = 0; md < LSBD1_7851; md++) {
			if (replystr[p+6] == IC7851_mode_nbr[md]) {
				B.imode = md;
				if (replystr[p+7] == 0x01 && B.imode < 4)
					B.imode += 10;
				if (replystr[p+7] == 0x02 && B.imode < 4)
					B.imode += 14;
				if (replystr[p+7] == 0x03 && B.imode < 4)
					B.imode += 18;
				if (B.imode > 21)
					B.imode = 1;
				break;
			}
			B.filter = replystr[p+8];
		}
	}

end_wait_modeB:
	get_trace(4, 
		"get mode B[",
		IC7851modes_[B.imode], 
		"] ", 
		str2hex(replystr.c_str(), replystr.length()));

	mode_filterB[B.imode] = B.filter;

	return B.imode;
}

int RIG_IC7851::get_bwA()
{
	if (A.imode == 5) return 0;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	if (waitFOR(8, "get bwA")) {
		A.iBW = fm_bcd(replystr.substr(6), 2);
	}
	return A.iBW;
}

void RIG_IC7851::set_bwA(int val)
{
	A.iBW = val;
	if (A.imode == 5) return;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(A.iBW, 2));
	cmd.append(post);
	if (IC7851_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	waitFB("set bwA");
}

int RIG_IC7851::get_bwB()
{
	if (B.imode == 5) return 0;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	if (waitFOR(8, "get bwB")) {
		B.iBW = fm_bcd(replystr.substr(6), 2);
	}
	return B.iBW;
}

void RIG_IC7851::set_bwB(int val)
{
	B.iBW = val;
	if (B.imode == 5) return;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(A.iBW, 2));
	cmd.append(post);
	if (IC7851_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	waitFB("set bwB");
}

int RIG_IC7851::adjust_bandwidth(int m)
{
	int bw = 0;
	switch (m) {
		case 2: // AM
			bandwidths_ = IC7851_am_bws;
			bw_vals_ = IC7851_bw_vals_AM;
			bw = 19;
			break;
		case 5: // FM
			bandwidths_ = IC7851_fm_bws;
			bw_vals_ = IC7851_bw_vals_FM;
			bw = 0;
			break;
		case 3: case 7: // CW
			bandwidths_ = IC7851_ssb_bws;
			bw_vals_ = IC7851_bw_vals_SSB;
			bw = 12;
			break;
		case 4: case 8: // RTTY
			bandwidths_ = IC7851_ssb_bws;
			bw_vals_ = IC7851_bw_vals_SSB;
			bw = 12;
			break;
		case 0: case 1: // SSB
		case 12: case 13: // PKT
		default:
			bandwidths_ = IC7851_ssb_bws;
			bw_vals_ = IC7851_bw_vals_SSB;
			bw = 34;
			break;
	}
	return bw;
}

const char ** RIG_IC7851::bwtable(int m)
{
	const char ** table;
	switch (m) {
		case 2: // AM
			table = IC7851_am_bws;
			break;
		case 5: // FM
			table = IC7851_fm_bws;
			break;
		case 3: case 7: // CW
		case 4: case 8: // RTTY
		case 0: case 1: // SSB
		case 12: case 13: // PKT
		default:
			table = IC7851_ssb_bws;
			break;
	}
	return table;
}

int RIG_IC7851::def_bandwidth(int m)
{
	int bw = 0;
	switch (m) {
		case 2: // AM
			bw = 19;
			break;
		case 5: // FM
			bw = 0;
			break;
		case 4: case 8: // RTTY
			bw = 12;
			break;
		case 3: case 7: // CW
			bw = 12;
			break;
		case 0: case 1: // SSB
		case 12: case 13: // PKT
		default:
			bw = 34;
	}
	return bw;
}

void RIG_IC7851::set_mic_gain(int v)
{
	if (progStatus.USBaudio) return;
	int ICvol = (int)(v * 255 / 100);

	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(to_bcd(ICvol, 3));
	cmd.append( post );
	waitFB("set mic gain");
}

static const char *atten_labels[] = {
"ATT", "3 dB", "6 dB", "9 dB", "12 dB", "15 dB", "18 dB", "21 dB"};

int  RIG_IC7851::next_attenuator()
{
	if (atten_level >= 7) return 0;
	else return (atten_level + 1);
}

void RIG_IC7851::set_attenuator(int val)
{
	atten_level = val;

	int cmdval = atten_level;
	atten_label(atten_labels[atten_level], true);
	cmd = pre_to;
	cmd += '\x11';
	cmd += cmdval;
	cmd.append( post );
	waitFB("set_attenuator");
}

int RIG_IC7851::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	if (waitFOR(7, "get attenuator")) {
		if (replystr[4] == 0x06) {
			atten_level = replystr[5];
			if (atten_level >= 0 && atten_level <= 7)
				atten_label(atten_labels[atten_level], true);
		}
	}
	return atten_level;
}

void RIG_IC7851::set_compression(int on, int val)
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

void RIG_IC7851::set_vox_onoff()
{
	if (progStatus.vox_onoff) {
		cmd.assign(pre_to).append("\x16\x46\x01");
		cmd.append( post );
		waitFB("set vox ON");
	} else {
		cmd.assign(pre_to).append("\x16\x46");
		cmd += '\x00';		// ALH
		cmd.append( post );
		waitFB("set vox OFF");
	}
}

void RIG_IC7851::set_vox_gain()
{
	cmd.assign(pre_to).append("\x14\x16");
	cmd.append(to_bcd((int)(progStatus.vox_gain * 2.55), 3));
	cmd.append( post );
	waitFB("SET vox gain");
}

void RIG_IC7851::set_vox_anti()
{
	cmd.assign(pre_to).append("\x14\x17");
	cmd.append(to_bcd((int)(progStatus.vox_anti * 2.55), 3));
	cmd.append( post );
	waitFB("SET anti-vox");
}

void RIG_IC7851::set_vox_hang()
{
	cmd.assign(pre_to).append("\x1A\x05\0x01\0x83");
	cmd.append(to_bcd((int)(progStatus.vox_hang / 10 ), 2));
	cmd.append( post );
	waitFB("SET vox hang");
}

void RIG_IC7851::get_vox_hang_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 200; step = 10;
}

// CW controls

void RIG_IC7851::set_cw_wpm()
{
	cmd.assign(pre_to).append("\x14\x0C");
	cmd.append(to_bcd(round((progStatus.cw_wpm - 6) * 255 / (48 - 6)), 3));
	cmd.append( post );
	waitFB("SET cw wpm");
}

// not implemented -- work in progress
//int RIG_IC7851::get_cw_wpm()
//{
//	int wpm = progStatus.cw_wpm;
//	cmd = pre_to;
//	cmd.append("\x14\x0C");
//	cmd.append(post);
//	resp = pre_fm;
//	cmd.append("\x14\x0C");
//	if (waitFOR(9, "get WPM")) {
//		size_t p = replystr.rfind(resp);
//		if (p != std::string::npos) {
//			wpm = replystr[p + 6];
//			
//	}
//}

void RIG_IC7851::get_cw_wpm_min_max(int &min, int &max)
{
	min = 6; max = 48;
}

void RIG_IC7851::set_cw_qsk()
{
	int n = round(progStatus.cw_qsk * 10);
	cmd.assign(pre_to).append("\x14\x0F");
	cmd.append(to_bcd(n, 3));
	cmd.append(post);
	waitFB("Set cw qsk delay");
}

void RIG_IC7851::set_cw_spot_tone()
{
	cmd.assign(pre_to).append("\x14\x09"); // values 0=300Hz 255=900Hz
	int n = round((progStatus.cw_spot_tone - 300) * 255.0 / 600.0);
	if (n > 255) n = 255;
	if (n < 0) n = 0;
	cmd.append(to_bcd(n, 3));
	cmd.append( post );
	waitFB("SET cw spot tone");
}

void RIG_IC7851::set_cw_vol()
{
	cmd.assign(pre_to);
	cmd.append("\x14\x15");
	cmd.append(to_bcd((int)(progStatus.cw_vol * 2.55), 3));
	cmd.append( post );
	waitFB("SET cw sidetone volume");
}

void RIG_IC7851::get_cw_qsk_min_max_step(double &min, double &max, double &step)
{
	min = 2.0; max = 13.0; step = 0.1;
}

void RIG_IC7851::get_cw_spot_tone_min_max_step(int &min, int &max, int &step)
{
	min = 300; max = 900; step = 5;
}

// Tranceiver PTT on/off
void RIG_IC7851::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	waitFB("set ptt");
	ptt_ = val;
}

int RIG_IC7851::get_PTT()
{
	cmd = pre_to;
	cmd += '\x1c'; cmd += '\x00';
	std::string resp = pre_fm;
	resp += '\x1c'; resp += '\x00';
	cmd.append(post);
	if (waitFOR(8, "get PTT")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			ptt_ = replystr[p + 6];
	}
	return ptt_;
}

void RIG_IC7851::get_pc_min_max_step(double &min, double &max, double &step)
{
	min = 5; pmax = max = 200; step = 1;
}

// Volume control val 0 ... 100
void RIG_IC7851::set_volume_control(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set vol");
}

int RIG_IC7851::get_volume_control()
{
	int val = progStatus.volume;
	std::string cstr = "\x14\x01";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(9, "get vol")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p+6));
	}
	return val;
}

void RIG_IC7851::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

int RIG_IC7851::get_smeter()
{
	std::string cstr = "\x15\x02";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get smeter")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr /2.55);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

struct pwrpair {int mtr; float pwr;};

static pwrpair pwrtbl[] = { 
{0, 0.0},
{40, 20.0},
{76, 40.0},
{92, 50.0},
{103, 60.0},
{124, 80.0},
{143, 100.0},
{183, 150.0},
{213, 200.0},
{255, 300.0} };

int RIG_IC7851::get_power_out(void)
{
	std::string cstr = "\x15\x11";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= 0;
	if (waitFOR(9, "get power out")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			size_t i = 0;
			for (i = 0; i < sizeof(pwrtbl) / sizeof(pwrpair) - 1; i++)
				if (mtr >= pwrtbl[i].mtr && mtr < pwrtbl[i+1].mtr)
					break;
			if (mtr < 0) mtr = 0;
			if (mtr > 213) mtr = 213;
			mtr = (int)ceil(pwrtbl[i].pwr + 
				(pwrtbl[i+1].pwr - pwrtbl[i].pwr)*(mtr - pwrtbl[i].mtr)/(pwrtbl[i+1].mtr - pwrtbl[i].mtr));
			
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

int RIG_IC7851::get_swr(void) 
{
	std::string cstr = "\x15\x12";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get swr")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr /2.55);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

int RIG_IC7851::get_alc(void)
{
	std::string cstr = "\x15\x13";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get alc")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr /2.55);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

void RIG_IC7851::set_pbt_inner(int val)
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

void RIG_IC7851::set_pbt_outer(int val)
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

int RIG_IC7851::get_pbt_inner()
{
	int val = 0;
	std::string cstr = "\x14\x07";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(9, "get pbt inner")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			val = num100(replystr.substr(p+6));
			val -= 50;
		}
	}
	rig_trace(2, "get_pbt_inner()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

int RIG_IC7851::get_pbt_outer()
{
	int val = 0;
	std::string cstr = "\x14\x08";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(9, "get pbt outer")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			val = num100(replystr.substr(p+6));
			val -= 50;
		}
	}
	rig_trace(2, "get_pbt_outer()", str2hex(replystr.c_str(), replystr.length()));
	return val;
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

void RIG_IC7851::get_band_selection(int v)
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x01");
	cmd += to_bcd_be( v, 2 );
	cmd += '\x01';
	cmd.append( post );

	if (waitFOR(23, "get band stack")) {
		set_trace(2, "get band stack", str2hex(replystr.c_str(), replystr.length()));
		size_t p = replystr.rfind(pre_fm);
		if (p != std::string::npos) {
			unsigned long long bandfreq = fm_bcd_be(replystr.substr(p+8, 5), 10);
			int bandmode = replystr[p+13];
			int bandfilter = replystr[p+14];
			int banddata = replystr[p+15] & 0x10;
			int tone = fm_bcd(replystr.substr(p+16, 3), 6);
			size_t index = 0;
			for (index = 0; index < sizeof(PL_tones) / sizeof(*PL_tones); index++)
				if (tone == PL_tones[index]) break;
			tTONE = index;
			tone = fm_bcd(replystr.substr(p+19, 3), 6);
			for (index = 0; index < sizeof(PL_tones) / sizeof(*PL_tones); index++)
				if (tone == PL_tones[index]) break;
			rTONE = index;
			if ((bandmode == 0) && banddata) 
				bandmode = ((banddata == 0x10) ? 10 : 
							(banddata == 0x20) ? 11 :
							(banddata == 0x30) ? 12 : 0);
			if ((bandmode == 1) && banddata)
				bandmode = ((banddata == 0x10) ? 13 : 
							(banddata == 0x20) ? 14 :
							(banddata == 0x30) ? 15 : 1);
			if (inuse == onA) {
				set_vfoB(bandfreq);
				set_modeB(bandmode);
				set_FILT(bandfilter);
			} else {
				set_vfoA(bandfreq);
				set_modeA(bandmode);
				set_FILT(bandfilter);
			}
		}
	} else
		set_trace(2, "get band stack", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC7851::set_band_selection(int v)
{
	unsigned long long freq = (inuse == onB ? B.freq : A.freq);
	int mode = (inuse == onB ? B.imode : A.imode);

	cmd.assign(pre_to);
	cmd.append("\x1A\x01");
	cmd += to_bcd_be( v, 2 );
	cmd += '\x01';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd += mode;
	cmd += '\x01';
	cmd += '\x00';
	cmd.append(to_bcd(PL_tones[tTONE], 6));
	cmd.append(to_bcd(PL_tones[rTONE], 6));
	cmd.append(post);
	waitFB("set_band_selection");
	set_trace(2, "set_band_selection()", str2hex(replystr.c_str(), replystr.length()));

	cmd.assign(pre_to);
	cmd.append("\x1A\x01");
	cmd += to_bcd_be( v, 2 );
	cmd += '\x01';
	cmd.append( post );

	waitFOR(23, "get band stack");
}

void RIG_IC7851::set_notch(bool on, int freq)
{
	int hexval;
	switch (vfo->imode) {
		default:
		case USB_7851: case USBD1_7851: case USBD2_7851: case USBD3_7851: case RTTYR_7851:
			hexval = freq - 1500;
			break;
		case LSB_7851: case LSBD1_7851: case LSBD2_7851: case LSBD3_7851: case RTTY_7851:
			hexval = 1500 - freq;
			break;
		case CW_7851:
			hexval = progStatus.cw_spot_tone - freq;
			break;
		case CWR_7851:
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
	waitFB("set notch");

	cmd = pre_to;
	cmd.append("\x14\x0D");
	cmd.append(to_bcd(hexval,3));
	cmd.append(post);
	waitFB("set notch val");
}

bool RIG_IC7851::get_notch(int &val)
{
	bool on = false;
	val = 1500;

	std::string cstr = "\x16\x48";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get notch")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			on = replystr[p + 6];
		cmd = pre_to;
		resp = pre_fm;
		cstr = "\x14\x0D";
		cmd.append(cstr);
		resp.append(cstr);
		cmd.append(post);
		if (waitFOR(9, "notch val")) {
			size_t p = replystr.rfind(resp);
			if (p != std::string::npos) {
				val = (int)ceil(fm_bcd(replystr.substr(p+6),3));
				val -= 128;
				val *= 20;
				switch (vfo->imode) {
					default:
					case USB_7851: case USBD1_7851: case USBD2_7851: case USBD3_7851: case RTTYR_7851:
						val = 1500 + val;
						break;
					case LSB: case LSBD1_7851: case LSBD2_7851: case LSBD3_7851: case RTTY_7851:
						val = 1500 - val;
						break;
					case CW_7851:
						val = progStatus.cw_spot_tone - val;
						break;
					case CWR_7851:
						val = progStatus.cw_spot_tone + val;
						break;
				}
			}
		}
	}
	return on;
}

void RIG_IC7851::get_notch_min_max_step(int &min, int &max, int &step)
{
	switch (vfo->imode) {
		default:
		case USB_7851: case USBD1_7851: case USBD2_7851: case USBD3_7851: case RTTYR_7851:
		case LSB_7851: case LSBD1_7851: case LSBD2_7851: case LSBD3_7851: case RTTY_7851:
			min = 0; max = 3000; step = 20; break;
		case CW_7851: case CWR_7851:
			min = progStatus.cw_spot_tone - 500;
			max = progStatus.cw_spot_tone + 500;
			step = 20;
			break;
	}
}

// ---------------------------------------------------------------------
// set date and time
// 1A 05 00 95 HH MM : set time
// 1A 05 00 94 YY YY MM DD : set date
// ---------------------------------------------------------------------
// dt formated as YYYYMMDD
// ---------------------------------------------------------------------
void RIG_IC7851::sync_date(char *dt)
{
	cmd.clear();
	cmd.append(pre_to);
	cmd += '\x1A'; cmd += '\x05';
	cmd += '\x00'; cmd += '\x95';
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
void RIG_IC7851::sync_clock(char *tm)
{
	cmd.clear();
	cmd.append(pre_to);
	cmd += '\x1A'; cmd += '\x05';
	cmd += '\x00'; cmd += '\x96';
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

double RIG_IC7851::get_idd(void)
{
	get_trace(1, "get_idd()");
	std::string cstr = "\x15\x16";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	double mtr= 0;

	ret = waitFOR(9, "get idd");
	igett("");

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = 25.0 * mtr / 241.0;
			if (mtr > 25) mtr = 25;
		}
	}
	return mtr;
}

double RIG_IC7851::get_voltmeter()
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
			val = 52.0 * mtr + 211.0;
			if (val > 52) val = 52;
			return val;
		}
	}
	return -1;
}

#undef NUM_FILTERS
#undef NUM_MODES
