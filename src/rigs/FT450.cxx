/*
 * Yaesu FT-450 drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */


#include "FT450.h"
#include "rig.h"

static const char FT450name_[] = "FT-450";

static const char *FT450modes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "RTTY-L", "CW-R", "USER-L", "RTTY-U", "FM-N", "USER-U", NULL};
static const char mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '8', '9', 'B', 'C' };
static const char mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'L', 'U', 'U', 'U' };

static const char *FT450_widths[] = {"NARR", "NORM", "WIDE", NULL};

static const char *FT450_US_60m[] = {NULL, "126", "127", "128", "130", NULL};
// US has 5 60M presets. Using dummy numbers for all.
// First NULL means skip 60m sets in set_band_selection().
// Maybe someone can do a cat command MC; on all 5 presets and add returned numbers above.
// To send cat commands in flrig goto menu Config->Xcvr select->Send Cmd.

static const char **Channels_60m = FT450_US_60m;

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)btnNotch,    214, 125,  50 },
	{ (Fl_Widget *)sldrNOTCH,   266, 125, 156 },
	{ (Fl_Widget *)sldrMICGAIN,  54, 145, 156 },
	{ (Fl_Widget *)sldrPOWER,    54, 165, 368 },
	{ (Fl_Widget *)btnNR,       214, 145,  50 },
	{ (Fl_Widget *)sldrNR,      266, 145, 156 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

RIG_FT450::RIG_FT450() {
// base class values	
	name_ = FT450name_;
	modes_ = FT450modes_;
	bandwidths_ = FT450_widths;

	widgets = rig_widgets;

	comm_baudrate = BR38400;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	modeA = 1;
	bwA = 2;
	def_mode = 10;
	def_bw = 2;
	def_freq = 14070000;

	has_noise_reduction =
	has_noise_reduction_control =
	has_band_selection =
	has_extras =
	has_vox_onoff =
	has_vox_gain =
	has_vox_hang =

	has_cw_wpm =
	has_cw_keyer =
//	has_cw_vol =
	has_cw_spot =
	has_cw_spot_tone =
	has_cw_qsk =
	has_cw_weight =

	has_split =
	has_smeter =
	has_swr_control =
	has_power_out =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_noise_control =
	has_bandwidth_control =
	has_micgain_control =
	has_notch_control =
	has_attenuator_control =
	has_preamp_control =
	has_ifshift_control =
	has_ptt_control =
	has_tune_control =
	has_special = true;
	
// derived specific
	notch_on = false;
	m_60m_indx = 0;
}

void RIG_FT450::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = btnIFsh;
	rig_widgets[3].W = sldrIFSHIFT;
	rig_widgets[4].W = btnNotch;
	rig_widgets[5].W = sldrNOTCH;
	rig_widgets[6].W = sldrMICGAIN;
	rig_widgets[7].W = sldrPOWER;
	rig_widgets[8].W = btnNR;
	rig_widgets[9].W = sldrNR;

	selectA();
}

void RIG_FT450::set_band_selection(int v)
{
	int inc_60m = false;
	cmd = "IF;";
	waitN(27, 100, "get vfo mode in set_band_selection", ASC);
	size_t p = replystr.rfind("IF");
	if (p == string::npos) return;
	if (replystr[p+21] != '0') {	// vfo 60M memory mode
		inc_60m = true;
	}

	if (v == 12) {	// 5MHz 60m presets
		if (Channels_60m[0] == NULL) return;	// no 60m Channels so skip
		if (inc_60m) {
			if (Channels_60m[++m_60m_indx] == NULL)
				m_60m_indx = 0;
		}
		cmd.assign("MC").append(Channels_60m[m_60m_indx]).append(";");
	} else {		// v == 1..11 band selection OR return to vfo mode == 0
		if (inc_60m)
			cmd = "VM;";
		else {
			if (v < 3)
				v = v - 1;
			cmd.assign("BS").append(to_decimal(v, 2)).append(";");
		}
	}

	sendCommand(cmd);
	showresp(WARN, ASC, "Select Band Stacks", cmd, replystr);
}

int  RIG_FT450::adjust_bandwidth(int m)
{
	return 1;
}

void RIG_FT450::selectA()
{
	cmd = "SV0;";
	sendCommand(cmd, 0);
}

void RIG_FT450::selectB()
{
	cmd = "SV1;";
	sendCommand(cmd, 0);
}

long RIG_FT450::get_vfoA ()
{
	cmd = rsp = "FA";
	cmd += ';';
	waitN(11, 100, "get vfo A", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return freqA;
	int f = 0;
	for (size_t n = 2; n < 10; n++)
		f = f*10 + replystr[p+n] - '0';
	freqA = f;
	return freqA;
}

void RIG_FT450::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "FA00000000;";
	for (int i = 9; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo A", cmd, replystr);
}

long RIG_FT450::get_vfoB ()
{
	cmd = rsp = "FB";
	cmd += ';';
	waitN(11, 100, "get vfo B", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return freqB;
	int f = 0;
	for (size_t n = 2; n < 10; n++)
		f = f*10 + replystr[p+n] - '0';
	freqB = f;
	return freqB;
}

void RIG_FT450::set_vfoB (long freq)
{
	freqB = freq;
	cmd = "FB00000000;";
	for (int i = 9; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo B", cmd, replystr);
}

void RIG_FT450::set_split(bool on)
{
	if (on) cmd = "FT1;";
	else cmd = "FT0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET split", cmd, replystr);
}

bool RIG_FT450::get_split()
{
	cmd = rsp = "FT";
	cmd += ';';
	waitN(4, 100, "get split", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return false;
	return replystr[p+2] == '1' ? true : false;
}

int RIG_FT450::get_smeter()
{
	cmd = rsp = "SM0";
	cmd += ';';
	waitN(7, 100, "get smeter", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	replystr[p+6] = 0;
	int mtr = atoi(&replystr[p+3]);
	mtr = mtr * 100.0 / 256.0;
	return mtr;
}

// measured by W3NR
//  SWR..... mtr ... display
//  6:1..... 255 ... 100
//  3:1..... 132 ...  50
//  2:1..... 066 ...  26
//  2.5:1... 099 ...  39
//  1.5:1... 033 ...  13
//  1.1:1... 008 ...   3

int RIG_FT450::get_swr()
{
	cmd = rsp = "RM6";
	cmd += ';';
	waitN(7, 100, "get swr", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	replystr[p+6] = 0;
	int mtr = atoi(&replystr[p+3]);
	return mtr / 2.55;
}


int RIG_FT450::get_power_out()
{
	cmd = rsp = "RM5";
	sendCommand(cmd.append(";"));
	waitN(7, 100, "get pout", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	replystr[p+6] = 0;
	double mtr = (double)(atoi(&replystr[p+3]));
	mtr = -6.6263535 + .11813178 * mtr + .0013607405 * mtr * mtr;
	return (int)mtr;
}

int RIG_FT450::get_power_control()
{
	cmd = rsp = "PC";
	cmd += ';';
	waitN(6, 100, "get power", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	replystr[p+5] = 0;
	int mtr = atoi(&replystr[p+2]);
	return mtr;
}

void RIG_FT450::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET power", cmd, replystr);
}

// Volume control return 0 ... 100
int RIG_FT450::get_volume_control()
{
	cmd = rsp = "AG0";
	cmd += ';';
	waitN(7, 100, "get vol", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.volume;
	if (p + 6 >= replystr.length()) return progStatus.volume;
	int val = atoi(&replystr[p+3]) * 100 / 250;
	if (val > 100) val = 100;
	return val;
}

void RIG_FT450::set_volume_control(int val) 
{
	int ivol = (int)(val * 250 / 100);
	cmd = "AG0000;";
	for (int i = 5; i > 2; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vol", cmd, replystr);
}

void RIG_FT450::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 255; step = 1;
}

// Tranceiver PTT on/off
void RIG_FT450::set_PTT_control(int val)
{
	cmd = val ? "TX1;" : "TX0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET PTT", cmd, replystr);
}

void RIG_FT450::tune_rig()
{
	cmd = "AC002;";
	sendCommand(cmd);
	showresp(WARN, ASC, "tune rig", cmd, replystr);
}

void RIG_FT450::set_attenuator(int val)
{
	if (val) cmd = "RA01;";
	else	 cmd = "RA00;";
	sendCommand(cmd);
	showresp(WARN, ASC, "get att", cmd, replystr);
}

int RIG_FT450::get_attenuator()
{
	cmd = rsp = "RA0";
	cmd += ';';
	waitN(5, 100, "get att", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	return (replystr[p+3] == '1' ? 1 : 0);
}

void RIG_FT450::set_preamp(int val)
{
	if (val) cmd = "PA00;";
	else	 cmd = "PA01;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set preamp", cmd, replystr);
}

int RIG_FT450::get_preamp()
{
	cmd = rsp = "PA0";
	cmd += ';';
	waitN(5, 100, "get pre", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	return (replystr[p+3] == '1' ? 0 : 1);
}

void RIG_FT450::set_modeA(int val)
{
	modeA = val;
	cmd = "MD0";
	cmd += mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode A", cmd, replystr);
	if (val == 2 || val == 6) return;
	if (progStatus.spot_onoff) {
		progStatus.spot_onoff = false;
		set_spot_onoff();
		cmd = "CS0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "SET spot off", cmd, replystr);
		btnSpot->value(0);
	}
}

int RIG_FT450::get_modeA()
{
	cmd = rsp = "MD0";
	cmd += ';';
	waitN(5, 100, "get mode A", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return modeA;
	int md = replystr[p+3];
	if (md <= '9') md = md - '1';
	else md = 9 + md - 'B';
	modeA = md;
	return modeA;
}

void RIG_FT450::set_bwA(int val)
{
	bwA = val;
	switch (val) {
		case 0 : cmd = "SH000;"; break;
		case 1 : cmd = "SH016;"; break;
		case 2 : cmd = "SH031;"; break;
		default: cmd = "SH031;";
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET bwA", cmd, replystr);
}

int RIG_FT450::get_bwA()
{
	cmd = rsp = "SH0";
	cmd += ';';
	waitN(6, 100, "get bw A", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return bwA;
	string bws = replystr.substr(p+3,2);
	if (bws == "00") bwA = 0;
	else if (bws == "16") bwA = 1;
	else if (bws == "31") bwA = 2;
	return bwA;
}

void RIG_FT450::set_modeB(int val)
{
	modeB = val;
	cmd = "MD0";
	cmd += mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode B", cmd, replystr);
	if (val == 2 || val == 6) return;
	if (progStatus.spot_onoff) {
		progStatus.spot_onoff = false;
		set_spot_onoff();
		cmd = "CS0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "SET spot off", cmd, replystr);
		btnSpot->value(0);
	}
}

int RIG_FT450::get_modeB()
{
	cmd = rsp = "MD0";
	cmd += ';';
	waitN(5, 100, "get mode B", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return modeB;
	int md = replystr[p+3];
	if (md <= '9') md = md - '1';
	else md = 9 + md - 'B';
	modeB = md;
	return modeB;
}

void RIG_FT450::set_bwB(int val)
{
	bwB = val;
	switch (val) {
		case 0 : cmd = "SH000;"; break;
		case 1 : cmd = "SH016;"; break;
		case 2 : cmd = "SH031;"; break;
		default: cmd = "SH031;";
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET bwB", cmd, replystr);
}

int RIG_FT450::get_bwB()
{
	cmd = rsp = "SH0";
	cmd += ';';
	waitN(6, 100, "get bw B", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return bwB;
	string bws = replystr.substr(p+3,2);
	if (bws == "00") bwB = 0;
	else if (bws == "16") bwB = 1;
	else if (bws == "31") bwB = 2;
	return bwB;
}

int RIG_FT450::get_modetype(int n)
{
	return mode_type[n];
}

void RIG_FT450::set_if_shift(int val)
{
	cmd = "IS0+0000;";
	if (val < 0) cmd[3] = '-';
	val = abs(val);
	for (int i = 4; i > 0; i--) {
		cmd[3+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET ifshift", cmd, replystr);
}

bool RIG_FT450::get_if_shift(int &val)
{
	cmd = rsp = "IS0";
	cmd += ';';
	waitN(9, 100, "get if shift", ASC);

	size_t p = replystr.rfind(rsp);
	val = progStatus.shift_val;
	if (p == string::npos) return progStatus.shift;
	val = atoi(&replystr[p+4]);
	if (replystr[p+3] == '-') val = -val;
	return (val != 0);
}

void RIG_FT450::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -1000;
	max = 1000;
	step = 100;
}

void RIG_FT450::set_notch(bool on, int val)
{
	cmd = "BP00000;";
	if (on == false) {
		sendCommand(cmd);
		showresp(WARN, ASC, "SET notch off", cmd, replystr);
		notch_on = false;
		return;
	}
	cmd[6] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET notch on", cmd, replystr);
	notch_on = true;

	cmd[3] = '1'; // manual NOTCH position
	cmd[6] = '0';
	val += 200;
	for (int i = 3; i > 0; i--) {
		cmd[3 + i] += val % 10;
		val /=10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET notch val", cmd, replystr);
}

bool  RIG_FT450::get_notch(int &val)
{
	bool ison = false;
	cmd = rsp = "BP00";
	cmd += ';';
	waitN(8, 100, "get notch on/off", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return ison;

	if (replystr[p+6] == '1') { // manual notch enabled
		ison = true;
		val = progStatus.notch_val;
		cmd = rsp = "BP01";
		cmd += ';';
		waitN(8, 100, "get notch val", ASC);
		p = replystr.rfind(rsp);
		if (p == string::npos) return ison;
		val = atoi(&replystr[p+4]);
		val *= 10;
		val -= 200;
	}
	return ison;
}

void RIG_FT450::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = -190;
	max = +190;
	step = 10;
}

void RIG_FT450::set_noise(bool b)
{
	if (b)
		cmd = "NB01;";
	else
		cmd = "NB00;";
	sendCommand (cmd);
	showresp(WARN, ASC, "SET NB", cmd, replystr);
}

// val 0 .. 100
void RIG_FT450::set_mic_gain(int val)
{
	cmd = "MG000;";
	val = (int)(val * 2.55); // convert to 0 .. 255
	for (int i = 3; i > 0; i--) {
		cmd[1+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mic", cmd, replystr);
}

int RIG_FT450::get_mic_gain()
{
	cmd = rsp = "MG";
	cmd += ';';
	waitN(6, 100, "get mic", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	replystr[p+5] = 0;
	return atoi(&replystr[p+2]);;
}

void RIG_FT450::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_FT450::set_special(int v)
{
	if (v) cmd = "VR1;";
	else   cmd = "VR0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Set special", cmd, replystr);
}

void RIG_FT450::set_vox_onoff()
{
	cmd = "VX0;";
	if (progStatus.vox_onoff) cmd[2] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox on/off", cmd, replystr);
}

void RIG_FT450::set_vox_gain()
{
	cmd = "VG";
	cmd.append(to_decimal(progStatus.vox_gain, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox gain", cmd, replystr);
}

void RIG_FT450::set_vox_hang()
{
	cmd = "VD";
	cmd.append(to_decimal(progStatus.vox_hang, 4)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox delay", cmd, replystr);
}

void RIG_FT450::set_cw_wpm()
{
	cmd = "KS";
	if (progStatus.cw_wpm > 60) progStatus.cw_wpm = 60;
	if (progStatus.cw_wpm < 4) progStatus.cw_wpm = 4;
	cmd.append(to_decimal(progStatus.cw_wpm, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw wpm", cmd, replystr);
}


void RIG_FT450::enable_keyer()
{
	cmd = "KR0;";
	if (progStatus.enable_keyer) cmd[2] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET keyer on/off", cmd, replystr);
}

bool RIG_FT450::set_cw_spot()
{
	if (vfo.imode == 2 || vfo.imode == 6) {
		cmd = "CS0;";
		if (progStatus.spot_onoff) cmd[2] = '1';
		sendCommand(cmd);
		showresp(WARN, ASC, "SET spot on/off", cmd, replystr);
		return true;
	} else
		return false;
}

void RIG_FT450::set_cw_weight()
{
	int n = round(progStatus.cw_weight * 10);
	cmd.assign("EX024").append(to_decimal(n, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw weight", cmd, replystr);
}

void RIG_FT450::set_cw_qsk()
{
	int n = progStatus.cw_qsk / 5 - 3;
	cmd.assign("EX018").append(to_decimal(n, 1)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw qsk", cmd, replystr);
}

void RIG_FT450::set_cw_spot_tone()
{
	int n = (progStatus.cw_spot_tone - 400) / 50 + 1;
	cmd = "EX020";
	cmd.append(to_decimal(n, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw tone", cmd, replystr);
}

// DNR
void RIG_FT450::set_noise_reduction_val(int val)
{
	cmd.assign("RL0").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_val", cmd, replystr);
}

int  RIG_FT450::get_noise_reduction_val()
{
	int val = 1;
	cmd = rsp = "RL0";
	cmd.append(";");
	waitN(6, 100, "GET noise reduction val", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return val;
	val = atoi(&replystr[p+3]);
	return val;
}

// DNR
void RIG_FT450::set_noise_reduction(int val)
{
	cmd.assign("NR0").append(val ? "1" : "0" ).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET noise reduction", cmd, replystr);
}

int  RIG_FT450::get_noise_reduction()
{
	int val;
	cmd = rsp = "NR0";
	cmd.append(";");
	waitN(5, 100, "GET noise reduction", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	val = replystr[p+3] - '0';
	return val;
}
