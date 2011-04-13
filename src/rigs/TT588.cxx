/*
 * TenTec Omni-VII (TT588) drivers
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

/*
 * Note for anyone wishing to expand on the command set.
 *
 * A rejected command is responded to by a three character sequence "Zy\r".
 * where the y is the letter of the command in error
 * you need to check for that response
 *
*/

#include "TT588.h"
#include "support.h"
#include "math.h"

static const char TT588name_[] = "Omni-VII";

//static const char *TT588modes_[] = { "D-USB", "USB", "LSB", "CW", "AM", "FM", NULL}
//static const char TT588mode_chr[] =  { '1', '1', '2', '3', '0', '4' };
//static const char TT588mode_type[] = { 'U', 'U', 'L', 'L', 'U', 'U' };
static const char *TT588modes_[] = {
		"AM", "USB", "LSB", "CWU", "FM", "CWL", "FSK", NULL};
static const char TT588mode_chr[] =  { '0', '1', '2', '3', '4', '5', '6' };
static const char TT588mode_type[] = { 'U', 'U', 'L', 'U', 'U', 'L', 'L' };

// filter # is 37 - index
static const char *TT588_widths[] = {
"200",   "250",  "300",  "350",  "400",  "450",  "500",  
"600",  "700",  "800", "900",  "1000", 
"1200", "1400", "1600", "1800", "2000", "2200", "2400", 
"2500", "2600", 
"2800", "3000", "3200", "3400", "3600", "3800", "4000", 
"4500", "5000", "5500", "6000", "6500", "7000", "7500", "8000", 
"9000", "12000", NULL};

static const int TT588_numeric_widths[] = {
200,   250,  300,  350,  400,  450,  500,
600,  700,  800,  900,  1000, 
1200, 1400, 1600, 1800, 2000, 2200, 2400, 
2500, 2600, 
2800, 3000, 3200, 3400, 3600, 3800, 4000,
4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000,
9000, 12000, NULL};


static char TT588setFREQA[]		= "*Annnn\r";
//static char TT588setFREQB[]		= "*Bnnnn\r";
//static char TT588setAGC[]		= "*Gn\r";
//static char TT588setSQLCH[]		= "*Hc\r";
static char TT588setRF[]		= "*Ic\r";
static char TT588setATT[]		= "*Jc\r";
//static char TT588setNB[]		= "*Knar\r";
static char TT588setMODE[]		= "*Mnn\r";
static char TT588setPBT[]		= "*Pxx\r";
static char TT588setVOL[]		= "*Un\r";
static char TT588setBW[]		= "*Wx\r";

static char TT588getFREQA[]		= "?A\r";
//static char TT588getAGC[]		= "?G\r";
//static char TT588getSQLCH[]	= "?H\r";
//static char TT588getRF[]		= "?I\r";
static char TT588getATT[]		= "?J\r";
//static char TT588getNB[]		= "?K\r";
static char TT588getMODE[]		= "?M\r";
static char TT588getPBT[]		= "?P\r";
static char TT588getSMETER[]	= "?S\r";
static char TT588getVOL[]		= "?U\r";
static char TT588getBW[]		= "?W\r";

static char TT588getFWDPWR[]	= "?F\r";

//static char TT588getREFPWR[]	= "?R\r";

static char TT588setXMT[]		= "*Tnn\r";

RIG_TT588::RIG_TT588() {
// base class values
	name_ = TT588name_;
	modes_ = TT588modes_;
	bandwidths_ = TT588_widths;
	comm_baudrate = BR57600;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 20;
	comm_timeout = 50;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;// false;
	comm_rtsptt = false;
	comm_dtrptt = false;
	serloop_timing = 200;

	modeA = 1;
	bwA = 25;
	def_mode = 3;
	defbw_ = 25;
	deffreq_ = 14070000;
	max_power = 100;
	pbt = 0;
	VfoAdj = progStatus.vfo_adj;
	vfo_corr = 0;

	has_bpf_center =
	has_power_control =
	has_micgain_control =
	has_notch_control =
	has_preamp_control =
	has_tune_control =
	has_noise_control =
	has_swr_control = 
	has_vfo_adj = false;

 
	has_volume_control =
	has_rf_control =
	has_attenuator_control =
	has_ifshift_control =
	has_ptt_control =
	has_bandwidth_control =
	has_mode_control = true;

}

void RIG_TT588::checkresponse(string s)
{
	if (RigSerial.IsOpen() == false)
		return;
	LOG_ERROR("%s:\nsent  %s\nreply %s\n", s.c_str(),
		str2hex(cmd.c_str(), cmd.length()),
		str2hex((char *)replybuff, strlen((char *)replybuff)));
}

void RIG_TT588::showresponse(string s)
{
	printf("%s: %s\n", s.c_str(),str2hex((char *)replybuff, strlen((char *)replybuff)));
}

void RIG_TT588::initialize()
{
	VfoAdj = progStatus.vfo_adj;
}

void RIG_TT588::shutdown()
{
	set_if_shift(0);
}

long RIG_TT588::get_vfoA ()
{
	cmd = TT588getFREQA;
	bool ret = sendCommand(cmd, 6, true);
	if (ret == true && replybuff[0] == 'A') {
		int f = 0;
		for (size_t n = 1; n < 5; n++)
			f = f*256 + (unsigned char)replybuff[n];
		freqA = f;
	}
	return (long)(freqA - vfo_corr);
}

void RIG_TT588::set_vfoA (long freq)
{
	freqA = freq;
	vfo_corr = (freq / 1e6) * VfoAdj + 0.5;
	long xfreq = freqA + vfo_corr;
	cmd = TT588setFREQA;
	cmd[5] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[4] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[3] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[2] = xfreq & 0xff;
	sendCommand(cmd, 0, true);
	set_if_shift(pbt);
	return ;
}

void RIG_TT588::setVfoAdj(double v)
{
	VfoAdj = v;
}

void RIG_TT588::set_modeA(int val)
{
	modeA = val;
	cmd = TT588setMODE;
	cmd[2] = cmd[3] = TT588mode_chr[val];
	sendCommand(cmd, 0, true);
}

int RIG_TT588::get_modeA()
{
	cmd = TT588getMODE;
	sendCommand(cmd, 4, true);
	if (replybuff[0] == 'M') {
		modeA = replybuff[1] - '0';
	}
	return modeA;
}

int RIG_TT588::get_modetype(int n)
{
	return TT588mode_type[n];
}

void RIG_TT588::set_bwA(int val)
{
	bwA = val;
	cmd = TT588setBW;
	cmd[2] = 37 - val;
	sendCommand(cmd, 0, true);
	set_if_shift(pbt);
}

int RIG_TT588::get_bwA()
{
	cmd = TT588getBW;
	sendCommand(cmd, 3, true);
	if (replybuff[0] == 'W')
		bwA = 37 - (unsigned char)replybuff[1];
	return bwA;
}

int  RIG_TT588::adjust_bandwidth(int m)
{
	if (m == 0) return 31;
	if (m == 1 || m == 2) return 22;
	if (m == 3 || m == 5) return 7;
	if (m == 4 || m == 6) return 22;
	return 22;
}

void RIG_TT588::set_if_shift(int val)
{
	pbt = val;
	cmd = TT588setPBT;
	int bpval = progStatus.bpf_center - 200 - TT588_numeric_widths[bwA]/2;
	short int si = val;
	if ((modeA == 1 || modeA == 2) && progStatus.use_bpf_center)
		si += (bpval > 0 ? bpval : 0);
	cmd[2] = (si & 0xff00) >> 8;
	cmd[3] = (si & 0xff);
	sendCommand(cmd, 0, true);
	sendCommand(TT588getPBT, 4, true);
	if (replybuff[1] != cmd[2] || replybuff[2] != cmd[3]) {
		sendCommand(cmd, 0, true);
	}
}

bool RIG_TT588::get_if_shift(int &val)
{
	val = 0;
	cmd = TT588getPBT;
	sendCommand(cmd, 4, true);
	return false;
}

void RIG_TT588::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -2000;
	max = 2000;
	step = 10;
}

void RIG_TT588::set_attenuator(int val)
{
	cmd = TT588setATT;
	if (val) cmd[2] = '1';
	else     cmd[2] = '0';
	sendCommand(cmd, 0, true);
}


int RIG_TT588::get_attenuator()
{
	cmd = TT588getATT;
	sendCommand(cmd, 3, true);
	if (replybuff[0] == 'J' && replybuff[1] == '1')
		return 1;
	return 0;
}

int RIG_TT588::get_smeter()
{
	int sval = 0;
	float fval = 0;
	cmd = TT588getSMETER;
	sendCommand(cmd, 6, true);
	if (replybuff[0] == 'S') {
		sscanf(&replybuff[1], "%4x", &sval);
		fval = sval/256.0;
		sval = (int)(fval * 100.0 / 18.0);
		if (sval > 100) sval = 100;
	}
	return (sval);
}

int RIG_TT588::get_volume_control()
{
	cmd = TT588getVOL;
	sendCommand(cmd, 3, true);
	if (replybuff[0] == 'U')
		return (int)((replybuff[1] & 0x7F) / 1.27);
	return 0;
}

void RIG_TT588::set_volume_control(int vol)
{
	cmd = TT588setVOL;
	cmd[2] = 0x7F & (int)(vol * 1.27);
	sendCommand(cmd, 0, true);
}

void RIG_TT588::set_rf_gain(int val)
{
	cmd = TT588setRF;
	cmd[2] = 0x7F & (int)(val * 1.27);
	sendCommand(cmd, 0, true);
}

int  RIG_TT588::get_rf_gain()
{
	return 100; 
// Omni-VII does not reply with values as specified in the programmers manual
}

// Tranceiver PTT on/off

int query_cnt = 0;
void RIG_TT588::set_PTT_control(int val)
{
	if (val) query_cnt = 2000 / progStatus.serloop_timing; // every 2 seconds
	cmd = TT588setXMT;
	if (val) {
		cmd[2] = 0x04;
		cmd[3] = 0;
	} else {
		cmd[2] = 0;
		cmd[3] = 0;
	}
	sendCommand(cmd, 0, true);
}

int RIG_TT588::get_power_out()
{
	query_cnt--;
	if (!query_cnt)
		set_PTT_control(1);
	cmd = TT588getFWDPWR;
	sendCommand(cmd, 6, true);
	fwdpwr = refpwr = 0;
	if (replybuff[0] == 'F') {
		fwdpwr = replybuff[1] & 0x7F;
		refpwr = replybuff[2];
	}
	fwdv = sqrtf(fwdpwr);
	refv = sqrtf(refpwr);
	return fwdpwr;
}

