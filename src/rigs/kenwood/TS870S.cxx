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
//
// Changes for the TS-870S March 2012, Dave Baxter, G0WBX
//
// Todo:
// Look at:-												(Example)
// Autonotch control:- 	(SSB Only on the 870S)				(TS-2000)
// Noise Reduction control:- 								(IC-7700)
// Extra controls for speach compressor etc:- 				(IC-9100 (and others))
// Misc controls auto rig on/off with flrig startup/quit:-	(FT-450/950) (PS0; PS1;)
// Check Autotune action, and any 'busy' status.

#include "config.h"
#include "kenwood/TS870S.h"
#include "support.h"

static const char TS870Sname_[] = "TS-870S";

//	   mode array Index Values :-         0      1      2     3     4     5      6       7
static const char *TS870Smodes_[] = {   "LSB", "USB", "CW", "FM", "AM", "FSK", "CW-R", "FSK-R", NULL};
static const char TS870S_mode_type[] = { 'L',   'U',   'U',  'U',  'U',  'L',   'L',    'U' };

//----------------------------------------------------------------------
static const char *TS870S_empty[] = { "N/A", NULL };

//----------------------------------------------------------------------
// FWnnnn; command structure
// Allowable data(units of 10 Hz):
// FM: 500, 600, 800, 1000, 1200, 1400 (bandwidth)
// AM: 0, 10, 20, 50 (high pass freq)

// Sets or reads IF shift. 
// For SSB and AM, sets or reads the low-pass filter cut-off frequency.
//
// IS(p1)(p2)(p2)(p2)(p2);
// If P2 is positive or zero, P1 can be “+” or “ ”.
// Allowable data (in units of Hz):
// SSB: 1400, 1600, 1800, 2000, 2200,
// 2400, 2600, 2800, 3000, 3400, 4600, 6000
// AM:2500, 3000, 4000, 5000, 6000, 7000,
// CW:400~1000 (50 Hz steps)
//----------------------------------------------------------------------
// CW: 5, 10, 20, 40, 60, 100 (bandwidth)

static int DEF_CW_BW = 4;

static const char *TS870S_CWwidths[] = { // Available CW bandwidths
"50", "100", "200", "400", "600", "1000", NULL};

static const char *TS870S_CWbw[] = { // Corresponding commands.
"FW0005;", "FW0010;", "FW0020;", "FW0040;", "FW0060;", "FW0100;", NULL};

//----------------------------------------------------------------------
// FSK: 25, 50, 100, 150 (bandwidth)
static int DEF_FSK_BW = 2;

static const char *TS870S_FSKwidths[] = { // Available FSK bandwidths
"250", "500", "1000", "1500", NULL};

static const char *TS870S_FSKbw[] = { // Corresponding commands.
"FW0025;", "FW0050;", "FW0100;", "FW0150;", NULL };

//----------------------------------------------------------------------
// SSB: 0, 5, 10, 20, 30, 40, 50, 60, 80, 100  (high pass freq)

static int DEF_SL_SH = 0x8704;

static const char *TS870S_SSB_SL[] = { 			// MD1; and MD2;
  "0",   "50", "100", "200", "300", 			// Available settings (Hz)
"400",  "500", "600", "800", "1000", NULL };

static const char *TS870S_CAT_ssb_SL[] = {		// Corresponding commands.
"FW0000;", "FW0005;", "FW0010;", "FW0020;", "FW0030;",
"FW0040;", "FW0050;", "FW0060;", "FW0080;", "FW0100;", NULL };

static const char *TS870S_SSB_SL_tooltip = "lo cut";
static const char *TS870S_SSB_btn_SL_label = "L";

static const char *TS870S_SSB_SH[] = { 			// MD1; and MD2;
"1400", "1600", "1800", "2000", "2200", "2400", // Available settings (Hz)
"2600", "2800", "3000", "3400", "4600", "6000", NULL };

static const char *TS870S_CAT_ssb_SH[] = {		// Corresponding commands.
"IS 1400;", "IS 1600;", "IS 1800;", "IS 2000;", "IS 2200;", "IS 2400;",
"IS 2600;", "IS 2800;", "IS 3000;", "IS 3400;", "IS 4600;", "IS 6000;", NULL };

static const char *TS870S_SSB_SH_tooltip = "hi cut";
static const char *TS870S_SSB_btn_SH_label = "H";

//----------------------------------------------------------------------
static int DEF_AM_SL_SH = 0x8401;

static const char *TS870S_AM_SL[] = { // Available values...
"0", "100", "200", "500", NULL };

static const char *TS870S_CAT_am_SL[] = { // ... Corresponding commands.
"FW0000;", "FW0010;", "FW0020;", "FW0050;", NULL};

static const char *TS870S_AM_SL_tooltip = "lo cut";
static const char *TS870S_AM_btn_SL_label = "L";

static const char *TS870S_AM_SH[] = {
"2500", "3000", "4000", "5000", "6000", "7000", NULL };

static const char *TS870S_CAT_am_SH[] = {
"IS 2500;", "IS 3000;", "IS 4000;", "IS 5000;", "IS 6000;", "IS 7000;", NULL};

static const char *TS870S_AM_SH_tooltip = "hi cut";
static const char *TS870S_AM_btn_SH_label = "H";

//----------------------------------------------------------------------
static int DEF_FM_BW = 4;
static const char *TS870S_FMwidths[] = { // Available FM bandwidths
"5000", "6000", "8000", "10000", "12000", "14000", NULL};

static const char *TS870S_FMbw[] = { // Corresponding commands.
"FW0500;", "FW0600;", "FW0800;", "FW1000;", "FW1200;", "FW1400;", NULL };

//----------------------------------------------------------------------

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 125, 156 },
	{ (Fl_Widget *)sldrPOWER,    54, 145, 368 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

//----------------------------------------------------------------------
void RIG_TS870S::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = btnIFsh;
	rig_widgets[3].W = sldrIFSHIFT;
	rig_widgets[4].W = sldrMICGAIN;
	rig_widgets[5].W = sldrPOWER;

	selectA();
//	cmd = "AC 00;";       sendCommand(cmd);  // ATU Off.
	cmd = "EX0270000;";   sendCommand(cmd);  // TX Enable.		wbx2

	get_preamp();
	get_attenuator();
}

// add code here for shutting down the transceiver ...
void RIG_TS870S::shutdown()
{
}

//----------------------------------------------------------------------
RIG_TS870S::RIG_TS870S() {

	name_ = TS870Sname_;
	modes_ = TS870Smodes_;
	bandwidths_ = TS870S_SSB_SH;

	dsp_SL     = TS870S_SSB_SL;
	SL_tooltip = TS870S_SSB_SL_tooltip;
	SL_label   = TS870S_SSB_btn_SL_label;

	dsp_SH     = TS870S_SSB_SH;
	SH_tooltip = TS870S_SSB_SH_tooltip;
	SH_label   = TS870S_SSB_btn_SH_label;

	widgets = rig_widgets;

	serial_baudrate = BR57600;
	stopbits = 1;
	serial_retries = 2;

	serial_write_delay = 0;
	serial_post_write_delay = 5;

	serial_timeout = 50;
	serial_rtscts = true;
	serial_rtsplus = false;
	serial_dtrplus = false;
	serial_catptt = true;
	serial_rtsptt = false;    // ditto (used for hardware handshake)
	serial_dtrptt = false;    // ditto

//	Defaults.
	B.imode = A.imode = USB;
	B.iBW = A.iBW = DEF_SL_SH;
	B.freq = A.freq = 14070000ULL;
	can_change_alt_vfo = true;

	nb_level = 2;

	has_micgain_control =
	has_notch_control =
	has_ifshift_control =
	has_swr_control = false;

	has_mode_control =
	has_bandwidth_control =
	has_dsp_controls =
	has_smeter =
	has_power_out =
	has_split =
	has_split_AB =
	has_rf_control =
	has_ifshift_control =
	has_noise_control =
	has_micgain_control =
	has_volume_control =
	has_ptt_control =
	has_attenuator_control =
	has_power_control = true;

	has_tune_control = false; // disabled for now

	has_preamp_control = false;

	precision = 10;
	ndigits = 7;

}

//----------------------------------------------------------------------
const char * RIG_TS870S::get_bwname_(int n, int md)
{ // md is an index value, NOT a "mode" designator itself.
  //  n also is an index value, used to reference the name in the array.

  // This uses "ternary" methods, in the form of...
  // (boolean test) ? (do if true) : (do if false);
  // http://www.teach-me-c.com/blog/c-ternary-operator

	static char bwname[20];

	if (n > 256) { // hi/lo cutt off bw setting mode.
		int hi = (n >> 8) & 0x7F; // hi byte (not MSB)
		int lo = n & 0xFF;        // lo byte
		snprintf(bwname, sizeof(bwname), "%s/%s",
			(md == 0 || md == 1) ? TS870S_SSB_SL[lo] :	// SSB lo
			(md == 4) ? TS870S_AM_SL[lo] : "N/A",		//  AM lo
			(md == 0 || md == 1) ? TS870S_SSB_SH[hi] :	// SSB hi
			(md == 4) ? TS870S_AM_SH[hi] : "N/A" );		//  AM hi

	} else { // plain vanilla single bandwidth mode.
		snprintf(bwname, sizeof(bwname), "%s",
			(md == 2 || md == 6) ? TS870S_CWwidths[n] :	//  CW or CW-R
		    (md == 5 || md == 7) ? TS870S_FSKwidths[n]:	// FSK or FSK-R
			(md == 3 ) ? TS870S_FMwidths[n] : "N/A" );  //  FM
	}
	return bwname;
}

//----------------------------------------------------------------------
void RIG_TS870S::selectA()
{
	cmd = "FR0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx/Tx A", cmd, "");
//	vfo = 0;
	inuse = onA;
}

//----------------------------------------------------------------------
void RIG_TS870S::selectB()
{
	cmd = "FR1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx/Tx B", cmd, "");
//	vfo = 1;
	inuse = onB;
}


//----------------------------------------------------------------------
bool RIG_TS870S::check ()
{
	return get_TS870Sid();
}

unsigned long long RIG_TS870S::get_vfoA ()
{
	cmd = "FA;";
	get_trace(1, "get_vfoA()");
	int ret = wait_char(';', 14, 100, "get vfoA", ASC);
	gett("");
	if (ret < 14)
		return A.freq;

	size_t p = replystr.rfind("FA");
	if (p == std::string::npos) return A.freq;

	unsigned long long f = 0ULL;
	unsigned long long mul = 1ULL;
	for (size_t n = 12; n > 1; n--) {
		f += (replystr[p + n] - '0') * mul;
		mul *= 10;
	}
	A.freq = f;
	return A.freq;
}

//----------------------------------------------------------------------
void RIG_TS870S::set_vfoA (unsigned long long freq)
{
	A.freq = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfo A", cmd, "");
}

//----------------------------------------------------------------------
unsigned long long RIG_TS870S::get_vfoB ()
{
	cmd = "FB;";
	get_trace(1, "get_vfoB()");
	int ret = wait_char(';', 14, 100, "get vfoB", ASC);
	gett("");
	if (ret < 14)
		return B.freq;

	size_t p = replystr.rfind("FB");
	if (p == std::string::npos) return B.freq;

	unsigned long long f = 0ULL;
	unsigned long long mul = 1ULL;
	for (size_t n = 12; n > 1; n--) {
		f += (replystr[p + n] - '0') * mul;
		mul *= 10;
	}
	B.freq = f;

	return B.freq;
}

//----------------------------------------------------------------------
void RIG_TS870S::set_vfoB (unsigned long long freq)
{
	B.freq = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfo B", cmd, "");
}

//----------------------------------------------------------------------
int RIG_TS870S::get_smeter() {
	cmd = "SM;";
	get_trace(1, "get_smeter()");
	int ret = wait_char(';', 7, 100, "get smeter", ASC);
	gett("");
	if (ret < 7)
		return 0;

	size_t p = replystr.rfind("SM");
	if (p == std::string::npos) return -1;

	replystr[p + 6] = 0;
	int mtr = atoi(&replystr[p + 2]);
	mtr = (mtr * 100) / 30;										//wbx
	return mtr;
}

//----------------------------------------------------------------------
int RIG_TS870S::get_power_out()
{ // Shares same command as the 'S' meter reading.
	int mtr = 0;
	cmd = "SM;";
	get_trace(1, "get_power_out()");
	int ret = wait_char(';', 7, 100, "get output power", ASC);
	gett("");
	if (ret < 7)
		return mtr;

	size_t p = replystr.rfind("SM");
	if (p == std::string::npos) return mtr;

	replystr[p + 6] = 0;
	mtr = atoi(&replystr[p + 2]);
	mtr = (mtr * 100) / 30;										//wbx
	if (mtr > 100) mtr = 100; // but with a power limit!
	return mtr;

	// The power meter scale is not the same as Flrig's default.
	// a change is needed, one day.   wbx2
	// Note that the power meter scale is an image and would have to be
	// redrawn and then set specifically for the xcvr in support.cxx
}

//----------------------------------------------------------------------
// Transceiver power level
void RIG_TS870S::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd); // 010 .. 100
}

//----------------------------------------------------------------------
double RIG_TS870S::get_power_control()
{
	cmd = "PC;";
	get_trace(1, "get_power_control()");
	gett("");
	int ret = wait_char(';', 6, 100, "get pwr ctrl", ASC);
	if (ret < 6) return 0;

	size_t p = replystr.rfind("PC");
	if (p == std::string::npos) return 0;
	int mtr = 0;
	replystr[p + 5] = 0;
	mtr = atoi(&replystr[p + 2]);
	return mtr;
}

//----------------------------------------------------------------------
// Volume control return (rig sends back 0 .. 255)
int RIG_TS870S::get_volume_control()
{
	cmd = "AG;";  // reply data is 'AGnnn;' nnn = 000 .. 255  wbx
	get_trace(1, "get_volume_control()");
	int ret = wait_char(';', 6, 100, "get vol ctrl", ASC);
	gett("");
	if (ret < 6) return 0;

	size_t p = replystr.rfind("AG");
	if (p == std::string::npos) return 0;

	replystr[p + 5] = 0; // change the ';' into a 'null'
	int val = atoi(&replystr[p + 2]);
	return (int)(val / 2.55);  // we only want 0 .. 100
}

//----------------------------------------------------------------------
void RIG_TS870S::set_volume_control(int val) // 0 .. 100
{
	int ivol = (int)(val * 2.55); // 0 .. 255
	cmd = "AG000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendCommand(cmd);
}

//----------------------------------------------------------------------
// Tranceiver PTT on/off

/*
 * NOTE!  If you use these CAT commands TX and RX, then when in Transmit
 * the TS-870S *ONLY* takes audio from the front panel Mic socket.
 * If you need to use the audio going in the rear Acc socket, you MUST
 * use a hard wired PTT to the Acc socket.  THEN it will take audio from
 * there, and blank the mic.
 * To do that, in the Config/Xcvr Select menu, use one of the Hardware PTT options.
 * Just so it has been said...
 * 
 * BUMMER - HKJ
 */

void RIG_TS870S::set_PTT_control(int val)
{
	if (val) cmd = "TX;";
	else	 cmd = "RX;";
	sendCommand(cmd);
}

//----------------------------------------------------------------------
// Play with the autotuner		// disabled for now wbx
void RIG_TS870S::tune_rig()
{
//	cmd = "AC111;";
//	sendCommand(cmd);

	// this might take a while!
}

//----------------------------------------------------------------------
// the TS-870 actually has 4 attenuator settings.
// RA00; = Off,  RA01; = 6dB,  RA02; = 12dB,  RA03; = 18dB      wbx

int  RIG_TS870S::next_attenuator()
{
	switch (atten_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 3;
		case 3: return 0;
	}
	return 0;
}

void RIG_TS870S::set_attenuator(int val)
{
	atten_level = val;
	if (atten_level == 1) {			// If attenuator level = 0 (off)
		cmd = "RA01;";				// this is the command...
		atten_label("Att 6", true);	// show it in the button...
	}
	else if (atten_level == 2) {
		cmd = "RA02;";
		atten_label("Att 12", true);
	}
	else if (atten_level == 3) {
		cmd = "RA03;";
		atten_label("Att 18", true);
	}
	else if (atten_level == 0) {
		cmd = "RA00;";
		atten_label("Att", false);
	}

	sendCommand(cmd);
	showresp(WARN, ASC, "set Att", cmd, "");
}

//----------------------------------------------------------------------
//	Modified to read and show the actual radio setting, in the button.
int RIG_TS870S::get_attenuator() {
	cmd = "RA;";
	get_trace(1, "get_attenuator()");
	int ret = wait_char(';', 5, 100, "get att", ASC);
	gett("");
	if (ret < 5)
		return att_on;

	size_t p = replystr.rfind("RA");
	if (p == std::string::npos) return att_on;

	if (replystr[p + 2] == '0' && replystr[p + 3] == '0') {
		att_on = 0;						// Attenuator is OFF
		atten_level = 0;					// remember it...
		atten_label("Att", false);		// show it...
	}
	else if (replystr[p + 2] == '0' && replystr[p + 3] == '1') {
		att_on = 1;						// Attenuator is ON, 6dB
		atten_level = 1;					// remember the level
		atten_label("Att 6", true);		// show it...
	}
	else if (replystr[p + 2] == '0' && replystr[p + 3] == '2') {
		att_on = 1;						// .. still ON, 12dB
		atten_level = 2;					// remember this level
		atten_label("Att 12", true);	// show it.
	}
	else if (replystr[p + 2] == '0' && replystr[p + 3] == '3') {
		att_on = 1;						// .. still ON 18dB
		atten_level = 3;					// remember...
		atten_label("Att 18", true);	// show this too..
	}

	return att_on;			// let the rest of the world know.
}

//----------------------------------------------------------------------
bool RIG_TS870S::get_TS870Sid() {
	cmd = "ID;";
	get_trace(1, "get_ID");
	int ret = wait_char(';', 6, 100, "get ID", ASC);
	gett("");
	if ( (ret < 6) || (replystr.rfind("ID015;") == std::string::npos) )
		return false;
	return true;
}

//======================================================================
// mode commands
//======================================================================
void RIG_TS870S::set_modeA(int val)
{
	active_mode = A.imode = val;
	switch (val) {
		case 0: cmd = "MD1;"; break;
		case 1: cmd = "MD2;"; break;
		case 2: cmd = "MD3;"; break;
		case 3: cmd = "MD4;"; break;
		case 4: cmd = "MD5;"; break;
		case 5: cmd = "MD6;"; break;
		case 6: cmd = "MD7;"; break;
		case 7: cmd = "MD9;"; break;
	}
	sendCommand(cmd);
	set_trace(2, "set_modeA ", cmd.c_str());
	showresp(WARN, ASC, "set mode A", cmd, "");

	A.iBW = set_widths(val);
}

//----------------------------------------------------------------------
int RIG_TS870S::get_modeA()
{
	int md = A.imode;
	cmd = "MD;";
	get_trace(1, "get_modeA()");
	int ret = wait_char(';', 4, 100, "get mode A", ASC);
	gett("");
	if (ret < 4)
		return A.imode;

	size_t p = replystr.rfind("MD");
	if (p == std::string::npos) return A.imode;

	switch (replystr[p + 2]) {
		case '1' : md = tsLSB;  break;
		case '2' : md = tsUSB;  break;
		case '3' : md = tsCW;   break;
		case '4' : md = tsFM;   break;
		case '5' : md = tsAM;   break;
		case '6' : md = tsFSK;  break;
		case '7' : md = tsCWR;  break;
		case '9' : md = tsFSKR; break;
		default  : md = A.imode;
	}

	if (md != A.imode) {
		active_mode = A.imode = md;
		A.iBW = set_widths(md);
	}
	return A.imode;
}

//----------------------------------------------------------------------
void RIG_TS870S::set_modeB(int val)
{
	active_mode = B.imode = val;
	switch (val) {
		case 0: cmd = "MD1;"; break;
		case 1: cmd = "MD2;"; break;
		case 2: cmd = "MD3;"; break;
		case 3: cmd = "MD4;"; break;
		case 4: cmd = "MD5;"; break;
		case 5: cmd = "MD6;"; break;
		case 6: cmd = "MD7;"; break;
		case 7: cmd = "MD9;"; break;
	}
	sendCommand(cmd);
	set_trace(2, "set_modeB ", cmd.c_str());
	showresp(WARN, ASC, "set mode B", cmd, "");

	B.iBW = set_widths(val);
}

//----------------------------------------------------------------------
int RIG_TS870S::get_modeB()
{
	int md = B.imode;
	cmd = "MD;";
	int ret = wait_char(';', 4, 100, "get mode B", ASC);
	gett("");
	if (ret < 4)
		return B.imode;

	size_t p = replystr.rfind("MD");
	if (p == std::string::npos) return B.imode;

	switch (replystr[p + 2]) {
		case '1' : md = tsLSB;  break;
		case '2' : md = tsUSB;  break;
		case '3' : md = tsCW;   break;
		case '4' : md = tsFM;   break;
		case '5' : md = tsAM;   break;
		case '6' : md = tsFSK;  break;
		case '7' : md = tsCWR;  break;
		case '9' : md = tsFSKR; break;
		default  : md = B.imode;
	}

	if (md != B.imode) {
		active_mode = B.imode = md;
		B.iBW = set_widths(md);
	}
	return B.imode;
}


//======================================================================
// Bandpass filter commands
//======================================================================
// Comes here when user changes mode, among other things..
int RIG_TS870S::set_widths(int val) // val is from the mode list index, as selected by the user.
{
	int bw = 0;

	switch (val) {
		case tsLSB:
		case tsUSB: {  // SSB modes
			bandwidths_ = TS870S_SSB_SH;
			dsp_SL = TS870S_SSB_SL;
			dsp_SH = TS870S_SSB_SH;
			SL_tooltip = TS870S_SSB_SL_tooltip;
			SL_label   = TS870S_SSB_btn_SL_label;
			SH_tooltip = TS870S_SSB_SH_tooltip;
			SH_label   = TS870S_SSB_btn_SH_label;
			bw = DEF_SL_SH; // 300 ... 2800 Hz
			break;
		}
		case tsCW:
		case tsCWR: {  // CW modes
			bandwidths_ = TS870S_CWwidths;
			dsp_SL = TS870S_empty;
			dsp_SH = TS870S_empty;
			bw = 2; // 200Hz
			break;
		}
		case tsFSK:
		case tsFSKR: {  // FSK modes
			bandwidths_ = TS870S_FSKwidths;
			dsp_SL = TS870S_empty;
			dsp_SH = TS870S_empty;
			bw = 1; // 500Hz
			break;
		}
		case tsAM: { // val == 5 ==> AM mode
			bandwidths_ = TS870S_empty;
			dsp_SL = TS870S_AM_SL;
			dsp_SH = TS870S_AM_SH;
			SL_tooltip = TS870S_AM_SL_tooltip;
			SL_label   = TS870S_AM_btn_SL_label;
			SH_tooltip = TS870S_AM_SH_tooltip;
			SH_label   = TS870S_AM_btn_SH_label;
			bw = 0x8401; // 100Hz .. 6000Hz
			break;
		}
		case tsFM: {  // FM mode
			bandwidths_ = TS870S_FMwidths;  // load the dropdown with our list
			dsp_SL = TS870S_empty;
			dsp_SH = TS870S_empty;
			bw = 4; // 10000Hz
			break;
		}
	}
	return bw;
}

//----------------------------------------------------------------------
const char **RIG_TS870S::bwtable(int m)
{
	if (m == tsLSB || m == tsUSB || m == tsAM)
// these modes have lo and hi settings. BUT MUST RETURN A VALID pointer
// NOT EMPTY!
		return TS870S_SSB_SH;  

	else if (m == tsCW || m == tsCWR)
		return TS870S_CWwidths;

	else if (m == tsFSK || m == tsFSKR)
		return TS870S_FSKwidths;

	else if (m == tsFM)
		return TS870S_FMwidths;

	else
		return TS870S_SSB_SH;
}

const char **RIG_TS870S::lotable(int m)
{
	if (m == tsLSB || m == tsUSB)
		return TS870S_SSB_SL;  // these modes have lo and hi settings.

	if (m == tsAM)
		return TS870S_AM_SL;

	return NULL;//TS870S_empty;
}

const char **RIG_TS870S::hitable(int m)
{
	if (m == tsLSB || m == tsUSB)
		return TS870S_SSB_SH;  // these modes have lo and hi settings.

	if (m == tsAM)
		return TS870S_AM_SH;

	return NULL;
}

//----------------------------------------------------------------------
int RIG_TS870S::adjust_bandwidth(int val)
{
	if (val == tsLSB || val == tsUSB )
		return DEF_SL_SH; // 2800Hz .. 300Hz

	else if (val == tsCW || val == tsCWR)
		return DEF_CW_BW; // 400Hz

	else if (val == tsFSK || val == tsFSKR)
		return DEF_FSK_BW; // 500Hz

	else if (val == tsFM)
		return DEF_FM_BW; // 10000Hz

//	else if (val == tsAM)
	return DEF_AM_SL_SH; // 6000Hz .. 100Hz
}

//----------------------------------------------------------------------
int RIG_TS870S::def_bandwidth(int val)
{
	return adjust_bandwidth(val);
}

//----------------------------------------------------------------------
void RIG_TS870S::set_bwA(int val)
{
	switch (B.imode) {
		case tsLSB:
		case tsUSB:  {
			if (val < 256)
				break;
			int lo = val & 0x7F;
			int hi = (val >> 8) & 0x7F;
			if (lo > 9 || hi > 11)
				break;
			A.iBW = val;

			cmd = TS870S_CAT_ssb_SL[lo];
			sendCommand(cmd);
			set_trace(2, "set lower ", cmd.c_str());
			showresp(WARN, ASC, "set lower", cmd, "");

			cmd = TS870S_CAT_ssb_SH[hi];
			sendCommand(cmd);
			set_trace(2, "set upper ", cmd.c_str());
			showresp(WARN, ASC, "set upper", cmd, "");
			break;
		}

		case tsAM:  {
			if (val < 256)
				break;
			int lo = val & 0x7F;
			int hi = (val >> 8) & 0x7F;
			if (lo > 3 || hi > 5)
				break;
			A.iBW = val;

			cmd = TS870S_CAT_am_SL[lo];
			sendCommand(cmd);
			set_trace(2, "set lower ", cmd.c_str());
			showresp(WARN, ASC, "set lower", cmd, "");

			cmd = TS870S_CAT_am_SH[hi];
			sendCommand(cmd);
			set_trace(2, "set upper ", cmd.c_str());
			showresp(WARN, ASC, "set upper", cmd, "");
			break;
		}

		case tsCW:
		case tsCWR: {
			if (val > 256) {
				break;
			}
			A.iBW = val;
			cmd = TS870S_CWbw[A.iBW];
			sendCommand(cmd);
			set_trace(2, "set CW bw ", cmd.c_str());
			showresp(WARN, ASC, "set CW bw", cmd, "");
			break;
		}

		case tsFSK:
		case tsFSKR:  {
			if (val > 256)
				break;
			A.iBW = val;
			cmd = TS870S_FSKbw[A.iBW];
			sendCommand(cmd);
			set_trace(2, "set FSK bw ", cmd.c_str());
			showresp(WARN, ASC, "set FSK bw", cmd, "");
			break;
		}

		case tsFM: {
			A.iBW = val;
			cmd = TS870S_FMbw[A.iBW];
			sendCommand(cmd);
			set_trace(2, "set FM bw ", cmd.c_str());
			showresp(WARN, ASC, "set FM bw", cmd, "");
			break;
		}
	}
}

//----------------------------------------------------------------------
void RIG_TS870S::set_bwB(int val)
{
	switch (B.imode) {
		case tsLSB:
		case tsUSB:  {
			if (val < 256)
				break;
			int lo = val & 0x7F;
			int hi = (val >> 8) & 0x7F;
			if (lo > 9 || hi > 11)
				break;
			B.iBW = val;

			cmd = TS870S_CAT_ssb_SL[lo];
			sendCommand(cmd);
			set_trace(2, "set lower ", cmd.c_str());
			showresp(WARN, ASC, "set lower", cmd, "");

			cmd = TS870S_CAT_ssb_SH[hi];
			sendCommand(cmd);
			set_trace(2, "set upper ", cmd.c_str());
			showresp(WARN, ASC, "set upper", cmd, "");

			break;
		}

		case tsAM:  {
			if (val < 256)
				break;
			int lo = val & 0x7F;
			int hi = (val >> 8) & 0x7F;
			if (lo > 3 || hi > 5)
				break;
			B.iBW = val;

			cmd = TS870S_CAT_am_SL[lo];
			sendCommand(cmd);
			set_trace(2, "set lower ", cmd.c_str());
			showresp(WARN, ASC, "set lower", cmd, "");

			cmd = TS870S_CAT_am_SH[hi];
			sendCommand(cmd);
			set_trace(2, "set upper ", cmd.c_str());
			showresp(WARN, ASC, "set upper", cmd, "");

			break;
		}

		case tsCW:
		case tsCWR: {
			if (val > 256)
				break;
			B.iBW = val;
			cmd = TS870S_CWbw[B.iBW];
			sendCommand(cmd);
			set_trace(2, "set CW bw ", cmd.c_str());
			showresp(WARN, ASC, "set CW bw", cmd, "");
			break;
		}

		case tsFSK:
		case tsFSKR:  {
			if (val > 256)
				break;
			B.iBW = val;
			cmd = TS870S_FSKbw[B.iBW];
			sendCommand(cmd);
			set_trace(2, "set FSK bw ", cmd.c_str());
			showresp(WARN, ASC, "set FSK bw", cmd, "");
			break;
		}

		case tsFM: {
			A.iBW = val;
			cmd = TS870S_FMbw[A.iBW];
			sendCommand(cmd);
			set_trace(2, "set FM bw ", cmd.c_str());
			showresp(WARN, ASC, "set FM bw", cmd, "");
			break;
		}
	}
}

//----------------------------------------------------------------------
int RIG_TS870S::get_bwA() {

	int i = 0;
	int ret;
	size_t p;

	switch (A.imode) {
		case tsFM: {
//			A.iBW = 1; // default value
			cmd = "FW;"; // 'width' query
			get_trace(1, "get_bwA()");
			ret = wait_char(';', 7, 100, "get CW width", ASC);
			gett("");
			if (ret < 7)
				return A.iBW;

			p = replystr.rfind("FW");
			if (p == std::string::npos)
				return A.iBW;

			for (i = 0; TS870S_FMbw[i] != NULL; i++)
				if (replystr.find(TS870S_FMbw[i]) == p) {
					A.iBW = i;
					break; 	// Found returned data, in std::string array.
				}
			break;
		}

		case tsCW:
		case tsCWR: {
//			A.iBW = 1; // default
			cmd = "FW;"; // 'width' query
			get_trace(1, "get_bwA()");
			ret = wait_char(';', 7, 100, "get CW width", ASC);
			gett("");
			if (ret < 7)
				return A.iBW;

			p = replystr.rfind("FW");
			if (p == std::string::npos)
				return A.iBW;
			for (i = 0; TS870S_CWbw[i] != NULL; i++)
				if (replystr.find(TS870S_CWbw[i]) == p) {
					A.iBW = i;
					break; 	// Found returned data, in std::string array.
				}
			break;
		}

		case tsFSK:
		case tsFSKR: {
//			A.iBW = 1; // default
			cmd = "FW;";
			get_trace(1, "get_bwA()");
			ret = wait_char(';', 7, 100, "get FSK width", ASC);
			gett("");
			if (ret < 7)
				return A.iBW;

			p = replystr.rfind("FW");
			if (p == std::string::npos)
				return A.iBW;

			for (i = 0; TS870S_FSKbw[i] != NULL; i++)
				if (replystr.find(TS870S_FSKbw[i]) == p) {
					A.iBW = i;
					break; 	// Found returned data, in std::string array.
				}
			break;
		}

		case tsAM:  {
//			A.iBW = DEF_AM_SL_SH;
			int lo = A.iBW & 0x7F, hi = (A.iBW >> 8) & 0x7F;
		// High byte is hi cut index (not MSB though.) Low byte is lo cuttoff index.

			cmd = "FW;"; // Read Low cuttoff. Returns a two digit code as 'FLxxxx;' in 10Hz increments.
			get_trace(1, "get_lower");
			ret = wait_char(';', 7, 100, "get lower", ASC);
			gett("");
			if (ret < 7)
				return A.iBW;

			p = replystr.rfind("FW");
			if (p == std::string::npos)
				return A.iBW;

			for (i = 0; TS870S_CAT_am_SL[i] != NULL; i++)
				if (replystr.find(TS870S_CAT_am_SL[i]) == p) {
					lo = i;
					break; 	// Found returned data, in std::string array.
				}

			cmd = "IS;";
			get_trace(1, "get_upper");
			ret = wait_char(';', 8, 100, "get upper", ASC);
			gett("");
			if (ret < 8)
				return A.iBW;

			p = replystr.rfind("IS ");
			if (p == std::string::npos)
				return A.iBW;

			for (i = 0; TS870S_CAT_am_SH[i] != NULL; i++)
				if (replystr.find(TS870S_CAT_am_SH[i]) == p) {
					hi = i;
					break; 	// Found returned data, in std::string array.
				}

			A.iBW = ((hi << 8) | (lo & 0x7F)) | 0x8000;
			break;
		}

		case tsLSB:
		case tsUSB: {
//			A.iBW = DEF_SL_SH;
			int lo = A.iBW & 0x7F, hi = (A.iBW >> 8) & 0x7F; // Same trick as above...

			cmd = "FW;"; // Read Low cuttoff. Returns a two digit code as 'FLxxxx;' in 10Hz increments.
			get_trace(1, "get lower");
			ret = wait_char(';', 7, 100, "get lower", ASC);
			gett("");
			if (ret < 7)
				return A.iBW;

			p = replystr.rfind("FW");
			if (p == std::string::npos)
				return A.iBW;

			for (i = 0; TS870S_CAT_ssb_SL[i] != NULL; i++)
				if (replystr.find(TS870S_CAT_ssb_SL[i]) == p) {
					lo = i;
					break; 	// Found returned data, in std::string array.
				}

			cmd = "IS;";
			get_trace(1, "get upper");
			ret = wait_char(';', 8, 100, "get upper", ASC);
			gett("");
			if (ret < 8)
				return A.iBW;

			p = replystr.rfind("IS ");

			if (p == std::string::npos)
				return A.iBW;

			for (i = 0; TS870S_CAT_ssb_SH[i] != NULL; i++)
				if (replystr.find(TS870S_CAT_ssb_SH[i]) == p) {
					hi = i;
					break; 	// Found returned data, in std::string array.
				}

			A.iBW = ((hi << 8) | (lo & 0x7F)) | 0x8000;

			break;
		}
	} // switch
	return A.iBW;
}

//----------------------------------------------------------------------
int RIG_TS870S::get_bwB() {

	int i = 0;
	int ret;
	size_t p;

	switch (A.imode) {
		case tsFM: {
//			B.iBW = 1; // default value
			cmd = "FW;"; // 'width' query
			get_trace(1, "get_bwA()");
			ret = wait_char(';', 7, 100, "get CW width", ASC);
			gett("");
			if (ret < 7)
				return B.iBW;

			p = replystr.rfind("FW");
			if (p == std::string::npos)
				return B.iBW;

			for (i = 0; TS870S_FMbw[i] != NULL; i++)
				if (replystr.find(TS870S_FMbw[i]) == p) {
					B.iBW = i;
					break; 	// Found returned data, in std::string array.
				}
			break;
		}

		case tsCW:
		case tsCWR: {
//			B.iBW = 1; // default
			cmd = "FW;"; // 'width' query
			get_trace(1, "get_bwA()");
			ret = wait_char(';', 7, 100, "get CW width", ASC);
			gett("");
			if (ret < 7)
				return B.iBW;

			p = replystr.rfind("FW");
			if (p == std::string::npos)
				return B.iBW;
			for (i = 0; TS870S_CWbw[i] != NULL; i++)
				if (replystr.find(TS870S_CWbw[i]) == p) {
					B.iBW = i;
					break; 	// Found returned data, in std::string array.
				}
			break;
		}

		case tsFSK:
		case tsFSKR: {
//			B.iBW = 1; // default
			cmd = "FW;";
			get_trace(1, "get_bwA()");
			ret = wait_char(';', 7, 100, "get FSK width", ASC);
			gett("");
			if (ret < 7)
				return B.iBW;

			p = replystr.rfind("FW");
			if (p == std::string::npos)
				return B.iBW;

			for (i = 0; TS870S_FSKbw[i] != NULL; i++)
				if (replystr.find(TS870S_FSKbw[i]) == p) {
					B.iBW = i;
					break; 	// Found returned data, in std::string array.
				}
			break;
		}

		case tsAM:  {
//			B.iBW = DEF_AM_SL_SH;
			int lo = B.iBW & 0x7F, hi = (B.iBW >> 8) & 0x7F;
		// High byte is hi cut index (not MSB though.) Low byte is lo cuttoff index.

			cmd = "FW;"; // Read Low cuttoff. Returns a two digit code as 'FLxxxx;' in 10Hz increments.
			get_trace(1, "get_lower");
			ret = wait_char(';', 7, 100, "get lower", ASC);
			gett("");
			if (ret < 7)
				return B.iBW;

			p = replystr.rfind("FW");
			if (p == std::string::npos)
				return B.iBW;

			for (i = 0; TS870S_CAT_am_SL[i] != NULL; i++)
				if (replystr.find(TS870S_CAT_am_SL[i]) == p) {
					lo = i;
					break; 	// Found returned data, in std::string array.
				}

			cmd = "IS;";
			get_trace(1, "get_upper");
			ret = wait_char(';', 8, 100, "get upper", ASC);
			gett("");
			if (ret < 8)
				return B.iBW;

			p = replystr.rfind("IS ");
			if (p == std::string::npos)
				return B.iBW;

			for (i = 0; TS870S_CAT_am_SH[i] != NULL; i++)
				if (replystr.find(TS870S_CAT_am_SH[i]) == p) {
					hi = i;
					break; 	// Found returned data, in std::string array.
				}

			B.iBW = ((hi << 8) | (lo & 0x7F)) | 0x8000;
			break;
		}

		case tsLSB:
		case tsUSB: {
//			B.iBW = DEF_SL_SH;
			int lo = B.iBW & 0x7F, hi = (B.iBW >> 8) & 0x7F; // Same trick as above...

			cmd = "FW;"; // Read Low cuttoff. Returns a two digit code as 'FLxxxx;' in 10Hz increments.
			get_trace(1, "get lower");
			ret = wait_char(';', 7, 100, "get lower", ASC);
			gett("");
			if (ret < 7)
				return B.iBW;

			p = replystr.rfind("FW");
			if (p == std::string::npos)
				return B.iBW;

			for (i = 0; TS870S_CAT_ssb_SL[i] != NULL; i++)
				if (replystr.find(TS870S_CAT_ssb_SL[i]) == p) {
					lo = i;
					break; 	// Found returned data, in std::string array.
				}

			cmd = "IS;";
			get_trace(1, "get upper");
			ret = wait_char(';', 8, 100, "get upper", ASC);
			gett("");
			if (ret < 8)
				return B.iBW;

			p = replystr.rfind("IS ");

			if (p == std::string::npos)
				return B.iBW;

			for (i = 0; TS870S_CAT_ssb_SH[i] != NULL; i++)
				if (replystr.find(TS870S_CAT_ssb_SH[i]) == p) {
					hi = i;
					break; 	// Found returned data, in std::string array.
				}

			B.iBW = ((hi << 8) | (lo & 0x7F)) | 0x8000;

			break;
		}
	} // switch
	return B.iBW;
}

//----------------------------------------------------------------------
int RIG_TS870S::get_modetype(int n)
{
	return TS870S_mode_type[n];
}

//----------------------------------------------------------------------
// val 0 .. 255
void RIG_TS870S::set_mic_gain(int val)
{
	int imic = (int)(val * 2.55); // 0 .. 255
	cmd = "MG000;";
	for (int i = 3; i > 0; i--) {
		cmd[1+i] += imic % 10;
		imic /= 10;
	}
 	sendCommand(cmd);
}

//----------------------------------------------------------------------
int RIG_TS870S::get_mic_gain()
{
	int val = 0;
	int ret;
	cmd = "MG;";
	get_trace(1,"get_mic_gain()");
	ret = wait_char(';', 6, 100, "get mic ctrl", ASC);
	gett("");
	if (ret < 6)
		return val;

	size_t p = replystr.rfind("MG");
	if (p == std::string::npos) 
		return val;

	replystr[p + 5] = 0;
	val = atoi(&replystr[p + 2]);

	return (int)(val / 2.55);  // we only want 0 .. 100
}

//----------------------------------------------------------------------
void RIG_TS870S::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

//----------------------------------------------------------------------
void RIG_TS870S::set_noise(bool val)
{
	if (val)
		cmd = "NB1;";
	else
		cmd = "NB0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set NB", cmd, "");
}

int  RIG_TS870S::get_noise()
{
	int ret;
	cmd = "NB;";
	get_trace(1,"get_noise()");
	ret = wait_char(';', 4, 100, "get NB", ASC);
	gett("");
	if (ret < 4)
		return 0;

	size_t p = replystr.rfind("NB");
	if (p == std::string::npos) return 0;

	return (replystr[p + 2] == '1'); // true if 1
}

//======================================================================
// IF shift command only available if the transceiver is in the CW mode
// step size is 50 Hz
//======================================================================
// See also, the code in support.cxx
//	setMode and setModeControl, calling 'checkTS870S'
//
void RIG_TS870S::set_if_shift(int val)
{
	if (active_mode == tsCW || active_mode == tsCWR) { // cw modes
		progStatus.shift_val = val;
		cmd = "IS ";
		cmd.append(to_decimal(abs(val),4)).append(";");
		sendCommand(cmd);
		showresp(WARN, ASC, "set IF shift", cmd, "");
	}
}

//----------------------------------------------------------------------
bool RIG_TS870S::get_if_shift(int &val)
{
	int ret;
	val = progStatus.shift_val;
	if (active_mode == tsCW || active_mode == tsCWR) { // cw modes
		cmd = "IS;";
		get_trace(1, "get_if_shift");
		ret = wait_char(';', 8, 100, "get IF shift", ASC);
		if (ret < 8)
			return false;

		size_t p = replystr.rfind("IS");
		if (p != std::string::npos) {
			val = fm_decimal(replystr.substr(p+3), 4);
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------
void RIG_TS870S::get_if_min_max_step(int &min, int &max, int &step)
{
	if_shift_min = min = 400;
	if_shift_max = max = 1000;
	if_shift_step = step = 50;
	if_shift_mid = 800;
}

//----------------------------------------------------------------------
// Dave, G0WBX, does this work?
// I do not see a BP in the 870 command table
//
void RIG_TS870S::set_notch(bool on, int val)
{
	cmd = "BP00000;";
	if (on == false) {
		sendCommand(cmd);
		notch_on = false;
		return;
	}
	if (!notch_on) {
		cmd[6] = '1'; // notch ON
		sendCommand(cmd);
		cmd[6] = '0';
		notch_on = true;
	}
	cmd[3] = '1'; // manual NOTCH position
// set notch value offset by 200, ie: 001 -> 400
	val = (-val / 9) + 200;
	if (val < 1) val = 1;
	if (val > 400) val = 400;
	for (int i = 3; i > 0; i--) {
		cmd[3 + i] += val % 10;
		val /=10;
	}
	sendCommand(cmd);
}

//----------------------------------------------------------------------
//tbd
bool  RIG_TS870S::get_notch(int &val)
{
	return false;
//	bool ison = false;
//	cmd = "BP;";
//	int ret = sendCommand(cmd);
//	return ison;
}

//----------------------------------------------------------------------
void RIG_TS870S::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = -1143;
	max = +1143;
	step = 9;
}

// revert to use KENWOOD generic get set for split operation
/*
//----------------------------------------------------------------------
// revision is similar to TS950S code
//----------------------------------------------------------------------
void RIG_TS870S::set_split(bool val)
{
// find out which VFO (or if Memory) is in use for RX.

	cmd = "FR;";   sendCommand(cmd);
	showresp(WARN, ASC, "RX-VFO?", cmd, "");
	if (wait_char(';', 4, 100, "get VFO for split", ASC) < 4) return;

	size_t p = replystr.rfind("FR");
// if bad data, change nothing.
	if (p == std::string::npos) return;

// '0' = VFO-A, '1' = VFO-B, '2' = Memory.
	switch (replystr[p+2]) {
		case '0' : inuse = onA; break;
		case '1' : inuse = onB; break;
		case '2' :
		default  : return;  // do nothing the xcvr is in memory mode
	}

	split = val;
	if (inuse == onB) {
		if (val) {
			cmd = "FR1;FT0;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B, Tx on A", cmd, "");
		} else {
			cmd = "FR1;FT1;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B, Tx on B", cmd, "");
		}
	} else {
		if (val) {
			cmd = "FR0;FT1;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A, Tx on B", cmd, "");
		} else {
			cmd = "FR0;FT0;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A, Tx on A", cmd, "");
		}
	}
	Fl::awake(highlight_vfo, (void *)0);
}

//----------------------------------------------------------------------
// This works for the 950S ... you can can revert to use your
//  IF byte 32 test if this does not work on the 870S
//----------------------------------------------------------------------
int RIG_TS870S::get_split()
{
	size_t p;
	int split = 0;
	char rx = 0, tx = 0;
// tx vfo
	cmd = rsp = "FT";
	cmd.append(";");
	if (wait_char(';', 4, 100, "get split tx vfo", ASC) == 4) {
		p = replystr.rfind(rsp);
		if (p == std::string::npos) return split;
		tx = replystr[p+2];
	}
// rx vfo
	cmd = rsp = "FR";
	cmd.append(";");
	if (wait_char(';', 4, 100, "get split rx vfo", ASC) == 4) {
		p = replystr.rfind(rsp);
		if (p == std::string::npos) return split;
		rx = replystr[p+2];
// split test
        if (tx != rx) split = 1; // change submitted by G8KBV
	}
	return split;
}
*/
