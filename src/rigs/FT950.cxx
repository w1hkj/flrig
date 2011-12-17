/*
 * Yaesu FT-950 drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * Copyright 2011, Terry Embry, KJ4EED
 * 
 */


#include "FT950.h"
#include "debug.h"
#include "support.h"

#define WVALS_LIMIT -1

static const char FT950name_[] = "FT-950";

static const char *FT950modes_[] = {
"LSB", "USB", "CW", "FM", "AM", "RTTY-L",
"CW-R", "PKT-L", "RTTY-U", "PKT-FM",
"FM-N", "PKT-U", "AM-N", NULL};

static const char FT950_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D' };
static const char FT950_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'L', 'U', 'U', 'U', 'U', 'U' };

// 20110707 - SSB "2450", 14 discontinued in latest North American firmware 
static const int FT950_def_bw[] = { 18, 18, 5, 0, 0, 5, 5, 18, 5, 0, 0, 18, 0 };
static const char *FT950_widths_SSB[] = {
"200", "400", "600", "850", "1100", "1350", "1500", "1650", "1800",
"1950", "2100", "2250", "2400", "2500", "2600", "2700",
"2800", "2900", "3000", NULL };

static int FT950_wvals_SSB[] = {
1,2,3,4,5,6,7,8,9,10,11,12,13,15,16,17,18,19,20, WVALS_LIMIT};

static const char *FT950_widths_CW[] = {
"100", "200", "300", "400", "500",
"800", "1200", "1400", "1700", "2000", "2400", NULL };

static int FT950_wvals_CW[] = {
3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, WVALS_LIMIT };

static const int FT950_wvals_AMFM[] = { 0, WVALS_LIMIT };

static const char *FT950_widths_AMnar[]  = {  "6000", NULL };
static const char *FT950_widths_AMwide[] = {  "9000", NULL };
static const char *FT950_widths_FMnar[]  = { "12500", NULL };
static const char *FT950_widths_FMwide[] = { "25000", NULL };

static const char *FT950_US_60m[] = {"125", "126", "127", "128", "130", NULL};
// UK has 7 60M presets. Using dummy numbers for all.  If you want support,
// Maybe someone can do a cat command MC; on all 7 presets and add returned numbers below.
// To send cat commands in flrig goto menu Config->Xcvr select->Send Cmd.
// static const char *FT950_UK_60m[] = {"125", "126", "127", "128", "130", "131", "132", NULL};

static const char **Channels_60m = FT950_US_60m;

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 },
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)btnNotch,    214, 125,  50 },
	{ (Fl_Widget *)sldrNOTCH,   266, 125, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 145, 156 },
	{ (Fl_Widget *)sldrPOWER,    54, 165, 368 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

RIG_FT950::RIG_FT950() {
// base class values	
	name_ = FT950name_;
	modes_ = FT950modes_;
	bandwidths_ = FT950_widths_SSB;
	bw_vals_ = FT950_wvals_SSB;

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

	A.imode = B.imode = modeB = modeA = def_mode = 1;
	A.iBW = B.iBW = bwA = bwB = def_bw = 2;
	A.freq = B.freq = freqA = freqB = def_freq = 14070000;

	has_band_selection =
	has_noise_reduction =
	has_noise_reduction_control =
	has_extras =
	has_vox_onoff =
	has_vox_gain =
	has_vox_anti =
	has_vox_hang =
	has_vox_on_dataport =

	has_cw_wpm =
	has_cw_keyer =
//	has_cw_vol =
	has_cw_spot =
	has_cw_spot_tone =
	has_cw_qsk =
	has_cw_weight =

	can_change_alt_vfo =
	has_smeter =
	has_swr_control =
	has_power_out =
	has_power_control =
	has_volume_control =
	has_rf_control =
	has_micgain_control =
	has_mode_control =
	has_noise_control =
	has_bandwidth_control =
	has_notch_control =
	allow_notch_changes =
	has_auto_notch =
	has_attenuator_control =
	has_preamp_control =
	has_ifshift_control =
	has_ptt_control =
	has_tune_control =
	has_swr_control = true;

// derived specific
	atten_level = 0;
	preamp_level = 0;
// set defaults
	progStatus.use_rig_data = true;
	if (!progStatus.use_rig_data) {
		progStatus.notch = false;
		progStatus.notch_val = 1500;
		progStatus.noise_reduction = false;
		progStatus.noise_reduction_val = 1;
	}
}

void RIG_FT950::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrRFGAIN;
	rig_widgets[3].W = btnIFsh;
	rig_widgets[4].W = sldrIFSHIFT;
	rig_widgets[5].W = btnNotch;
	rig_widgets[6].W = sldrNOTCH;
	rig_widgets[7].W = sldrMICGAIN;
	rig_widgets[8].W = sldrPOWER;
}


long RIG_FT950::get_vfoA ()
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

void RIG_FT950::set_vfoA (long freq)
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

long RIG_FT950::get_vfoB ()
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


void RIG_FT950::set_vfoB (long freq)
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


bool RIG_FT950::twovfos()
{
	return true;
}

void RIG_FT950::selectA()
{
	cmd = "FR0;FT2;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select A", cmd, replystr);
}

void RIG_FT950::selectB()
{
	cmd = "FR4;FT3;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select B", cmd, replystr);
}

void RIG_FT950::A2B()
{
	cmd = "AB;";
	sendCommand(cmd);
	showresp(WARN, ASC, "vfo A --> B", cmd, replystr);
}

bool RIG_FT950::can_split()
{
	return true;
}

void RIG_FT950::set_split(bool val)
{
	split = val;
	if (val) {
		useB = false;
		cmd = "FR0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "Rx on A", cmd, replystr);
		cmd = "FT3;";
		sendCommand(cmd);
		showresp(WARN, ASC, "Tx on B", cmd, replystr);
	} else {
		cmd = "FR0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "Rx on A", cmd, replystr);
		cmd = "FT2;";
		sendCommand(cmd);
		showresp(WARN, ASC, "Tx on A", cmd, replystr);
	}
	Fl::awake(highlight_vfo, (void *)0);
}

bool RIG_FT950::get_split()
{
	return split;
}


int RIG_FT950::get_smeter()
{
	cmd = rsp = "SM0";
	cmd += ';';
	waitN(7, 100, "get smeter", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int mtr = atoi(&replystr[p+3]);
	mtr = mtr * 100.0 / 256.0;
	return mtr;
}

int RIG_FT950::get_swr()
{
	cmd = rsp = "RM6";
	cmd += ';';
	waitN(7, 100, "get swr", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int mtr = atoi(&replystr[p+3]);
	return mtr / 2.56;
}

int RIG_FT950::get_power_out()
{
	cmd = rsp = "RM5";
	sendCommand(cmd.append(";"));
	waitN(7, 100, "get pout", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	double mtr = (double)(atoi(&replystr[p+3]));

// following conversion iaw data measured by Terry, KJ4EED
	mtr = (.06 * mtr) + (.002 * mtr * mtr);

	return (int)mtr;
}

// Transceiver power level
int RIG_FT950::get_power_control()
{
	cmd = rsp = "PC";
	cmd += ';';
	waitN(6, 100, "get power", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.power_level;
	if (p + 5 >= replystr.length()) return progStatus.power_level;

	int mtr = atoi(&replystr[p+2]);
	return mtr;
}

void RIG_FT950::set_power_control(double val)
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
int RIG_FT950::get_volume_control()
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

void RIG_FT950::set_volume_control(int val) 
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

// Tranceiver PTT on/off
void RIG_FT950::set_PTT_control(int val)
{
	cmd = val ? "TX1;" : "TX0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET PTT", cmd, replystr);
}

void RIG_FT950::tune_rig()
{
	cmd = "AC002;";
	sendCommand(cmd);
	showresp(WARN, ASC, "tune rig", cmd, replystr);
}

void RIG_FT950::set_attenuator(int val)
{
	int cmdval = 0;
	if (atten_level == 0) {
		atten_level = 1;
		atten_label("6 dB", true);
		cmdval = 0x06;
	} else if (atten_level == 1) {
		atten_level = 2;
		atten_label("12 dB", true);
		cmdval = 0x12;
	} else if (atten_level == 2) {
		atten_level = 3;
		atten_label("18 dB", true);
		cmdval = 0x18;
	} else if (atten_level == 3) {
		atten_level = 0;
		atten_label("Att", false);
		cmdval = 0x00;
	}
	cmd = "RA00;";
	cmd[3] += atten_level;
	sendCommand(cmd);
	showresp(WARN, ASC, "SET att", cmd, replystr);
}

int RIG_FT950::get_attenuator()
{
	cmd = rsp = "RA0";
	cmd += ';';
	waitN(5, 100, "get att", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.attenuator;
	if (p + 3 >= replystr.length()) return progStatus.attenuator;
	atten_level = replystr[p+3] - '0';
	if (atten_level == 1) {
		atten_label("6 dB", true);
	} else if (atten_level == 2) {
		atten_label("12 dB", true);
	} else if (atten_level == 3) {
		atten_label("18 dB", true);
	} else {
		atten_level = 0;
		atten_label("Att", false);
	}
	return atten_level;
}

void RIG_FT950::set_preamp(int val)
{
	cmd = "PA00;";
	if (preamp_level == 0) {
		preamp_level = 1;
		preamp_label("Amp 1", true);
	} else if (preamp_level == 1) {
		preamp_level = 2;
		preamp_label("Amp 2", true);
	} else if (preamp_level == 2) {
		preamp_level = 0;
		preamp_label("IPO", false);
	}
	cmd[3] = '0' + preamp_level;
	sendCommand (cmd);
	showresp(WARN, ASC, "SET preamp", cmd, replystr);
}

int RIG_FT950::get_preamp()
{
	cmd = rsp = "PA0";
	cmd += ';';
	waitN(5, 100, "get pre", ASC);

	size_t p = replystr.rfind(rsp);
	if (p != string::npos)
		preamp_level = replystr[p+3] - '0';
	if (preamp_level == 1) {
		preamp_label("Amp 1", true);
	} else if (preamp_level == 2) {
		preamp_label("Amp 2", true);
	} else {
		preamp_label("IPO", false);
		preamp_level = 0;
	}
	return preamp_level;
}

int RIG_FT950::adjust_bandwidth(int val)
{
	int bw = 0;
	if (val == 2 || val == 5 || val == 6 || val == 8) {
		bandwidths_ = FT950_widths_CW;
		bw_vals_ = FT950_wvals_CW;
		bw = FT950_def_bw[val];
	} else if (val == 3 || val == 4 || val == 10 || val == 12) {
		if (val == 3) bandwidths_ = FT950_widths_FMwide;
		else if (val ==  4) bandwidths_ = FT950_widths_AMwide;
		else if (val == 10) bandwidths_ = FT950_widths_FMnar;
		else if (val == 12) bandwidths_ = FT950_widths_AMnar;
		bw_vals_ = FT950_wvals_AMFM;
		bw = FT950_def_bw[val];
	} else {
		bandwidths_ = FT950_widths_SSB;
		bw_vals_ = FT950_wvals_SSB;
		bw = FT950_def_bw[val];
	}
	return bw;
}

int RIG_FT950::def_bandwidth(int val)
{
	return FT950_def_bw[val];
}

const char ** RIG_FT950::bwtable(int n)
{
	if (n == 3) return FT950_widths_FMwide;
	else if (n == 4) return FT950_widths_AMwide;
	else if (n == 10) return FT950_widths_FMnar;
	else if (n == 12) return FT950_widths_AMnar;
	else if (n == 2 || n == 5 || n == 6 || n == 8)
		return FT950_widths_CW;
	return FT950_widths_SSB;
}

void RIG_FT950::set_modeA(int val)
{
	modeA = val;
	cmd = "MD0";
	cmd += FT950_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode A", cmd, replystr);
	adjust_bandwidth(modeA);
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

int RIG_FT950::get_modeA()
{
	cmd = rsp = "MD0";
	cmd += ';';
	waitN(5, 100, "get mode A", ASC);

	size_t p = replystr.rfind(rsp);
	if (p != string::npos) {
		if (p + 3 < replystr.length()) {
			int md = replystr[p+3];
			if (md <= '9') md = md - '1';
			else md = 9 + md - 'A';
			modeA = md;
		}
	}
	adjust_bandwidth(modeA);
	return modeA;
}

void RIG_FT950::set_modeB(int val)
{
	modeB = val;
	cmd = "MD0";
	cmd += FT950_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode B", cmd, replystr);
	adjust_bandwidth(modeB);
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

int RIG_FT950::get_modeB()
{
	cmd = rsp = "MD0";
	cmd += ';';
	waitN(5, 100, "get mode B", ASC);

	size_t p = replystr.rfind(rsp);
	if (p != string::npos) {
		if (p + 3 < replystr.length()) {
			int md = replystr[p+3];
			if (md <= '9') md = md - '1';
			else md = 9 + md - 'A';
			modeB = md;
		}
	}
	adjust_bandwidth(modeB);
	return modeB;
}

void RIG_FT950::set_bwA(int val)
{
	int bw_indx = bw_vals_[val];
	bwA = val;

	if (modeA == 3 || modeA == 4 || modeA == 10 || modeA == 12) {
		return;
	}
	if ((((modeA == 0 || modeA == 1) && val < 8)) ||
		((modeA == 2 || modeA == 5 || modeA == 6 || modeA == 7 || modeA == 8 || modeA == 11) &&
		val < 4) ) cmd = "NA01;";
	else cmd = "NA00;";

	cmd.append("SH0");
	cmd += '0' + bw_indx / 10;
	cmd += '0' + bw_indx % 10;
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET bw A", cmd, replystr);
}

int RIG_FT950::get_bwA()
{
	if (modeA == 3 || modeA == 4 || modeA == 10 || modeA == 12) {
		bwA = 0;
		return bwA;	
	} 
	cmd = rsp = "SH0";
	cmd += ';';
	waitN(6, 100, "get bw A", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return bwA;
	if (p + 5 >= replystr.length()) return bwA;

	replystr[p+5] = 0;
	int bw_idx = fm_decimal(&replystr[p+3],2);
	const int *idx = bw_vals_;
	int i = 0;
	while (*idx != WVALS_LIMIT) {
		if (*idx == bw_idx) break;
		idx++;
		i++;
	}
	if (*idx == WVALS_LIMIT) i--;
	bwA = i;
	return bwA;
}

void RIG_FT950::set_bwB(int val)
{
	int bw_indx = bw_vals_[val];
	bwB = val;

	if (modeB == 3 || modeB == 4 || modeB == 10 || modeB == 12) {
		return;
	}
	if ((((modeB == 0 || modeB == 1) && val < 8)) ||
		((modeB == 2 || modeB == 5 || modeB == 6 || modeB == 7 || modeB == 8 || modeB == 11) &&
		val < 4) ) cmd = "NA1;";
	else cmd = "NA0;";

	cmd.append("SH0");
	cmd += '0' + bw_indx / 10;
	cmd += '0' + bw_indx % 10;
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET bw B", cmd, replystr);
}

int RIG_FT950::get_bwB()
{
	if (modeB == 3 || modeB == 4 || modeB == 10 || modeB == 12) {
		bwB = 0;
		return bwB;
	} 
	cmd = rsp = "SH0";
	cmd += ';';
	waitN(6, 100, "get bw B", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return bwB;
	if (p + 5 >= replystr.length()) return bwB;
	
	replystr[p+5] = 0;
	int bw_idx = fm_decimal(&replystr[p+3],2);
	const int *idx = bw_vals_;
	int i = 0;
	while (*idx != WVALS_LIMIT) {
		if (*idx == bw_idx) break;
		idx++;
		i++;
	}
	if (*idx == WVALS_LIMIT) i--;
	bwB = i;
	return bwB;
}

int RIG_FT950::get_modetype(int n)
{
	return FT950_mode_type[n];
}

void RIG_FT950::set_if_shift(int val)
{
	cmd = "IS0+0000;";
	if (val != 0) progStatus.shift = true;
	else progStatus.shift = false;
	if (val < 0) cmd[3] = '-';
	val = abs(val);
	for (int i = 4; i > 0; i--) {
		cmd[3+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET if shift", cmd, replystr);
}

bool RIG_FT950::get_if_shift(int &val)
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

void RIG_FT950::get_if_min_max_step(int &min, int &max, int &step)
{
	if_shift_min = min = -1000;
	if_shift_max = max = 1000;
	if_shift_step = step = 20;
	if_shift_mid = 0;
}

void RIG_FT950::set_notch(bool on, int val)
{
	cmd = "BP00000;";
	if (on) {
		cmd[6] = '1';
		sendCommand(cmd);
		showresp(WARN, ASC, "SET notch on", cmd, replystr);
	} else {
		sendCommand(cmd);
		showresp(WARN, ASC, "SET notch off", cmd, replystr);
	}

	cmd[3] = '1'; // manual NOTCH position
	cmd[6] = '0';
	val /= 10;
	for (int i = 3; i > 0; i--) {
		cmd[3 + i] += val % 10;
		val /=10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET notch val", cmd, replystr);
}

bool  RIG_FT950::get_notch(int &val)
{
	bool ison = false;
	cmd = rsp = "BP00";
	cmd += ';';
	waitN(8, 100, "get notch on/off", ASC);

	size_t p = replystr.rfind(rsp);
	val = progStatus.notch_val = 1500;		// fix default slider position if disabled
	if (p == string::npos) return ison;

	if (replystr[p+6] == '1') { // manual notch enabled
		ison = true;
		val = progStatus.notch_val;
		cmd = rsp = "BP01";
		cmd += ';';
		waitN(8, 100, "get notch val", ASC);
		p = replystr.rfind(rsp);
		if (p == string::npos)
			val = 10;
		else
			val = fm_decimal(&replystr[p+4],3) * 10;
	}
	return ison;
}

void RIG_FT950::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 10;
	max = 3000;
	step = 10;
}

// Terry, KJ4EED
// auto notch NOT replaced with audio peak filter
//
// Lets move APF to CW menu at a later date
// CW menus are only sets?
//
void RIG_FT950::set_auto_notch(int v)
{
// Please save this for now
//	cmd.assign("CO000").append(v ? "2" : "0" ).append(";");
//	sendCommand(cmd);
//	showresp(WARN, ASC, "SET Auto Peak Filter", cmd, replystr);

	cmd.assign("BC0").append(v ? "1" : "0" ).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET DNF Auto Notch Filter", cmd, replystr);
}

int  RIG_FT950::get_auto_notch()
{
//	// Audio Peak Filter
//	cmd = "CO00;";
//	waitN(5, 100, "get Audio Peak Filter", ASC);
//	size_t p = replystr.rfind("CO0");
//	if (p == string::npos) return 0;
//	if (replystr[p+5] == '2') return 1;
//	return 0;

	cmd = "BC0;";
	waitN(5, 100, "get auto notch", ASC);
	size_t p = replystr.rfind("BC0");
	if (p == string::npos) return 0;
	if (replystr[p+3] == '1') return 1;
	return 0;
}

int FT950_blanker_level = 0;

void RIG_FT950::set_noise(bool b)
{
	cmd = "NB00;";
	if (FT950_blanker_level == 0) {
		FT950_blanker_level = 1;
		nb_label("NB 1", true);
	} else if (FT950_blanker_level == 1) {
		FT950_blanker_level = 2;
		nb_label("NB 2", true);
	} else if (FT950_blanker_level == 2) {
		FT950_blanker_level = 0;
		nb_label("NB", false);
	}
	cmd[3] = '0' + FT950_blanker_level;
	sendCommand (cmd);
	showresp(WARN, ASC, "SET NB", cmd, replystr);
}

int RIG_FT950::get_noise()
{
	cmd = rsp = "NB0";
	cmd += ';';
	waitN(5, 100, "get NB", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return FT950_blanker_level;

	FT950_blanker_level = replystr[p+3] - '0';
	if (FT950_blanker_level == 1) {
		nb_label("NB 1", true);
	} else if (FT950_blanker_level == 2) {
		nb_label("NB 2", true);
	} else {
		nb_label("NB", false);
		FT950_blanker_level = 0;
	}
	return FT950_blanker_level;
}

// val 0 .. 100
void RIG_FT950::set_mic_gain(int val)
{
	cmd = "MG000;";
	val = (int)(val * 248 / 100); // magic num tracks radio display value, convert to 0 .. 255
	for (int i = 3; i > 0; i--) {
		cmd[1+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mic", cmd, replystr);
}

int RIG_FT950::get_mic_gain()
{
	cmd = rsp = "MG";
	cmd += ';';
	waitN(6, 100, "get mic", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.mic_gain;
	int val = atoi(&replystr[p+2]);
	val = val * 100 / 248;	// magic num tracks radio display value
	if (val > 100) val = 100;
	return val;
}

void RIG_FT950::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_FT950::set_rf_gain(int val)
{
	cmd = "RG0000;";
	int rfval = val * 250 / 100;
	for (int i = 5; i > 2; i--) {
		cmd[i] = rfval % 10 + '0';
		rfval /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET rfgain", cmd, replystr);
}

int  RIG_FT950::get_rf_gain()
{
	int rfval = 0;
	cmd = rsp = "RG0";
	cmd += ';';
	waitN(7, 100, "get rfgain", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.rfgain;
	for (int i = 3; i < 6; i++) {
		rfval *= 10;
		rfval += replystr[p+i] - '0';
	}
	rfval = rfval * 100 / 250;
	if (rfval > 100) rfval = 100;
	return rfval;
}

void RIG_FT950::get_rf_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_FT950::set_vox_onoff()
{
	cmd = "VX0;";
	if (progStatus.vox_onoff) cmd[2] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox", cmd, replystr);
}

void RIG_FT950::set_vox_gain()
{
	cmd = "VG";
	cmd.append(to_decimal(progStatus.vox_gain, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox gain", cmd, replystr);
}

void RIG_FT950::set_vox_anti()
{
	cmd = "EX117";
	cmd.append(to_decimal(progStatus.vox_anti, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET anti-vox", cmd, replystr);
}

void RIG_FT950::set_vox_hang()
{
	cmd = "VD";
	cmd.append(to_decimal(progStatus.vox_hang, 4)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox delay", cmd, replystr);
}

void RIG_FT950::set_vox_on_dataport()
{
	cmd = "EX1140;";
	if (progStatus.vox_on_dataport) cmd[5] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox on data port", cmd, replystr);
}

void RIG_FT950::set_cw_wpm()
{
	cmd = "KS";
	if (progStatus.cw_wpm > 60) progStatus.cw_wpm = 60;
	if (progStatus.cw_wpm < 4) progStatus.cw_wpm = 4;
	cmd.append(to_decimal(progStatus.cw_wpm, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw wpm", cmd, replystr);
}


void RIG_FT950::enable_keyer()
{
	cmd = "KR0;";
	if (progStatus.enable_keyer) cmd[2] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET keyer on/off", cmd, replystr);
}

bool RIG_FT950::set_cw_spot()
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

void RIG_FT950::set_cw_weight()
{
	int n = round(progStatus.cw_weight * 10);
	cmd.assign("EX046").append(to_decimal(n, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw weight", cmd, replystr);
}

void RIG_FT950::set_cw_qsk()
{
	int n = progStatus.cw_qsk / 5 - 3;
	cmd.assign("EX049").append(to_decimal(n, 1)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw qsk", cmd, replystr);
}

void RIG_FT950::set_cw_spot_tone()
{
	int n = (progStatus.cw_spot_tone - 300) / 50;
	cmd.assign("EX045").append(to_decimal(n, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw tone", cmd, replystr);
}

/*
void RIG_FT950::set_cw_vol()
{
}
*/

void RIG_FT950::set_band_selection(int v)
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

// DNR
void RIG_FT950::set_noise_reduction_val(int val)
{
	cmd.assign("RL0").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_val", cmd, replystr);

}

int  RIG_FT950::get_noise_reduction_val()
{
	int val = 1;
	cmd = "RL0;";
	waitN(6, 100, "GET noise reduction val", ASC);
	size_t p = replystr.rfind("RL0");
	if (p == string::npos) return val;
	val = atoi(&replystr[p+3]);
	return val;
}

// DNR
void RIG_FT950::set_noise_reduction(int val)
{
	if (val > 0)
		cmd = "NR01;";
	else
		cmd = "NR00;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET noise reduction", cmd, replystr);
}


int  RIG_FT950::get_noise_reduction()
{
	int val;
	cmd = "NR0;";
	waitN(5, 100, "GET noise reduction", ASC);
	size_t p = replystr.rfind("NR0");
	if (p == string::npos) return 0;
	val = replystr[p+3] - '0';
	return val;
}
