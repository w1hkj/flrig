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
#include <iostream>

#include "elecraft/KX3.h"
#include "status.h"

#include "support.h"

const char KX3name_[] = "KX3";

enum {KX3_LSB, KX3_USB, KX3_CW, KX3_FM, KX3_AM, KX3_DATA, KX3_CWR, KX3_DATAR};

const char *KX3modes_[] =
	{ "LSB", "USB", "CW", "FM", "AM", "DATA", "CW-R", "DATA-R", NULL};
const char modenbr[] =
	{ '1', '2', '3', '4', '5', '6', '7', '9' };
static const char KX3_mode_type[] =
	{ 'L', 'U', 'L', 'U', 'U', 'U', 'U', 'L' };

#define KX3_BWS 45

static const char *KX3_widths[] = {
   "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
  "550",  "600",  "650",  "700",  "750",  "800",  "850",  "900",  "950", "1000",
 "1100", "1200", "1300", "1400", "1500", "1600", "1700", "1800", "1900", "2000",
 "2100", "2200", "2300", "2400", "2500", "2600", "2700", "2800", "2900", "3000",
 "3200", "3400", "3600", "3800", "4000", NULL};

static int KX3_bw_vals[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40, 
41,42,43,44,45, WVALS_LIMIT};

static int def_mode_width[] = { 34, 34, 15, 37, 37, 34, 15, 34 };

static GUI k3_widgets[]= {
	{ (Fl_Widget *)btnVol, 2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME, 54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN, 54, 145, 156 },
	{ (Fl_Widget *)btn_KX3_IFsh, 214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 125, 156 },
	{ (Fl_Widget *)sldrPOWER, 266, 145, 156 },
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};

RIG_KX3::RIG_KX3() {
// base class values
	name_ = KX3name_;
	modes_ = KX3modes_;
	bandwidths_ = KX3_widths;
	bw_vals_ = KX3_bw_vals;
	comm_baudrate = BR38400;

	widgets = k3_widgets;

	stopbits = 1;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;

	def_freq = freqA = freqB = 14070000;
	def_mode = modeA = modeB = 1;
	def_bw = bwA = bwB = 34;

	can_change_alt_vfo =

	has_split_AB =
	has_micgain_control =
	has_rf_control =
	has_bandwidth_control =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_ptt_control =
	has_noise_control =
	has_noise_reduction =
	has_attenuator_control =
	has_smeter =
	has_power_out =
	has_split =
	has_ifshift_control =
	has_swr_control = 
	has_preamp_control = true;

	has_notch_control =
	has_tune_control =false;

	if_shift_min = 1280;
	if_shift_max = 1700;
	if_shift_step = 10;
	if_shift_mid = 1500;

	precision = 1;
	ndigits = 8;

	progStatus.rfgain = 250;
}

int  RIG_KX3::adjust_bandwidth(int m)
{
	return def_mode_width[m];
}

int  RIG_KX3::def_bandwidth(int m)
{
	return def_mode_width[m];
}

static int isok = false;

static void init_trace()
{
	debug::level = debug::DEBUG_LEVEL;

	progStatus.gettrace   = 
	progStatus.settrace   = 
	progStatus.serialtrace =
	progStatus.rigtrace    =
	progStatus.xmltrace    =
	progStatus.trace       = false;

	progStatus.gettrace   = true;
	progStatus.settrace   = true;
//	progStatus.serialtrace = true;
//	progStatus.rigtrace    = true;
//	progStatus.xmltrace    = true;
//	progStatus.trace       = true;


}

/*
 * OM (Option Module; GET)

OM APF---TBXI0n;
    |
0123456789012345
0.........1.....

A = ATU
P = external 100W PA
F = roofing filter
T = external 100W ATU
B = internal NiMH battery charger/real-time clock
X = KX3-2M or KX3-4M transverter module
I = KXIO2 RTC I/O module
0n = product identifer
     1 - KX2
     2 - KX3
*/
static struct KX3_OPTIONS {
	bool ATU;
	bool KXPA;
	bool ROOF;
	bool KXPA_ATU;
	bool NICAD;
	bool TXVRT;
	bool RTC;
	int  PROD;
} options = { 
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	0 };

void RIG_KX3::get_options()
{
	cmd = "OM;";
	get_trace(1, "get options");
	int ret = wait_char(';', 16, KX3_WAIT_TIME, "get options", ASC);
	gett("");
	if (ret < 16)
		return;

	options.ATU  = (replystr[3] == 'A');
	options.KXPA = (replystr[4] == 'P');
	options.ROOF = (replystr[5] == 'F');
	options.KXPA_ATU = (replystr[9] == 'T');
	options.NICAD = (replystr[10] == 'B');
	options.TXVRT = (replystr[11] == 'X');
	options.RTC = (replystr[12] == 'I');
	options.PROD = (replystr[14] - '0');
}

void RIG_KX3::initialize()
{
	init_trace();

	RigSerial->Timeout(50);

	LOG_INFO("KX3");
	k3_widgets[0].W = btnVol;
	k3_widgets[1].W = sldrVOLUME;
	k3_widgets[2].W = sldrRFGAIN;
	k3_widgets[3].W = btn_KX3_IFsh;
	k3_widgets[4].W = sldrIFSHIFT;
	k3_widgets[5].W = sldrMICGAIN;
	k3_widgets[6].W = sldrPOWER;

	check();

	if (!isok) return;

	cmd = "K31;"; // KX3 extended mode
	set_trace(1, "KX3 extended mode");
	sendCommand(cmd, 0, 50);
	sett("");
	showresp(INFO, ASC, "KX3 extended mode", cmd, replystr);

	cmd = "AI0;"; // disable auto-info
	set_trace(1, "disable auto info");
	sendCommand(cmd, 0, 50);
	sett("");
	showresp(INFO, ASC, "disable auto-info", cmd, replystr);

}

void RIG_KX3::shutdown()
{
}

// from programming notes:
// vfo change to also incurs a band change will disable CAT responses for up
// to 500 msec (UGH)
// use check after all vfo changes

bool RIG_KX3::check ()
{
	static char sztemp[50];

	if (isok) return true;

// try reading vfoA
	int ret = 0, wait = 10;

	while ( ( ret < 14) && (wait > 0) ) {
		snprintf(sztemp, sizeof(sztemp), "check() %d", 11 - wait);
		get_trace(1, sztemp);
		cmd = "FA;";
		ret = wait_char(';', 14, 600, sztemp, ASC);
		gett("");
		wait--;
	}

	isok = true;

	if (ret < 14) isok = false;

	return isok;

}

unsigned long int RIG_KX3::get_vfoA ()
{
	cmd = "FA;";

	get_trace(1, "get vfoA");
	int ret = wait_char(';', 14, KX3_WAIT_TIME, "get vfo A", ASC);
	gett("");

	if (ret < 14) return freqA;

	size_t p = replystr.rfind("FA");
	if (p == std::string::npos) return freqA;

	unsigned long int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqA = f;

	return freqA;
}

void RIG_KX3::set_vfoA (unsigned long int freq)
{
	freqA = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	set_trace(1, "set vfoA");
	sendCommand(cmd, 0, 50);
	sett("");
	showresp(INFO, ASC, "set vfo A", cmd, replystr);
	check();
}

unsigned long int RIG_KX3::get_vfoB ()
{
	cmd = "FB;";
	get_trace(1, "get vfoB");
	int ret = wait_char(';', 14, KX3_WAIT_TIME, "get vfo B", ASC);
	gett("");

	if (ret < 14) return freqB;
	size_t p = replystr.rfind("FB");
	if (p == std::string::npos) return freqB;

	unsigned long int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqB = f;
	return freqB;
}

void RIG_KX3::set_vfoB (unsigned long int freq)
{
	freqB = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}

	set_trace(1, "set vfoB");
	sendCommand(cmd, 0, 50);
	sett("");
	showresp(INFO, ASC, "set vfo B", cmd, replystr);

	check();
}

// Volume control
void RIG_KX3::set_volume_control(int val)
{
	cmd = "AG000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += val % 10;
		val /= 10;
	}

	set_trace(1, "set volume control");
	sendCommand(cmd, 0, 50);
	showresp(INFO, ASC, "set vol", cmd, replystr);
	sett("");

}

int RIG_KX3::get_volume_control()
{
	cmd = "AG;";

	get_trace(1, "get volume control");
	int ret = wait_char(';', 6, KX3_WAIT_TIME, "get volume", ASC);
	gett("");

	if (ret < 6) return progStatus.volume;
	size_t p = replystr.rfind("AG");
	if (p == std::string::npos) return 0;

	replystr[p + 5] = 0;
	int v = atoi(&replystr[p + 2]);
	return v;
}

/*
 * MD $ (Operating Mode; GET/SET)
 * SET/RSP format: MDn; or MD$n;
 *   where n is
 *     1 (LSB),
 *     2 (USB),
 *     3 (CW),
 *     4 (FM),
 *     5 (AM),
 *     6 (DATA),
 *     7 (CW-REV),
 *     9 (DATA-REV).
 * Notes:
 *   (1) K3 only: In Diversity Mode (accessed by holding SUB), sending MDn;
 *       sets both main and sub mode to n.
 *   (2) DATA and DATA-REV select the data sub-mode that was last in effect
 *       on the present band. (To read/set data sub-mode, use DT.)
 *       The norm/rev conditions for the K3’s data sub-modes are
 *       handled in two pairs at present:
 *         DATA A/PSK D, and
 *         AFSK A/FSK D.
 *       E.g., if the radio is set up for DATA A mode, alternating between
 *             MD6 and MD9 will cause both DATA A and PSK D to be set to the
 *             same normal/reverse condition.
 *       In K2 command modes 1 and 3 (K21 and K23), the RSP message converts
 *       modes 6 and 7 (DATA and DATA-REV) to modes 1 and 2 (LSB and USB).
 *       This may be useful with existing software applications that don't
 * handle DATA modes correctly.
*/
/* The DT command needs to be included for get/set mode
 *
 * DT (DATA Sub-Mode; GET/SET)
 *   SET/RSP format: DTn; where n is the data sub-mode last used with VFO A,
 *   whether or not DATA mode is in effect:
 *     0 (DATA A),
 *     1 (AFSK A),
 *     2 (FSK D),
 *     3 (PSK D). See MD for data normal/reverse considerations.
 *   In Diversity Mode (K3 only, accessed by sending DV1 or via a unsigned long int hold
 *   of SUB), sending DTn matches the sub receiver’s mode to the main receiver’s.
 *   Notes:
 *     (1) Use DT only when the transceiver is in DATA mode; otherwise,
 *         the returned value may not be valid.
 *     (2) In AI2/3 modes, changing the data sub-mode results in both FW
 *         and IS responses.
 *     (3) The present data sub-mode is also reported as part of the IF command,
 *         although this requires that K31 be in effect. Refer to the IF command
 *         for details.
*/
/* and last but not least, the IF command
 *
 * IF (Transceiver Information; GET only)
 *   RSP format: IF[f]*****+yyyyrx*00tmvspbd1*;
 *   where the fields are defined as follows:
 *     [f]    Operating frequency, excluding any RIT/XIT offset
 *            (11 digits; see FA command format)
 *      *     represents a space (BLANK, or ASCII 0x20)
 *      +     either "+" or "-" (sign of RIT/XIT offset)
 *      yyyy  RIT/XIT offset in Hz (range is -9999 to +9999 Hz when
 *            computer-controlled)
 *      r     1 if RIT is on, 0 if off
 *      x     1 if XIT is on, 0 if off
 *      t     1 if the K3 is in transmit mode, 0 if receive
 *      m     operating mode (see MD command)
 *      v     receive-mode VFO selection, 0 for VFO A, 1 for VFO B
 *      s     1 if scan is in progress, 0 otherwise
 *      p     1 if the transceiver is in split mode, 0 otherwise
 *      b     Basic RSP format: always 0;
 *            K2 Extended RSP format (K22):
 *            1 if present IF response is due to a band change; 0 otherwise
 *      d     Basic RSP format: always 0;
 *            K3 Extended RSP format (K31):
 *      1     DATA sub-mode, if applicable
 *              (0=DATA A, 1=AFSK A, 2= FSK D, 3=PSK D)
 * The fixed-value fields (space, 0, and 1) are provided for syntactic
 * compatibility with existing software.
 *
 * 01234567890123456789012345678901234567
 * 0         1         2         3      7
 * IF00014070000*****+yyyyrx*00tmvspbd1*;
 *   |---------|                   |
 *     vfo a/b                     split on = '1', off = '0'
 * IF00014070000     -000000 0002000011 ;  OFF
 * IF00014070000     -000000 0002001011 ;  ON
*/

void RIG_KX3::set_modeA(int val)
{
	modeA = val;
	cmd = "MD0;";
	cmd[2] = modenbr[val];

	set_trace(1, "set modeA");
	sendCommand(cmd, 0, 50);
	sett("");
	showresp(INFO, ASC, "set mode A", cmd, replystr);
}

int RIG_KX3::get_modeA()
{
	cmd = "MD;";

	get_trace(1, "get modeA");
	int ret = wait_char(';', 4, KX3_WAIT_TIME, "get mode A", ASC);
	gett("");

	if (ret < 4) return modeA;
	size_t p = replystr.rfind("MD");
	if (p == std::string::npos) return modeA;
	int md = replystr[p + 2] - '1';
	if (md == 8) md--;
	return (modeA = md);
}

void RIG_KX3::set_modeB(int val)
{
	modeB = val;
	cmd = "MD$0;";
	cmd[3] = modenbr[val];

	set_trace(1, "set modeB");
	sendCommand(cmd, 0, 50);
	sett("");

	showresp(INFO, ASC, "set mode B", cmd, replystr);
}

int RIG_KX3::get_modeB()
{
	cmd = "MD$;";

	get_trace(1, "get modeB");
	int ret = wait_char(';', 4, KX3_WAIT_TIME, "get mode B", ASC);
	gett("");

	if (ret < 4) return modeB;
	size_t p = replystr.rfind("MD$");
	if (p == std::string::npos) return modeB;
	int md = replystr[p + 3] - '1';
	if (md == 8) md--;
	return (modeB = md);
}

int RIG_KX3::get_modetype(int n)
{
	return KX3_mode_type[n];
}

void RIG_KX3::set_preamp(int val)
{
	set_trace(1, "set preamp");
	if (val) cmd = "PA1;";
	else     cmd = "PA0;";
	sendCommand(cmd, 0, 50);
	sett("");
}

int RIG_KX3::get_preamp()
{
	cmd = "PA;";

	get_trace(1, "get preamp");
	int ret = wait_char(';', 4, KX3_WAIT_TIME, "get preamp", ASC);
	gett("");

	if (ret < 4) return progStatus.preamp;
	size_t p = replystr.rfind("PA");
	if (p == std::string::npos) return 0;
	return (replystr[p + 2] == '1' ? 1 : 0);
}

int RIG_KX3::next_preamp()
{
	if (progStatus.preamp) return 0;
	return 1;
}

//
void RIG_KX3::set_attenuator(int val)
{
	set_trace(1, "set attenuator");
	if (val) cmd = "RA01;";
	else     cmd = "RA00;";
	sendCommand(cmd, 0, 50);
	sett("");
}

int RIG_KX3::get_attenuator()
{
	cmd = "RA;";

	get_trace(1, "get attenuator");
	int ret = wait_char(';', 5, KX3_WAIT_TIME, "get ATT", ASC);
	gett("");

	if (ret < 5) return progStatus.attenuator;
	size_t p = replystr.rfind("RA");
	if (p == std::string::npos) return 0;
	return (replystr[p + 3] == '1' ? 1 : 0);
}

int RIG_KX3::next_attenuator()
{
	if (progStatus.attenuator) return 0;
	return 1;
}

/*
 * PC (Requested Power Output Level; GET/SET)
 *
 * Basic SET/RSP format: PCnnn; For the K3, nnn is 000-012 or 000-110 watts depending on the power range.
 *
 * If CONFIG:KXV3 is set to TEST or if a transverter band with low-level I/O is selected, then the unit
 * is hundreds of a milliwatt, and the available range is 0.00-1.50 mW.
 *
 * This can be checked using the IC command, byte a, bit 4.
 *
 */

void RIG_KX3::get_pc_min_max_step(double &min, double &max, double &step)
{
	get_options();
	if (options.KXPA) {
		min = 0; max = 110; step = 1;
	} else {
		min = 0; max = 15; step = 1;
	}
}

void RIG_KX3::set_power_control(double val)
{
	int ival = val;
	cmd = "PC000;";
	cmd[4] += ival % 10;
	ival /= 10;
	cmd[3] += ival % 10;
	ival /= 10;
	cmd[2] += ival % 10;
	set_trace(1, "set power control");
	sendCommand(cmd, 0, 50);
	showresp(INFO, ASC, "set power ctrl", cmd, replystr);
	sett("");
}

double RIG_KX3::get_power_control()
{
	cmd = "PC;";

	get_trace(1, "get power control");
	int ret = wait_char(';', 6, KX3_WAIT_TIME, "get power level", ASC);
	gett("");

	if (ret < 6) return progStatus.power_level;

	size_t p = replystr.rfind("PC");
	if (p == std::string::npos)
		return progStatus.power_level;

	replystr[p+5] = 0;
	return atoi(&replystr[p+2]);
}

/*
 * PO ** (Actual Power Output Level; GET only; KX3 only)
 *
 * RSP format: PCnnn;
 *   where nnn is the power in tenths of a watt (QRP mode) or watts (QRO mode).
 *
 * Note: The QRO case only applies if the KXPA100 amplifier enabled via PA MODE=ON,
 * is connected to the KX3 via the special control cable, and the PWR level is set
 * to 11 W or higher. The reading is approximate,  * as it is estimated from the
 * KX3’s drive level. For a more accurate reading, use the KXPA100’s “^PF;” command.
*/

int RIG_KX3::power_scale()
{
	return powerScale;
}

int RIG_KX3::get_power_out()
{
	int ret = 0;
	size_t p = 0;
	int mtr = 0;

	if (options.KXPA) {
		cmd = "^OP;";
		get_trace(1, "test KXPA");
		ret = wait_char(';', 5, KX3_WAIT_TIME, "test KXPA", ASC);
		gett("");
		if (ret >= 5) {
			powerScale = 1;
			if (replystr.find("^OP1;") != std::string::npos) {
				cmd = "^PF;";
				ret = wait_char(';', 8, KX3_WAIT_TIME, "get KXPA power out", ASC);
				if (ret >= 8) {
					p = replystr.rfind("^PF");
					if (p == std::string::npos) return 0;
					replystr[p + 7] = 0;
					mtr = atoi(&replystr[p+3]);
					if (mtr > 0) return mtr/10;
				}
			} // in standby mode, fall through to read power out on KX3
		}
	}

	cmd = "PO;";

	get_trace(1, "get power out");
	ret = wait_char(';', 6, KX3_WAIT_TIME, "get power out", ASC);
	gett("");

	if (ret < 6) return 0;

	p = replystr.rfind("PO");
	if (p == std::string::npos) return 0;
	replystr[p + 5] = 0;
	mtr = atoi(&replystr[p + 2]); 
// mtr is power in tenths
	powerScale = 10;
	return mtr;
}

// Transceiver rf control
void RIG_KX3::set_rf_gain(int val)
{
	int ival = 250 + val;
	if (ival < 190) ival = 190;
	if (ival > 250) ival = 250;
	cmd = "RG000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}

	set_trace(1, "set rf gain");
	sendCommand(cmd, 0, 50);
	sett("");
	showresp(INFO, ASC, "set rfgain ctrl", cmd, replystr);
}

int RIG_KX3::get_rf_gain()
{
	cmd = "RG;";

	get_trace(1, "get rf gain");
	int ret = wait_char(';', 6, KX3_WAIT_TIME, "get RF gain", ASC);
	gett("");

	if (ret < 6) return progStatus.rfgain;
	size_t p = replystr.rfind("RG");
	if (p == std::string::npos) return progStatus.rfgain;

	replystr[p + 5] = 0;
	int v = atoi(&replystr[p + 2]);
	v -= 250;
	if (v < -60) v = -60;
	if (v > 0) v = 0;
	return v;
}

void RIG_KX3::get_rf_min_max_step(int &min, int &max, int &step)
{
   min = -60; max = 0; step = 1;
}

// Transceiver mic control
void RIG_KX3::set_mic_gain(int val)
{
	int ival = (int)val;
	cmd = "MG000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}

	set_trace(1, "set mic gain");
	sendCommand(cmd, 0, 50);
	sett("");
	showresp(INFO, ASC, "set mic ctrl", cmd, replystr);
}

int RIG_KX3::get_mic_gain()
{
	cmd = "MG;";

	get_trace(1, "get mic gain");
	int ret = wait_char(';', 6, KX3_WAIT_TIME, "get MIC gain", ASC);
	gett("");

	if (ret < 6) return progStatus.mic_gain;
	size_t p = replystr.rfind("MG");
	if (p == std::string::npos) return progStatus.mic_gain;

	replystr[p + 5] = 0;
	int v = atoi(&replystr[p + 2]);
	return v;
}

void RIG_KX3::get_mic_min_max_step(int &min, int &max, int &step)
{
   min = 0; max = 60; step = 1;
}

// Tranceiver PTT on/off
void RIG_KX3::set_PTT_control(int val)
{
	set_trace(1, "set PTT");
	if (val) cmd = "TX;";
	else     cmd = "RX;";
	sendCommand(cmd, 0, 50);
	sett("");
	ptt_ = val;
}

/*
 * IF (Transceiver Information; GET only)
 *   RSP format: IF[f]*****+yyyyrx*00tmvspbd1*;
 *   where the fields are defined as follows:
 *     [f]    Operating frequency, excluding any RIT/XIT offset
 *            (11 digits; see FA command format)
 *      *     represents a space (BLANK, or ASCII 0x20)
 *      +     either "+" or "-" (sign of RIT/XIT offset)
 *      yyyy  RIT/XIT offset in Hz (range is -9999 to +9999 Hz when
 *            computer-controlled)
 *      r     1 if RIT is on, 0 if off
 *      x     1 if XIT is on, 0 if off
 *      t     1 if the K3 is in transmit mode, 0 if receive
 *      m     operating mode (see MD command)
 *      v     receive-mode VFO selection, 0 for VFO A, 1 for VFO B
 *      s     1 if scan is in progress, 0 otherwise
 *      p     1 if the transceiver is in split mode, 0 otherwise
 *      b     Basic RSP format: always 0;
 *            K2 Extended RSP format (K22):
 *            1 if present IF response is due to a band change; 0 otherwise
 *      d     Basic RSP format: always 0;
 *            K3 Extended RSP format (K31):
 *      1     DATA sub-mode, if applicable
 *              (0=DATA A, 1=AFSK A, 2= FSK D, 3=PSK D)
 * The fixed-value fields (space, 0, and 1) are provided for syntactic
 * compatibility with existing software.
 *
 * 01234567890123456789012345678901234567
 * 0         1         2         3      7
 * IF00014070000*****+yyyyrx*00tmvspbd1*;
 *   |         |               |   |  |_____ [35] data submode 
 *   |         |               |   |________ [32] split on = '1', off = '0'
 *   |         |               |____________ [28] PTT state; 1 = TX, 0 = RX 
 *   |---------|____________________________ [02...12] vfo a/b
*/

int RIG_KX3::get_PTT()
{
	cmd = "IF;";
	get_trace(1, "get PTT");
	int ret = wait_char(';', 38, KX3_WAIT_TIME, "get split", ASC);
	gett("");
	if (ret < 38) return ptt_;
	size_t p = replystr.rfind("IF");
	return ptt_ = (replystr[p+28] == '1');
}

// BG (Bargraph Read; GET only)
// RSP format: BGnn; where <nn> is 00 (no bars) through 10 (bar 10) if the
// bargraph is in DOT mode, and 12 (no bars) through 22 (all 10 bars) if
// the bargraph is in BAR mode. Reads the S-meter level on receive. Reads
// the power output level or ALC level on transmit, depending on the RF/ALC
// selection. Also see SM/SM$ command, which can read either main or sub RX
// S-meter level.

// SM SM$ (S-meter Read; GET only)
// Basic RSP format: SMnnnn; where nnnn is 0000-0015.
// S9=6; S9+20=9; S9+40=12; S9+60=15.
// KX3 Extended RSP format (KX31): nnnn is 0000-0021.
// S9=9; S9+20=13; S9+40=17; S9+60=21.
// This command can be used to obtain either the main (SM) or sub (SM$)
// S-meter readings. Returns 0000 in transmit mode. BG can be used to
// simply emulate the bar graph level, and applies to either RX or TX mode.

int RIG_KX3::get_smeter()
{
	cmd = "SM;";

	get_trace(1, "get smeter");
	int ret = wait_char(';', 7, KX3_WAIT_TIME, "get Smeter", ASC);
	gett("");

	if (ret < 7) return 0;

	size_t p = replystr.rfind("SM");
	if (p == std::string::npos) return 0;

	int mtr = fm_decimal(replystr.substr(p+2), 4);
// use extended format conversion
	if (mtr <= 9) mtr = (int) (50.0 * mtr / 9.0);
	else mtr = (int)(50 + (mtr - 9.0) * 50.0 / 12.0);

	return mtr;
}

void RIG_KX3::set_noise(bool on)
{
	set_trace(1, "set noise blanker");
	if (on) cmd = "NB1;";
	else    cmd = "NB0;";
	sendCommand(cmd, 0, 50);
	sett("");
}

int RIG_KX3::get_noise()
{
	cmd = "NB;";

	get_trace(1, "get noise blanker");
	int ret = wait_char(';', 4, KX3_WAIT_TIME, "get Noise Blanker", ASC);
	gett("");

	if (ret < 4) return progStatus.noise;
	size_t p = replystr.rfind("NB");
	if (p == std::string::npos) return progStatus.noise;
	return (replystr[p+2] == '1' ? 1 : 0);
}

// BW $ (Filter Bandwidth and Number; GET/SET)
// KX3 Extended SET/RSP format (K31): BWxxxx; where xxxx is 0-9999, the bandwidth in 10-Hz units. May be
// quantized and/or range limited based on the present operating mode.
// BW00050 => BW0005;
// 50 - 1000 in 50 Hz increments
// 1000 - 3000 in 100 Hz increments
// 3000 - 4000 in 200 Hz increments

static std::string KX3_bws = "\
005;010;015;020;025;030;035;040;045;050;\
055;060;065;070;075;080;085;090;095;100;\
110;120;130;140;150;160;170;180;190;200;\
210;220;230;240;250;260;270;280;290;300;\
320;340;360;380;400;";

void RIG_KX3::set_bwA(int val)
{
	cmd.assign("BW0").append(KX3_bws.substr(val*4, 4));

	set_trace(2, "set bwA", KX3_widths[val]);
	sendCommand(cmd, 0, 50);
	sett("");
	showresp(INFO, ASC, "set bw A", cmd, replystr);
}

int RIG_KX3::get_bwA()
{
	cmd = "BW;";

	get_trace(1, "get bwA");
	int ret = wait_char(';', 7, KX3_WAIT_TIME, "get bandwidth A", ASC);
	gett("");

	if (ret < 7) return bwA;
	std::string bw = replystr.substr( replystr.length() - 4, 4 );

	size_t p = 0;
	while (p < KX3_bws.length()) {
		if (bw <= KX3_bws.substr(p, 4)) break;
		p += 4;
	}
	if ((bw < KX3_bws.substr(p,4)) && bw[2] < '5') p -= 4;
	if (p < 0) p = 0;
	if (p < KX3_bws.length()) bwA = p / 4;

	return bwA;
}

void RIG_KX3::set_bwB(int val)
{
	cmd.assign("BW0").append(KX3_bws.substr(val*4, 4));

	set_trace(2, "set bwB", KX3_widths[val]);
	sendCommand(cmd, 0, 50);
	sett("");
	showresp(INFO, ASC, "set bw B", cmd, replystr);
}

int RIG_KX3::get_bwB()
{
	cmd = "BW;";

	get_trace(1, "get bwA");
	int ret = wait_char(';', 7, KX3_WAIT_TIME, "get bandwidth B", ASC);
	gett("");

	if (ret < 7) return bwB;
	std::string bw = replystr.substr( replystr.length() - 4, 4 );

	size_t p = 0;
	while (p < KX3_bws.length()) {
		if (bw <= KX3_bws.substr(p, 4)) break;
		p += 4;
	}
	if ((bw < KX3_bws.substr(p,4)) && bw[2] < '5') p -= 4;
	if (p < 0) p = 0;
	if (p < KX3_bws.length()) bwB = p / 4;

	return bwB;
}

bool RIG_KX3::can_split()
{
	return true;
}

void RIG_KX3::set_split(bool val)
{
	set_trace(1, "set split");
	if (val) {
		cmd = "FT1;";
		sendCommand(cmd, 0, 50);
		showresp(INFO, ASC, "set split ON", cmd, replystr);
	} else {
		cmd = "FR0;";
		sendCommand(cmd, 0, 50);
		showresp(INFO, ASC, "set split OFF", cmd, replystr);
	}
	sett("");
	split_on = val;
}

// 01234567890123456789012345678901234567
// 0         1         2         3      7
// IF00014070000*****+yyyyrx*00tmvspbd1*;
//   |---------|                   |
//     vfo a/b                     split on = '1', off = '0'
// IF00014070000     -000000 0002000011 ;  OFF
// IF00014070000     -000000 0002001011 ;  ON

int RIG_KX3::get_split()
{
	cmd = "IF;";

	get_trace(1, "get split");
	int ret = wait_char(';', 38, KX3_WAIT_TIME, "get split", ASC);
	gett("");

	if (ret < 38) return split_on;
	size_t p = replystr.rfind("IF");
	if (p == std::string::npos) return split_on;
	split_on = replystr[p+32] - '0';
	return split_on;
}

void RIG_KX3::set_if_shift(int val)
{
	if (val < if_shift_min) val = 1500;
	if (val > if_shift_max) val = 1500;

	cmd = "IS 0000;";
	cmd[6] += val % 10; val /= 10;
	cmd[5] += val % 10; val /= 10;
	cmd[4] += val % 10; val /= 10;
	cmd[3] += val % 10;

	set_trace(1, "set if shift");
	sendCommand(cmd, 0, 50);
	sett("");
	showresp(INFO, ASC, "set if shift", cmd, replystr);
}

bool RIG_KX3::get_if_shift(int &val)
{
	cmd = "IS;";

	get_trace(1, "get if shift");
	wait_char(';', 8, KX3_WAIT_TIME, "get IF shift", ASC);
	gett("");

	val = progStatus.shift_val;

	if (replystr.length() < 8) return 1;

	int sh_val = 0;

	size_t p = replystr.rfind("IS ");
	if (p == std::string::npos)
		return progStatus.shift;

	sh_val = (((replystr[p+3] - '0') * 10 + 
			   (replystr[p+4] - '0') ) * 10 + 
			   (replystr[p+5] - '0')  ) * 10 + 
			   (replystr[p+6] - '0');

	val = progStatus.shift_val = sh_val;

	if (sh_val < if_shift_min) sh_val = if_shift_min;
	if (sh_val > if_shift_max) sh_val = if_shift_max;

	return 1;
}

void  RIG_KX3::get_if_mid()
{
	cmd = "IS 9999;";

	set_trace(1, "set center pbt");
	sendCommand(cmd, 0, 50);
	sett("");
	showresp(INFO, ASC, "center pbt", cmd, replystr);

	cmd = "IS;";
	get_trace(1, "get center pbt");
	int ret = wait_char(';', 8, 500, "get PBT center", ASC);
	gett("");

	if (ret < 8) return;
	size_t p = replystr.rfind("IS ");
	if (p == std::string::npos) return;
	sscanf(&replystr[p + 3], "%d", &if_shift_mid);
}

// KX3_LSB, KX3_USB, KX3_CW, KX3_FM, KX3_AM, KX3_DATA, KX3_CWR, KX3_DATAR
void RIG_KX3::get_if_min_max_step(int &min, int &max, int &step)
{
	int md = modeA;
	if (inuse == onB) md = modeB;
	switch (md) {
		case KX3_CW: case KX3_CWR:
			if_shift_min = 330;
			if_shift_max = 770;
			if_shift_step = 10;
			break;
		case KX3_LSB:
		case KX3_USB:
		case KX3_DATA:
		case KX3_DATAR:
			if_shift_min = 1280;
			if_shift_max = 1700;
			if_shift_step = 10;
			break;
		default:
			if_shift_min = 1500;
			if_shift_max = 1500;
			if_shift_step = 10;
			break;
	}
	min = if_shift_min;
	max = if_shift_max;
	step = if_shift_step;
	get_if_mid();
}

// ^OP; return ^OPn;  5 characters
//      ^OP1; signifies that KXPA100 is on-line and active
// SW; return SWnnn;  6 characters
// ^SW; return ^SWnnn;  7 characters (don't believe the programmer's guide!)
int RIG_KX3::get_swr()
{
	if (options.KXPA) {
		cmd = "^OP;";
		get_trace(1, "test KXPA");
		int ret = wait_char(';', 5, KX3_WAIT_TIME, "test KXPA", ASC);
		gett("");
		if (ret >= 5) {
			powerScale = 1;
			if (replystr.find("^OP1;") != std::string::npos) {
				cmd = "^SW;";
				get_trace(1, "get KXPA SWR");
				ret = wait_char(';', 7, KX3_WAIT_TIME, "get KXPA SWR reading", ASC);
				gett("");
				if (ret >= 7) {
					size_t p = replystr.rfind("^SW");
					if (p != std::string::npos) {
						replystr[p + 6] = 0;
						int mtr = fm_decimal(replystr.substr(p+3), 3);
						if (mtr <= 30) mtr = (int) (50.0 * (mtr - 10) / 20.0);
						else mtr = (int)(50 + 50 * (mtr - 30) / 70);
						return mtr;
					}
				}
			} // in standby mode or not present, fall through to read SWR on KX3
		}
	}

	cmd = "SW;";

	get_trace(1, "get swr");
	int ret = wait_char(';', 6, KX3_WAIT_TIME, "get Smeter", ASC);
	gett("");

	if (ret < 6) return 0;

	size_t p = replystr.rfind("SW");
	if (p == std::string::npos) return 0;

	int mtr = fm_decimal(replystr.substr(p+2), 3);
	if (mtr <= 30) mtr = (int) (50.0 * (mtr - 10) / 20.0);
	else mtr = (int)(50 + 50 * (mtr - 30) / 70);

	return mtr;

}
