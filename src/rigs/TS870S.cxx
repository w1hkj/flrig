/*
 * Kenwood TS870S driver
 * Based on the TS590S driver by Dave Freese.
 * a part of flrig.
 *
 * Copyright 2009, Dave Freese, W1HKJ
 * Changes for the TS-870S March 2012, Dave Baxter, G0WBX
 */
 
 /* Todo:
 * Look at:-												(Example)
 * Autonotch control:- 	(SSB Only on the 870S)				(TS-2000)
 * Noise Reduction control:- 								(IC-7700)
 * Extra controls for speach compressor etc:- 				(IC-9100 (and others))
 * Misc controls auto rig on/off with flrig startup/quit:-	(FT-450/950) (PS0; PS1;)
 * Check Autotune action, and any 'busy' status.
 */

#include "config.h"
#include "TS870S.h"
#include "support.h"

static const char TS870Sname_[] = "TS-870S";

//	   mode array Index Values :-         0      1      2     3     4     5      6       7
static const char *TS870Smodes_[] = {   "LSB", "USB", "CW", "FM", "AM", "FSK", "CW-R", "FSK-R", NULL};
static const char TS870S_mode_chr[] =  { '1',   '2',   '3',  '4',  '5',  '6',   '7',    '9' };
static const char TS870S_mode_type[] = { 'L',   'U',   'U',  'U',  'U',  'L',   'L',    'U' };

//----------------------------------------------------------------------
static const char *TS870S_empty[] = { "N/A", NULL };

//----------------------------------------------------------------------
static const char *TS870S_SSB_lo[] = { 			// MD1; and MD2;
  "0",   "50", "100", "200", "300", 			// Available settings (Hz)
"400",  "500", "600", "800", "1000", NULL };

static const char *TS870S_CAT_ssb_lo[] = {		// Corresponding commands.
"FW0000;", "FW0005;", "FW0010;", "FW0020;", "FW0030;",
"FW0040;", "FW0050;", "FW0060;", "FW0080;", "FW0100;", NULL };

static const char *TS870S_SSB_lo_tooltip = "lo cutoff";
static const char *TS870S_SSB_btn_lo_label = "L";


static const char *TS870S_SSB_hi[] = { 			// MD1; and MD2;
"1400", "1600", "1800", "2000", "2200", "2400", // Available settings (Hz)
"2600", "2800", "3000", "3400", "4600", "6000", NULL };

static const char *TS870S_CAT_ssb_hi[] = {		// Corresponding commands.
"IS 1400;", "IS 1600;", "IS 1800;", "IS 2000;", "IS 2200;", "IS 2400;",
"IS 2600;", "IS 2800;", "IS 3000;", "IS 3400;", "IS 4600;", "IS 6000;", NULL };

static const char *TS870S_SSB_hi_tooltip = "hi cutoff";
static const char *TS870S_SSB_btn_hi_label = "H";

//----------------------------------------------------------------------
static const char *TS870S_AM_lo[] = { // Available values...
"0", "100", "200", "500", NULL };

static const char *TS870S_CAT_am_lo[] = { // ... Corresponding commands.
"FW0000;", "FW0010;", "FW0020;", "FW0050;", NULL};

static const char *TS870S_AM_lo_tooltip = "lo cutoff";
static const char *TS870S_AM_btn_lo_label = "L";

static const char *TS870S_AM_hi[] = {
"2500", "3000", "4000", "5000", "6000", "7000", NULL };

static const char *TS870S_CAT_am_hi[] = {
"IS 2500;", "IS 3000;", "IS 4000;", "IS 5000;", "IS 6000;", "IS 7000;", NULL};

static const char *TS870S_AM_hi_tooltip = "hi cutoff";
static const char *TS870S_AM_btn_hi_label = "H";

//----------------------------------------------------------------------
static const char *TS870S_CWwidths[] = { // Available CW bandwidths
"50", "100", "200", "400", "600", "1000", NULL};

static const char *TS870S_CWbw[] = { // Corresponding commands.
"FW0005;", "FW0010;", "FW0020;", "FW0040;", "FW0060;", "FW0100;", NULL};

//----------------------------------------------------------------------
static const char *TS870S_FSKwidths[] = { // Available FSK bandwidths
"250", "500", "1000", "1500", NULL};

static const char *TS870S_FSKbw[] = { // Corresponding commands.
"FW0025;", "FW0050;", "FW0100;", "FW0150;", NULL };

//----------------------------------------------------------------------
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
	cmd = "AC000;"; sendCommand(cmd);
	get_preamp();
	get_attenuator();
	RIG_DEBUG = true;
}

//----------------------------------------------------------------------
RIG_TS870S::RIG_TS870S() {

	name_ = TS870Sname_;
	modes_ = TS870Smodes_;
	bandwidths_ = TS870S_empty;

	dsp_lo     = TS870S_SSB_lo;
	lo_tooltip = TS870S_SSB_lo_tooltip;
	lo_label   = TS870S_SSB_btn_lo_label;

	dsp_hi     = TS870S_SSB_hi;
	hi_tooltip = TS870S_SSB_hi_tooltip;
	hi_label   = TS870S_SSB_btn_hi_label;

	widgets = rig_widgets;

	comm_baudrate = BR57600;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = false;  	// default to safe
	comm_rtsptt = false;	//	ditto (used for hardware handshake)
	comm_dtrptt = false;	//	ditto

//	Defaults.
	B.imode = A.imode = 1;		// USB
	B.iBW = A.iBW = 0x8704; 	// hi=2800Hz .. lo=300Hz 
	B.freq = A.freq = 14070000;
	can_change_alt_vfo = true;

	nb_level = 2;

	has_micgain_control =
	has_notch_control =
	has_ifshift_control =
	has_swr_control = false;

	has_dsp_controls =
	has_smeter =
	has_power_out =
	has_split =
	has_noise_control =
	has_micgain_control =
	has_volume_control =	// see 'read_volume()' in support.cxx
	has_power_control = true;
	
	has_tune_control = false; // disabled for now
	
	has_attenuator_control = true;
	
	has_preamp_control = false;
	
	has_mode_control =
	has_bandwidth_control = 
	
	has_ifshift_control =    // See 'update_ifshift' in support.cxx
	
	has_ptt_control = true;
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
			(md == 0 || md == 1) ? TS870S_SSB_lo[lo] :	// SSB lo
			(md == 4) ? TS870S_AM_lo[lo] : "N/A",		//  AM lo
			(md == 0 || md == 1) ? TS870S_SSB_hi[hi] :	// SSB hi
			(md == 4) ? TS870S_AM_hi[hi] : "N/A" );		//  AM hi
			
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
	showresp(WARN, ASC, "Rx/Tx A", cmd, replystr);
}

//----------------------------------------------------------------------
void RIG_TS870S::selectB()
{
	cmd = "FR1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx/Tx B", cmd, replystr);
}

//----------------------------------------------------------------------
void RIG_TS870S::set_split(bool val)
{
	split = val;
	if (val) {
		cmd = "FT1;";
		sendCommand(cmd);
		showresp(WARN, ASC, "Rx-A Tx-B", cmd, replystr);
	} else {
		cmd = "FR0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "Rx/Tx A", cmd, replystr);
	}
}

//----------------------------------------------------------------------
int RIG_TS870S::get_split()
{
	cmd = "IF;";
	int ret = waitN(38, 100, "get info", ASC);
	if (ret < 38) return split;
	
	size_t p = replystr.rfind("IF");
	if (p == string::npos) return split;
	
	split = replystr[p+32] ? true : false;
	return split;
}

//----------------------------------------------------------------------
long RIG_TS870S::get_vfoA ()
{
	cmd = "FA;";
	int ret = waitN(14, 100, "get vfoA", ASC);

	if (ret < 14) return A.freq;
	size_t p = replystr.rfind("FA");
	if (p == string::npos) return A.freq;

	long f = 0L;
	long mul = 1L;
	for (size_t n = 12; n > 1; n--) {
		f += (replystr[p + n] - '0') * mul;
		mul *= 10;
	}
	A.freq = f;
	return A.freq;
}

//----------------------------------------------------------------------
void RIG_TS870S::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set vfo A", cmd, replystr);
}

//----------------------------------------------------------------------
long RIG_TS870S::get_vfoB ()
{
	cmd = "FB;";
	int ret = waitN(14, 100, "get vfoB", ASC);

	if (ret < 14) return B.freq;
	size_t p = replystr.rfind("FB");
	if (p == string::npos) return B.freq;

	long f = 0L;
	long mul = 1L;
	for (size_t n = 12; n > 1; n--) {
		f += (replystr[p + n] - '0') * mul;
		mul *= 10;
	}
	B.freq = f;

	return B.freq;
}

//----------------------------------------------------------------------
void RIG_TS870S::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set vfo B", cmd, replystr);
}

//----------------------------------------------------------------------
/* int RIG_TS870S::get_smeter() {
	int mtr = 0;
	cmd = "SM;";
	int ret = waitN(8, 100, "get smeter", ASC);

	if (ret < 7) return 0;
	size_t p = replystr.find("SM");
	if (p == string::npos) return 0;

	replystr[p + 6] = 0;
	mtr = atoi(&replystr[p + 2]);
	mtr *= 50;
	mtr /= 15;
	if (mtr > 100) mtr = 100;
	return mtr;
} */

//----------------------------------------------------------------------
int RIG_TS870S::get_smeter() {	 // scales correctly now 2012-02-24
	cmd = "SM;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "S meter", cmd, replystr);
	if (ret < 7) return 0;
	size_t p = replystr.rfind("SM");
	if (p == string::npos) return -1;

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
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "Output Power", cmd, replystr);
	if (ret < 7) return mtr;
	size_t p = replystr.rfind("SM");
	if (p == string::npos) return mtr;

	replystr[p + 6] = 0;
	mtr = atoi(&replystr[p + 2]);
	mtr = (mtr * 100) / 30;										//wbx
	if (mtr > 100) mtr = 100; // but with a power limit!
	return mtr;
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
	sendCommand(cmd, 0); // 010 .. 100
}

//----------------------------------------------------------------------
int RIG_TS870S::get_power_control()
{
	cmd = "PC;";
	int ret = waitN(6, 100, "get pwr ctrl", ASC);

	if (ret < 6) return 0;
	size_t p = replystr.rfind("PC");
	if (p == string::npos) return 0;
	int mtr = 0;
	if (ret >= 6) {
		replystr[p + 5] = 0;
		mtr = atoi(&replystr[p + 2]);
	}
	return mtr;
}

//----------------------------------------------------------------------
// Volume control return (rig sends back 0 .. 255)
int RIG_TS870S::get_volume_control()
{
	cmd = "AG;";  // reply data is 'AGnnn;' nnn = 000 .. 255  wbx

	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get vol ctrl", cmd, replystr);
	
//	int ret = waitN(7, 100, "get vol ctrl", ASC);

	if (ret < 6) return 0;
	size_t p = replystr.rfind("AG");
	if (p == string::npos) return 0;

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
	sendCommand(cmd, 0);
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
 */
 
void RIG_TS870S::set_PTT_control(int val)
{
	if (val) cmd = "TX;";
	else	 cmd = "RX;";
	sendCommand(cmd, 0);
}

//----------------------------------------------------------------------
// Play with the autotuner		// disabled for now wbx
void RIG_TS870S::tune_rig()
{
//	cmd = "AC111;";
//	sendCommand(cmd, 0);
	
	// this might take a while!
}

//----------------------------------------------------------------------
// the TS-870 actually has 4 attenuator settings.
// RA00; = Off,  RA01; = 6dB,  RA02; = 12dB,  RA03; = 18dB      wbx
void RIG_TS870S::set_attenuator(int val) {

	if (att_level == 0) {			// If attenuator level = 0 (off)
		att_level = 1;				// then turn it on, at 6dB
		cmd = "RA01;";				// this is the command...
		atten_label("Att 6", true);	// show it in the button...
	} 
	else if (att_level == 1) {		// If attenuator level = 1 (6dB)
		att_level = 2;				// then make it 12dB
		cmd = "RA02;";
		atten_label("Att 12", true);
	} 
	else if (att_level == 2) {		// if it's 12dB
		att_level = 3;				// go to 18dB
		cmd = "RA03;";
		atten_label("Att 18", true);
	} 
	else if (att_level == 3) {		// If it's 18dB
		att_level = 0;				// Loop back to none.
		cmd = "RA00;";
		atten_label("Att", false);
	}
	
	sendCommand(cmd);
	showresp(WARN, ASC, "set Att", cmd, replystr);	
}

//----------------------------------------------------------------------
//	Modified to read and show the actual radio setting, in the button.
int RIG_TS870S::get_attenuator() {
	cmd = "RA;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get Att", cmd, replystr);
	if (ret < 5) return att_on;
	size_t p = replystr.rfind("RA");
	if (p == string::npos) return att_on;

	if (replystr[p + 2] == '0' && replystr[p + 3] == '0') {
		att_on = 0;						// Attenuator is OFF
		att_level = 0;					// remember it...
		atten_label("Att", false);		// show it...
	}
	else if (replystr[p + 2] == '0' && replystr[p + 3] == '1') {
		att_on = 1;						// Attenuator is ON, 6dB
		att_level = 1;					// remember the level
		atten_label("Att 6", true);		// show it...
	}
	else if (replystr[p + 2] == '0' && replystr[p + 3] == '2') {
		att_on = 1;						// .. still ON, 12dB
		att_level = 2;					// remember this level
		atten_label("Att 12", true);	// show it.	
	}
	else if (replystr[p + 2] == '0' && replystr[p + 3] == '3') {
		att_on = 1;						// .. still ON 18dB
		att_level = 3;					// remember...
		atten_label("Att 18", true);	// show this too..	
	}
		
	return att_on;			// let the rest of th world know.
}

//----------------------------------------------------------------------
bool RIG_TS870S::get_TS870Sid() {
	cmd = "ID;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "Id", cmd, replystr);
	if (ret < 6) return false;
	size_t p = replystr.rfind("ID");			// String "ID015;"
	if (p == string::npos) return false;		// Bytes   012345
	if (replystr[p + 3] == '1' && 
		replystr[p + 4] == '5')  return true;	//wbx
	return false;
}

//======================================================================
// mode commands
//======================================================================
void RIG_TS870S::set_modeA(int val)
{
	active_mode = A.imode = val;
	cmd = "MD";
	cmd += TS870S_mode_chr[val];
	cmd += ';';
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set mode A", cmd, replystr);

	set_widths(val);
}

//----------------------------------------------------------------------
int RIG_TS870S::get_modeA()
{
	int md = A.imode;
	cmd = "MD;";
	int ret = waitN(4, 100, "get mode A", ASC);

	if (ret < 4) return A.imode;
	size_t p = replystr.rfind("MD");
	if (p == string::npos) return A.imode;

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
		set_widths(md);
	}
	return A.imode;
}

//----------------------------------------------------------------------
void RIG_TS870S::set_modeB(int val)
{
	active_mode = B.imode = val;
	cmd = "MD";
	cmd += TS870S_mode_chr[val];
	cmd += ';';
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set mode B", cmd, replystr);

	set_widths(val);
}

//----------------------------------------------------------------------
int RIG_TS870S::get_modeB()
{
	int md = B.imode;
	cmd = "MD;";
	int ret = waitN(4, 100, "get mode B", ASC);

	if (ret < 4) return B.imode;
	size_t p = replystr.rfind("MD");
	if (p == string::npos) return B.imode;

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
		set_widths(md);
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

	if (val == tsLSB || val == tsUSB ) {  // SSB modes
		bandwidths_ = TS870S_empty;
		dsp_lo = TS870S_SSB_lo;
		dsp_hi = TS870S_SSB_hi;
		lo_tooltip = TS870S_SSB_lo_tooltip;
		lo_label   = TS870S_SSB_btn_lo_label;
		hi_tooltip = TS870S_SSB_hi_tooltip;
		hi_label   = TS870S_SSB_btn_hi_label;
		bw = 0x8704; // 300 ... 2800 Hz
	}

	else if (val == tsCW || val == tsCWR) {  // CW modes
		bandwidths_ = TS870S_CWwidths;
		dsp_lo = TS870S_empty;
		dsp_hi = TS870S_empty;
		bw = 2; // 200Hz
	}

	else if (val == tsFSK || val == tsFSKR) {  // FSK modes
		bandwidths_ = TS870S_FSKwidths;
		dsp_lo = TS870S_empty;
		dsp_hi = TS870S_empty;
		bw = 1; // 500Hz
	}

	else if (val == tsAM) { // val == 5 ==> AM mode
		bandwidths_ = TS870S_empty;
		dsp_lo = TS870S_AM_lo;
		dsp_hi = TS870S_AM_hi;
		lo_tooltip = TS870S_AM_lo_tooltip;
		lo_label   = TS870S_AM_btn_lo_label;
		hi_tooltip = TS870S_AM_hi_tooltip;
		hi_label   = TS870S_AM_btn_hi_label;
		bw = 0x8401; // 100Hz .. 6000Hz
	}

// FM mode BW selection.
	else if (val == tsFM) {  // FM mode
		bandwidths_ = TS870S_FMwidths;  // load the dropdown with our list
		dsp_lo = TS870S_empty;
		dsp_hi = TS870S_empty;
		bw = 4; // 10000Hz
	}

	return bw;
}

//----------------------------------------------------------------------
const char **RIG_TS870S::bwtable(int m)
{
	if (m == tsLSB || m == tsUSB || m == tsAM)
		return TS870S_empty;  // these modes have lo and hi settings.

	else if (m == tsCW || m == tsCWR)
		return TS870S_CWwidths;

	else if (m == tsFSK || m == tsFSKR)
		return TS870S_FSKwidths;

	else if (m == tsFM)
		return TS870S_FMwidths;

	else
		return TS870S_empty;
}

//----------------------------------------------------------------------
int RIG_TS870S::adjust_bandwidth(int val)
{
	if (val == tsLSB || val == tsUSB )
		return 0x8704; // 2800Hz .. 300Hz

	else if (val == tsCW || val == tsCWR)
		return 2; // 200Hz

	else if (val == tsFSK || val == tsFSKR)
		return 1; // 500Hz

	else if (val == tsFM)
		return 3; // 10000Hz

//	else if (val == tsAM)
		return 0x8401; // 6000Hz .. 100Hz
}

//----------------------------------------------------------------------
int RIG_TS870S::def_bandwidth(int val)
{
	return adjust_bandwidth(val);
}

//----------------------------------------------------------------------
void RIG_TS870S::set_bwA(int val)
{
// LSB, USB
	if (A.imode == tsLSB || A.imode == tsUSB ) {
		if (val < 256) return;  // not hi/lo setting
		A.iBW = val;

		cmd = TS870S_CAT_ssb_lo[A.iBW & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, replystr);

		cmd = TS870S_CAT_ssb_hi[(A.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, replystr);

		return;
	}

// AM
	if (A.imode == tsAM) {
		if (val < 256) return; // not hi/lo setting
		A.iBW = val;

		cmd = TS870S_CAT_am_lo[A.iBW & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, replystr);

		cmd = TS870S_CAT_am_hi[(A.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, replystr);

		return;
	}
	if (val > 256) return;

// CW, CW-R
	if (A.imode == tsCW || A.imode == tsCWR) {
		A.iBW = val;
		cmd = TS870S_CWbw[A.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set CW bw", cmd, replystr);
		return;
	}

// FSK, FSK-R
	if (A.imode == tsFSK || A.imode == tsFSKR) {
		A.iBW = val;
		cmd = TS870S_FSKbw[A.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set FSK bw", cmd, replystr);
		return;
	}

// FM
	if (A.imode == tsFM) {
		A.iBW = val;
		cmd = TS870S_FMbw[A.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set FM bw", cmd, replystr);
		return;
	}
}

//----------------------------------------------------------------------
void RIG_TS870S::set_bwB(int val)
{
	if (B.imode == tsLSB || B.imode == tsUSB ) {
		if (val < 256) return;
		B.iBW = val;

		cmd = TS870S_CAT_ssb_lo[B.iBW & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, replystr);

		cmd = TS870S_CAT_ssb_hi[(B.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, replystr);

		return;
	}

	if (B.imode == tsAM) {
		if (val < 256) return;
		B.iBW = val;

		cmd = TS870S_AM_lo[B.iBW & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, replystr);

		cmd = TS870S_AM_hi[(B.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, replystr);

		return;
	}

	if (val > 256) return;

	if (B.imode == tsCW || B.imode == tsCWR) {
		B.iBW = val;
		cmd = TS870S_CWbw[B.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set CW bw", cmd, replystr);
		return;
	}

	if (B.imode == tsFSK || B.imode == tsFSKR) {
		B.iBW = val;
		cmd = TS870S_FSKbw[B.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set FSK bw", cmd, replystr);
		return;
	}

	if (A.imode == tsFM) {
		A.iBW = val;
		cmd = TS870S_FMbw[A.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set FM bw", cmd, replystr);
		return;
	}

}

//----------------------------------------------------------------------
int RIG_TS870S::get_bwA() {

	int i = 0;
	size_t p;

	if (A.imode == tsFM) { // FM mode.
		cmd = "FW;"; // 'Filter Width' query
		waitN(7, 100, "get CW width", ASC);
		p = replystr.rfind("FW");

		if (p != string::npos) { // If 'FW' found then scan the known responces to find out what we got.
			for (i = 0; TS870S_FMbw[i] != NULL; i++) // bump array index counter, till string match or end.
				if (replystr.find(TS870S_FMbw[i]) == p) break; 	// Found returned data, in string array.

			if (TS870S_FMbw[i] != NULL) A.iBW = i; // if we didn't hit the end, return the array index value.
			else A.iBW = 1; // Default.
		}
	}

	else if (A.imode == tsCW || A.imode == tsCWR) { // CW modes.
		cmd = "FW;"; // 'Filter Width' query
		waitN(7, 100, "get CW width", ASC);
		p = replystr.rfind("FW");

		if (p != string::npos) { // If 'FW' found then scan the known responces to find out what we got.
			for (i = 0; TS870S_CWbw[i] != NULL; i++) // bump array index counter, till string match or end.
				if (replystr.find(TS870S_CWbw[i]) == p) break; 	// Found returned data, in string array.

			if (TS870S_CWbw[i] != NULL) A.iBW = i; // if we didn't hit the end, return the array index value.
			else A.iBW = 1; // Default.
		}
	}

	else if (A.imode == tsFSK || A.imode == tsFSKR ) { // FSK modes.
		cmd = "FW;";
		waitN(7, 100, "get FSK width", ASC);
		p = replystr.rfind("FW");

		if (p != string::npos) { // If 'FW' found then scan the known responces to find out what we got.
			for (i = 0; TS870S_FSKbw[i] != NULL; i++) // bump array index counter, till string match or end.
				if (replystr.find(TS870S_FSKbw[i]) == p) break; 	// Found returned data, in string array.

			if (TS870S_FSKbw[i] != NULL) A.iBW = i; // if we didn't hit the end, return the array index value.
			else A.iBW = 1; // Default.
		}
	}


	else if (A.imode == tsAM) { // AM mode only

		int lo = A.iBW & 0x7F, hi = (A.iBW >> 8) & 0x7F;
		// High byte is hi cutoff index (not MSB though.) Low byte is lo cuttoff index.

		cmd = "FW;"; // Read Low cuttoff. Returns a two digit code as 'FLxxxx;' in 10Hz increments.
		waitN(5, 100, "get lower", ASC);
		p = replystr.rfind("FW");

		if (p != string::npos) { // If 'FW' found then scan the known responces to find out what we got.
			for (i = 0; TS870S_CAT_am_lo[i] != NULL; i++) // bump array index counter, till string match or end.
				if (replystr.find(TS870S_CAT_am_lo[i]) == p) break; 	// Found returned data, in string array.

			if (TS870S_CAT_am_lo[i] != NULL) lo = i; // if we didn't hit the end, return the array index value.
			else lo = 1; // Default.
		}

		cmd = "IS;";
		waitN(5, 100, "get upper", ASC);
		p = replystr.rfind("IS ");

		if (p != string::npos) {
			for (i = 0; TS870S_CAT_am_hi[i] != NULL; i++) // bump array index counter, till string match or end.
				if (replystr.find(TS870S_CAT_am_hi[i]) == p) break; 	// Found returned data, in string array.

			if (TS870S_CAT_am_hi[i] != NULL) hi = i; // if we didn't hit the end, return the array index value.
			else hi = 1; // Default.
		}
		A.iBW = ((hi << 8) | (lo & 0x7F)) | 0x8000;
		// Shift hi index up 8 bits into High Byte, then concatntat it with the Low byte, into one 16 bit value.
	}


	else if (A.imode == tsLSB || A.imode == tsUSB ) { // SSB (upper and lower)

		int lo = A.iBW & 0x7F, hi = (A.iBW >> 8) & 0x7F; // Same trick as above...

		cmd = "FW;"; // Read Low cuttoff. Returns a two digit code as 'FLxxxx;' in 10Hz increments.
		waitN(5, 100, "get lower", ASC);
		p = replystr.rfind("FW");

		if (p != string::npos) { // If 'FW' found then scan the known responces to find out what we got.
			for (i = 0; TS870S_CAT_ssb_lo[i] != NULL; i++) // bump array index counter, till string match or end.
				if (replystr.find(TS870S_CAT_ssb_lo[i]) == p) break; 	// Found returned data, in string array.

			if (TS870S_CAT_ssb_lo[i] != NULL) lo = i; // if we didn't hit the end, return the array index value.
			else lo = 1; // Default.
		}

		cmd = "IS;";
		waitN(5, 100, "get upper", ASC);
		p = replystr.rfind("IS ");

		if (p != string::npos) {
			for (i = 0; TS870S_CAT_ssb_hi[i] != NULL; i++) // bump array index counter, till string match or end.
				if (replystr.find(TS870S_CAT_ssb_hi[i]) == p) break; 	// Found returned data, in string array.

			if (TS870S_CAT_ssb_hi[i] != NULL) hi = i; // if we didn't hit the end, return the array index value.
			else hi = 1; // Default.
		}
		A.iBW = ((hi << 8) | (lo & 0x7F)) | 0x8000;
	}
	return A.iBW;
}

//----------------------------------------------------------------------
int RIG_TS870S::get_bwB()
{
	int i = 0;
	size_t p;

	if (B.imode == tsFM) {
		cmd = "FW;"; // 'Filter Width' query
		waitN(7, 100, "get CW width", ASC);
		p = replystr.rfind("FW");

		if (p != string::npos) { // If 'FW' found then scan the known responces to find out what we got.
			for (i = 0; TS870S_FMbw[i] != NULL; i++) // bump array index counter, till string match or end.
				if (replystr.find(TS870S_FMbw[i]) == p) break; 	// Found returned data, in string array.

			if (TS870S_FMbw[i] != NULL) B.iBW = i; // if we didn't hit the end, return the array index value.
			else B.iBW = 1; // Default.
		}
	}

	else if (B.imode == tsCW || B.imode == tsCWR) { // CW modes.
		cmd = "FW;"; // 'Filter Width' query
		waitN(7, 100, "get CW width", ASC);
		p = replystr.rfind("FW");

		if (p != string::npos) { // If 'FW' found then scan the known responces to find out what we got.
			for (i = 0; TS870S_CWbw[i] != NULL; i++) // bump array index counter, till string match or end.
				if (replystr.find(TS870S_CWbw[i]) == p) break; 	// Found returned data, in string array.

			if (TS870S_CWbw[i] != NULL) B.iBW = i; // if we didn't hit the end, return the array index value.
			else B.iBW = 1; // Default.
		}
	}

	else if (B.imode == tsFSK || B.imode == tsFSKR) { // FSK modes.
		cmd = "FW;";
		waitN(7, 100, "get FSK width", ASC);
		p = replystr.rfind("FW");

		if (p != string::npos) { // If 'FW' found then scan the known responces to find out what we got.
			for (i = 0; TS870S_FSKbw[i] != NULL; i++) // bumb array index counter, till string match or end.
				if (replystr.find(TS870S_FSKbw[i]) == p) break; 	// Found returned data, in string array.

			if (TS870S_FSKbw[i] != NULL) B.iBW = i; // if we didn't hit the end, return the array index value.
			else B.iBW = 1; // Default.
		}
	}

	else if (B.imode == tsAM) { // AM mode only

		int lo = B.iBW & 0x7F, hi = (B.iBW >> 8) & 0x7F;

		cmd = "FW;"; // Read Low cuttoff. Returns a two digit code as 'FLxxxx;' in 10Hz increments.
		waitN(5, 100, "get lower", ASC);
		p = replystr.rfind("FW");

		if (p != string::npos) { // If 'FW' found then scan the known responces to find out what we got.
			for (i = 0; TS870S_CAT_am_lo[i] != NULL; i++) // bump array index counter, till string match or end.
				if (replystr.find(TS870S_CAT_am_lo[i]) == p) break; 	// Found returned data, in string array.

			if (TS870S_CAT_am_lo[i] != NULL) lo = i; // if we didn't hit the end, return the array index value.
			else lo = 1; // Default.
		}

		cmd = "IS;";
		waitN(5, 100, "get upper", ASC);
		p = replystr.rfind("IS ");

		if (p != string::npos) {
			for (i = 0; TS870S_CAT_am_hi[i] != NULL; i++) // bump array index counter, till string match or end.
				if (replystr.find(TS870S_CAT_am_hi[i]) == p) break; 	// Found returned data, in string array.

			if (TS870S_CAT_am_hi[i] != NULL) hi = i; // if we didn't hit the end, return the array index value.
			else hi = 1; // Default.
		}
		B.iBW = ((hi << 8) | (lo & 0x7F)) | 0x8000;
	}


	else if (B.imode == tsLSB || B.imode == tsUSB ) { // SSB (Upper and lower)

		int lo = B.iBW & 0x7F, hi = (B.iBW >> 8) & 0x7F;

		cmd = "FW;"; // Read Low cuttoff. Returns a two digit code as 'FLxxxx;' in 10Hz increments.
		waitN(5, 100, "get lower", ASC);
		p = replystr.rfind("FW");

		if (p != string::npos) { // If 'FW' found then scan the known responces to find out what we got.
			for (i = 0; TS870S_CAT_ssb_lo[i] != NULL; i++) // bump array index counter, till string match or end.
				if (replystr.find(TS870S_CAT_ssb_lo[i]) == p) break; 	// Found returned data, in string array.

			if (TS870S_CAT_ssb_lo[i] != NULL) lo = i; // if we didn't hit the end, return the array index value.
			else lo = 1; // Default.
		}

		cmd = "IS;";
		waitN(5, 100, "get upper", ASC);
		p = replystr.rfind("IS ");

		if (p != string::npos) {
			for (i = 0; TS870S_CAT_ssb_hi[i] != NULL; i++) // bump array index counter, till string match or end.
				if (replystr.find(TS870S_CAT_ssb_hi[i]) == p) break; 	// Found returned data, in string array.

			if (TS870S_CAT_ssb_hi[i] != NULL) hi = i; // if we didn't hit the end, return the array index value.
			else hi = 1; // Default.
		}
		B.iBW = ((hi << 8) | (lo & 0x7F)) | 0x8000;
	}
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
 	sendCommand(cmd, 0);
}

//----------------------------------------------------------------------
int RIG_TS870S::get_mic_gain()
{
	int val = 0;
	cmd = "MG;";
	int ret = waitN(6, 100, "get mic ctrl", ASC);

	if (ret >= 6) {
		size_t p = replystr.rfind("MG");
		if (p == string::npos) return val;
		replystr[p + 5] = 0;
		val = atoi(&replystr[p + 2]);
	}
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
	showresp(WARN, ASC, "set NB", cmd, replystr);
}

int  RIG_TS870S::get_noise()
{
	cmd = "NB;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get NB", cmd, replystr);
	if (ret < 4) return 0;
	size_t p = replystr.rfind("NB");
	if (p == string::npos) return 0;

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
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set IF shift", cmd, replystr);
	}
}

//----------------------------------------------------------------------
bool RIG_TS870S::get_if_shift(int &val)
{
	if (active_mode == tsCW || active_mode == tsCWR) { // cw modes
		cmd = "IS;";
		waitN(8, 100, "get IF shift", ASC);
		size_t p = replystr.rfind("IS");
		if (p != string::npos) {
			val = fm_decimal(&replystr[p+3], 4);
		} else
			val = progStatus.shift_val;
		return true;
	}
	val = progStatus.shift_val;
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
void RIG_TS870S::set_notch(bool on, int val)
{
	cmd = "BP00000;";
	if (on == false) {
		sendCommand(cmd, 0);
		notch_on = false;
		return;
	}
	if (!notch_on) {
		cmd[6] = '1'; // notch ON
		sendCommand(cmd, 0);
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
	sendCommand(cmd, 0);
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

//----------------------------------------------------------------------
//	The End.
//----------------------------------------------------------------------
