/*
 * Kenwood TS870 driver
 * originally based on Kenwood TS570 driver
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 * changes,  2012, Dave Baxter, G0WBX
 *
 */


#include "TS870.h"
#include "support.h"

static const char TS870name_[] = "TS-870";

static const char *TS870modes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "FSK", "CW-R", "FSK-R", NULL};
static const char TS870_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '9' };
static const char TS870_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'U' };

static const char *TS870_SSBwidths[] = {
"NARR", "WIDE", NULL};

static const char *TS870_SSBbw[] = {
"FW0180;", "FW0300;", NULL};


// CW filter settings almost working, 27-02-2012	wbx
static const char *TS870_CWwidths[] = {
"50", "100", "200", "400", "600", "1000", NULL};			

static const char *TS870_CWbw[] = {						
"FW0005;", "FW0010;", "FW0020;",
"FW0040;", "FW0060;", "FW0100;", NULL};


// FSK widths working, 27-02-2102	wbx
static const char *TS870_FSKwidths[] = {
"250", "500", "1000", "1500", NULL};

static const char *TS870_FSKbw[] = {
  "FW0025;", "FW0050;", "FW0100;", "FW0150;", NULL};


//				   widget      Xpos, Ypos, Width    Small slider view.
static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 125, 156 },
	{ (Fl_Widget *)sldrPOWER,    54, 145, 368 },
//	{ (Fl_Widget *)btnNotch,	  2, 165,  50 },	// wbx
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

void RIG_TS870::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = btnIFsh;
	rig_widgets[3].W = sldrIFSHIFT;
	rig_widgets[4].W = sldrMICGAIN;
	rig_widgets[5].W = sldrPOWER;
//	rig_widgets[6].W = btnNotch;
	
	cmd = "FR0;"; sendCommand(cmd);
	showresp(WARN, ASC, "Rx on A", cmd, replystr);
	cmd = "AC001;"; sendCommand(cmd);
	showresp(WARN, ASC, "Thru - tune ON", cmd, replystr);
	get_preamp();
	get_attenuator();
	is_TS870S = get_TS870id();
}

RIG_TS870::RIG_TS870() {
// base class values
	name_ = TS870name_;
	modes_ = TS870modes_;
	bandwidths_ = TS870_SSBwidths;

	widgets = rig_widgets;

	comm_baudrate = BR57600;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = false;
	comm_rtsptt = false;
	comm_dtrptt = true;

	A.freq = 14070000;
	A.imode = 1;
	A.iBW = 1;
	B.freq = 7035000;
	B.imode = 1;
	B.iBW = 1;
	
	can_change_alt_vfo = true;

	has_notch_control = false;

	has_smeter =
	has_power_out =
	has_split =
	has_swr_control =
	has_auto_notch =
	has_ifshift_control =
	has_noise_control =
	has_micgain_control =
	has_volume_control =
	has_power_control =
	has_tune_control =
	has_attenuator_control = true;							// wbx
	
	has_preamp_control = false;								// wbx
	
	has_mode_control =
	has_bandwidth_control =
	has_ptt_control = true;
}

bool RIG_TS870::get_TS870id()
{
	cmd = "ID;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "Id", cmd, replystr);
	if (ret < 6) return false;
	size_t p = replystr.rfind("ID");						// "ID015;"
	if (p == string::npos) return false;					//  01234
	if (replystr[p + 3] == '1' && 
		replystr[p + 4] == '5')  return true;					//wbx
	return false;
}

void RIG_TS870::selectA()
{
	cmd = "FR0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select A", cmd, replystr);
}

void RIG_TS870::selectB()
{
	cmd = "FR1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select B", cmd, replystr);
}


//----------------------------------------------------------------------
void RIG_TS870::set_split(bool val) 
{
	split = val;
	if (val)
		cmd = "FT1;";
	else
		cmd = "FR0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "split", cmd, replystr);
}

//	IF00014070000.....+000000.203000004.;   (. = unused character)
//	                               ^		  1=Split 0=NoSplit
int RIG_TS870::get_split() {
	cmd = "IF;";
	int ret = sendCommand(cmd);
	showresp(INFO, ASC, "get info", cmd, replystr);
	if (ret < 38) return split;
	size_t p = replystr.rfind("IF");
	if (p == string::npos) return split;
	split = replystr[p+32] ? true : false;
	return split;
}


//----------------------------------------------------------------------
long RIG_TS870::get_vfoA () { 
	cmd = "FA;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get vfoA", cmd, replystr);
	if (ret < 14) return A.freq;
	size_t p = replystr.rfind("FA");
	if (p == string::npos) return A.freq;
	
	int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	A.freq = f;
	return A.freq;
}

void RIG_TS870::set_vfoA (long freq) {
	A.freq = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfoA", cmd, replystr);
}


//----------------------------------------------------------------------
long RIG_TS870::get_vfoB () {
	cmd = "FB;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get vfoB", cmd, replystr);
	if (ret < 14) return freqB;
	size_t p = replystr.rfind("FB");
	if (p == string::npos) return freqB;
	
	int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqB = f;
	return freqB;
}

void RIG_TS870::set_vfoB (long freq)
{
	freqB = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfoB", cmd, replystr);
}


//----------------------------------------------------------------------
// SM cmd 0 ... 100 (rig values 0 ... 30)						//wbx
int RIG_TS870::get_smeter()			// scales correctly now 2012-02-24
{
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
// RM cmd 0 ... 100 (rig values 0 ... 8)
int RIG_TS870::get_swr()
{
	cmd = "RM1;RM;"; // select measurement '1' (swr) and read meter
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "SWR", cmd, replystr);
	if (ret < 8) return 0;
	size_t p = replystr.rfind("RM1");
	if (p == string::npos) return 0;
	
	replystr[p + 7] = 0;
	int mtr = atoi(&replystr[p + 3]);
	mtr = (mtr * 100) / 15;
	return mtr;
}


//----------------------------------------------------------------------
// power output measurement 0 ... 30							//WBX
int RIG_TS870::get_power_out()
{
	cmd = "SM;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "P out", cmd, replystr);
	if (ret < 7) return 0;
	size_t p = replystr.rfind("SM");
	if (p == string::npos) return 0;
	
	replystr[p + 6] = 0;
	int mtr = atoi(&replystr[p + 2]);
	mtr = (int)(0.34 + (((0.035*mtr - 0.407)*mtr + 5.074)*mtr));
	if (mtr < 1) mtr = 0;
	if (mtr > 100) mtr = 100;
	return mtr;
}


//----------------------------------------------------------------------
// (xcvr power level is in 5W increments)
void RIG_TS870::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set pwr", cmd, replystr);
}

int RIG_TS870::get_power_control()
{
	cmd = "PC;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get pwr", cmd, replystr);
	if (ret < 6) return 0;
	size_t p = replystr.rfind("PC");
	if (p == string::npos) return 0;
	
	replystr[p + 5] = 0;
	int mtr = atoi(&replystr[p + 2]);
	return mtr;
}


//----------------------------------------------------------------------
void RIG_TS870::set_volume_control(int val)
{
	int ivol = (int)(val * 2.55);
	showresp(WARN, ASC, "set vol", cmd, replystr);
	cmd = "AG000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendCommand(cmd);
}

// Volume control return 0 ... 100  (rig values 0 ... 255)
int RIG_TS870::get_volume_control()
{
	cmd = "AG;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get vol", cmd, replystr);

	if (ret < 6) return 0;
	size_t p = replystr.rfind("AG");
	if (p == string::npos) return 0;
	
	replystr[p + 5] = 0;
	int val = atoi(&replystr[p + 2]);
	return (int)(val / 2.55);
}


//----------------------------------------------------------------------
// Tranceiver PTT on/off
void RIG_TS870::set_PTT_control(int val)
{
	if (val) cmd = "TX;";
	else	 cmd = "RX;";
	sendCommand(cmd);
	showresp(WARN, ASC, "PTT", cmd, replystr);
}


//----------------------------------------------------------------------
void RIG_TS870::tune_rig()
{
	cmd = "AC 11;";
	sendCommand(cmd);
	showresp(WARN, ASC, "TUNE", cmd, replystr);
}


//----------------------------------------------------------------------
// the TS-870 actually has 4 attenuator settings.
// RA00; = Off,  RA01; = 6dB,  RA02; = 12dB,  RA03; = 18dB      wbx
void RIG_TS870::set_attenuator(int val) {

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

//	Modified to read and show the actual radio setting, in the button.
int RIG_TS870::get_attenuator() {
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
// the TS-870 has no preamp   wbx
/* 
void RIG_TS870::set_preamp(int val)
{
	preamp_on = val;
	if (val) cmd = "PA1;";
	else	 cmd = "PA0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set pre", cmd, replystr);
}

int RIG_TS870::get_preamp()
{
	cmd = "PA;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get pre", cmd, replystr);
	if (ret < 4 ) return preamp_on;
	size_t p = replystr.rfind("PA");
	if (p == string::npos) return preamp_on;

	if (replystr[p + 2] == '1')
		preamp_on = 1;
	else
		preamp_on = 0;
	return preamp_on;
}
*/


//----------------------------------------------------------------------
void RIG_TS870::set_widths()
{
	switch (A.imode) {
	case 0:
	case 1:
	case 3:
	case 4:
	bandwidths_ = TS870_SSBwidths;
	A.iBW = 1;
	break;
	case 2:
	case 6:
	bandwidths_ = TS870_CWwidths;
	A.iBW = 5;
	break;
	case 5:
	case 7:
	bandwidths_ = TS870_FSKwidths;
	A.iBW = 2;
	break;
	default:
	break;
	}
}

const char **RIG_TS870::bwtable(int m)
{
	switch (m) {
		case 0:
		case 1:
		case 3:
		case 4:
			return TS870_SSBwidths;
			break;
		case 2:
		case 6:
			return TS870_CWwidths;
			break;
		case 5:
		case 7:
			return TS870_FSKwidths;
			break;
	}
	return TS870_SSBwidths;
}

void RIG_TS870::set_modeA(int val)
{
	A.imode = val;
	cmd = "MD";
	cmd += TS870_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set modeA", cmd, replystr);
	set_widths();
}

int RIG_TS870::get_modeA()
{
	cmd = "MD;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get modeA", cmd, replystr);
	if (ret < 4) return A.imode;
	size_t p = replystr.rfind("MD");
	if (p == string::npos) return A.imode;

	int md = replystr[p + 2];
	md = md - '1';
	if (md == 8) md = 7;
	A.imode = md;
	set_widths();
	return A.imode;
}

void RIG_TS870::set_modeB(int val)
{
	B.imode = val;
	cmd = "MD";
	cmd += TS870_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set modeB", cmd, replystr);
	set_widths();
}

int RIG_TS870::get_modeB()
{
	cmd = "MD;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get modeB", cmd, replystr);
	if (ret < 4) return B.imode;
	size_t p = replystr.rfind("MD");
	if (p == string::npos) return B.imode;

	int md = replystr[p + 2];
	md = md - '1';
	if (md == 8) md = 7;
	B.imode = md;
	set_widths();
	return B.imode;
}

int RIG_TS870::adjust_bandwidth(int val)
{
	switch (val) {
	case 0:
	case 1:
	case 3:
	case 4:
		bandwidths_ = TS870_SSBwidths;
		return 1;
	case 2:
	case 6:
		bandwidths_ = TS870_CWwidths;
		return 5;
	case 5:
	case 7:
		bandwidths_ = TS870_FSKwidths;
		return 2;
	}
	return 1;
}

int RIG_TS870::def_bandwidth(int val)
{
	switch (val) {
	case 0:
	case 1:
	case 3:
	case 4:
		return 1;
	case 2:
	case 6:
		return 5;
	case 5:
	case 7:
		return 2;
	}
	return 2;
}

void RIG_TS870::set_bwA(int val)
{
	A.iBW = val;

	switch (A.imode) {
	case 0:
	case 1:
	case 3:
	case 4:
		cmd = TS870_SSBbw[A.iBW];
		sendCommand(cmd, 0);
		break;
	case 2:
	case 6:
		cmd = TS870_CWbw[A.iBW];
		sendCommand(cmd, 0);
		break;
	case 5:
	case 7:
		cmd = TS870_FSKbw[A.iBW];
		sendCommand(cmd, 0);
		break;
	default:
		break;
	}
	showresp(WARN, ASC, "set bwA", cmd, replystr);
}

int RIG_TS870::get_bwA()
{
	int i;

	cmd = "FW;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get bwA", cmd, replystr);
	if (ret < 7) return A.iBW;
	size_t p = replystr.rfind("FW");
	if (p == string::npos) return A.iBW;

	string test = replystr.substr(p, 7);

	switch (A.imode) {
	case 0:
	case 1:
	case 3:
	case 4:
		for (i = 0; TS870_SSBbw[i] != NULL; i++)
			if (test == TS870_SSBbw[i])  break;
		if (TS870_SSBbw[i] != NULL) A.iBW = i;
		else A.iBW = 1;
		break;
	case 2:
	case 6:
		for (i = 0; TS870_CWbw[i] != NULL; i++)
			if (test == TS870_CWbw[i])  break;
		if (TS870_CWbw[i] != NULL) A.iBW = i;
		else A.iBW = 1;
		break;
	case 5:
	case 7:
		for (i = 0; TS870_FSKbw[i] != NULL; i++)
			if (test == TS870_FSKbw[i])  break;
		if (TS870_FSKbw[i] != NULL) A.iBW = i;
		else A.iBW = 1;
		break;
	default:
	break;
	}

	return A.iBW;
}

void RIG_TS870::set_bwB(int val)
{
	B.iBW = val;

	switch (B.imode) {
	case 0:
	case 1:
	case 3:
	case 4:
		cmd = TS870_SSBbw[B.iBW];
		sendCommand(cmd, 0);
		break;
	case 2:
	case 6:
		cmd = TS870_CWbw[B.iBW];
		sendCommand(cmd, 0);
		break;
	case 5:
	case 7:
		cmd = TS870_FSKbw[B.iBW];
		sendCommand(cmd, 0);
		break;
	default:
		break;
	}
	showresp(WARN, ASC, "set bwB", cmd, replystr);
}

int RIG_TS870::get_bwB()
{
	int i;

	cmd = "FW;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get bwB", cmd, replystr);
	if (ret < 7) return B.iBW;
	size_t p = replystr.rfind("FW");
	if (p == string::npos) return B.iBW;

	string test = replystr.substr(p,7);

	switch (B.imode) {
	case 0:
	case 1:
	case 3:
	case 4:
		for (i = 0; TS870_SSBbw[i] != NULL; i++)
			if (test == TS870_SSBbw[i])  break;
		if (TS870_SSBbw[i] != NULL) B.iBW = i;
		else B.iBW = 1;
		break;
	case 2:
	case 6:
		for (i = 0; TS870_CWbw[i] != NULL; i++)
			if (test == TS870_CWbw[i])  break;
		if (TS870_CWbw[i] != NULL) B.iBW = i;
		else B.iBW = 1;
		break;
	case 5:
	case 7:
		for (i = 0; TS870_FSKbw[i] != NULL; i++)
			if (test == TS870_FSKbw[i])  break;
		if (TS870_FSKbw[i] != NULL) B.iBW = i;
		else B.iBW = 1;
		break;
	default:
	break;
	}

	return B.iBW;
}

int RIG_TS870::get_modetype(int n)
{
	return TS870_mode_type[n];
}

// val 0 .. 100
void RIG_TS870::set_mic_gain(int val)
{
	cmd = "MG000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set mic", cmd, replystr);
}

int RIG_TS870::get_mic_gain()
{
	cmd = "MG;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get mic", cmd, replystr);
	if (ret < 6) return 0;
	size_t p = replystr.rfind("MG");
	if (p == string::npos) return 0;

	replystr[p + 5] = 0;
	int val = atoi(&replystr[p + 2]);
	return val;
}

void RIG_TS870::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_TS870::set_noise(bool b)
{
	if (b)
		cmd = "NB1;";
	else
		cmd = "NB0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set NB", cmd, replystr);
}

int  RIG_TS870::get_noise()
{
	cmd = "NB;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get NB", cmd, replystr);
	if (ret < 4) return 0;
	size_t p = replystr.rfind("NB");
	if (p == string::npos) return 0;

	return (replystr[p + 2] == '1');
}

//======================================================================
// IF shift command   The TS-870 does thing is a slightly different way
// step size is 50 Hz
//======================================================================
void RIG_TS870::set_if_shift(int val)
{
	cmd = "IS 0000;";
	if (val < 0) cmd[2] = '-';
	val = abs(val);
	for (int i = 6; i > 2; i--) {
		cmd[i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set IF shift", cmd, replystr);
}

bool RIG_TS870::get_if_shift(int &val)
{
	size_t p = 0;
	cmd = "IS;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get IF shift", cmd, replystr);

	if (ret >= 8) {
		p = replystr.rfind("IS");
		if (p == string::npos) return false;
		replystr[p + 7] = 0;
		val = atoi(&replystr[p + 3]);
		if (replystr[p+2] == '-') val = -val;
		return true;
	}
	val = 0;
	return false;
}

void RIG_TS870::get_if_min_max_step(int &min, int &max, int &step)
{
	min = 400;		//wbx
	max = 1000;
	step = 50;		//wbx
}

// The two auto-notch routines, pulled from the TS-2000 driver.	   wbx
void RIG_TS870::set_auto_notch(int v)
{
	cmd = v ? "NT1;" : "NT0;";
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set auto notch", cmd, replystr);
}

int  RIG_TS870::get_auto_notch()
{
	cmd = "NT;";
	waitN(4, 100, "get auto notch", ASC);
	size_t p = replystr.rfind("NT");
	if (p != string::npos)
		return (replystr[p+2] == '1');
	return 0;
}
