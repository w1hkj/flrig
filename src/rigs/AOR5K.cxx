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
// AOR 5000 driver based on several other drivers as examples.
//          Written 05/2017 by Mario Lorenz, dl5mlo@amsat-dl.org

#include "AOR5K.h"
#include "status.h"

const char AOR5Kname_[] = "AOR-5000";

const char *AOR5Kmodes_[] = 
	{ "FM", "AM", "LSB", "USB", "CW", "SAM", "SAL", "SAH", NULL};
const char modenbr[] = 
	{ '0', '1', '2', '3', '4', '5', '6', '7' };
static const char AOR5K_mode_type[] =
	{ 'U', 'U', 'L', 'U', 'U', 'U', 'U', 'L' };

static const char *AOR5K_IF_widths[] = {
   "500",  "3000",  "6000",  "15000",  "40000",  "110000",  "220000",  NULL};
static int AOR5K_IF_bw_vals[] = {
   0, 1, 2, 3, 4, 5, 6, WVALS_LIMIT};

static int def_mode_width[] = { 3, 2, 1, 1, 0, 1, 1, 1 };

static const char *AOR5K_SL_label = "HPF";
static const char *AOR5K_SL[] = { "50", "200", "300", "400", NULL };
static const char *AOR5K_SH_label = "LPF";
static const char *AOR5K_SH[] = { "3000", "4000", "6000", "12000", NULL };

static GUI aor5k_widgets[]= {
	{ (Fl_Widget *)btnVol, 2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME, 54, 125, 156 },
//	{ (Fl_Widget *)btnAGC, 2, 145, 50 },
//	{ (Fl_Widget *)sldrSQUELCH, 266, 125, 156 },
//        { (Fl_Widget *)btnNotch, 214, 145,  50 },
//        { (Fl_Widget *)sldrNOTCH, 266, 145, 156 },
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};

RIG_AOR5K::RIG_AOR5K() {
// base class values	
	name_ = AOR5Kname_;
	modes_ = AOR5Kmodes_;
	bandwidths_ = AOR5K_IF_widths;
	bw_vals_ = AOR5K_IF_bw_vals;

	dsp_SL = AOR5K_SL;
	SL_tooltip = AOR5K_SL_label;
	SL_label = AOR5K_SL_label;

	dsp_SH = AOR5K_SH;
	SH_tooltip = AOR5K_SH_label;
	SH_label = AOR5K_SH_label;

	comm_baudrate = BR9600;

	widgets = aor5k_widgets;

	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = false;
	comm_rtsptt = false;
	comm_dtrptt = false;

	def_freq = freqA = freqB = 14070000;
	def_mode = modeA = modeB = 1;
	def_bw = bwA = bwB = 1;

	has_volume_control =
	has_mode_control =
	has_bandwidth_control =
	has_noise_control =
	has_attenuator_control =
	has_agc_control =
	has_smeter =
	has_agc_level =	
	has_vfoAB =
	can_change_alt_vfo =
	has_dsp_controls =
 true;
	has_notch_control =
	has_sql_control =

	has_xcvr_auto_on_off =
	has_a2b =
	has_b2a =
	has_vfo_adj =
	has_rit = 
	has_xit =
	has_bfo =
	has_power_control =
	has_micgain_control =
	has_mic_line_control =
	has_auto_notch =
	has_noise_reduction_control =
	has_noise_reduction =
	has_preamp_control = 
	has_ifshift_control = 
	has_ptt_control =
	has_tune_control =
	has_swr_control =
	has_alc_control =
	has_rf_control =
	has_power_out =
	has_split =
	has_split_AB =
	has_data_port =
	has_getvfoAorB =

	has_extras = 
	has_nb_level =
	has_cw_wpm =
	has_cw_vol =
	has_cw_spot =
	has_cw_spot_tone =
	has_cw_qsk = 
	has_cw_break_in =
	has_cw_delay =
	has_cw_weight =
	has_cw_keyer = 
	has_vox_onoff =
	has_vox_gain =
	has_vox_anti =
	has_vox_hang =
	has_vox_on_dataport =
	has_compression =
	has_compON =
	use_line_in =
	has_bpf_center =
	has_special =
	has_ext_tuner =
	has_band_selection =  false;

	precision = 1;
	ndigits = 10;
	atten_level = 0;
	agcval = 0;
}

int  RIG_AOR5K::adjust_bandwidth(int m)
{
	return def_mode_width[m];
}

int  RIG_AOR5K::def_bandwidth(int m)
{
	return def_mode_width[m];
}

#define AOR5K_WAIT_TIME 800


void RIG_AOR5K::initialize()
{
	debug::level = debug::INFO_LEVEL;

	LOG_INFO("AOR5K");
	aor5k_widgets[0].W = btnVol;
	aor5k_widgets[1].W = sldrVOLUME;
//	aor5k_widgets[2].W = btnAGC;

//	aor5k_widgets[3].W = sldrSQUELCH;
//	aor5k_widgets[4].W = btnNotch;
//	aor5k_widgets[5].W = sldrNOTCH;
//	aor5k_widgets[6].W = sldrPOWER;

//	cmd = "AI0;"; // disable auto-info
//	sendCommand(cmd);
//	showresp(INFO, ASC, "disable auto-info", cmd, replystr);

	get_vfoA();
	get_modeA();
	get_bwA();

	get_vfoB();
	get_modeB();
	get_bwB();

	set_split(false); // normal ops

}

void RIG_AOR5K::shutdown()
{
}

void RIG_AOR5K::selectA() {
	cmd = "VE\r";
	wait_char('\r', 1, AOR5K_WAIT_TIME, "Select VFO A", ASC);
}
void RIG_AOR5K::selectB() {
	cmd = "VB\r";
	wait_char('\r', 1, AOR5K_WAIT_TIME, "Select VFO B", ASC);
}

bool RIG_AOR5K::check()
{
	cmd = "RX\r";
	int ret = wait_char('\r', 34, AOR5K_WAIT_TIME, "get vfo A", ASC);
	if (ret < 34) return false;
	return true;
}

long RIG_AOR5K::get_vfoA ()
{
	cmd = "RX\r";
	int ret = wait_char('\r', 34, AOR5K_WAIT_TIME, "get vfo A", ASC);
	if (ret < 34) return freqA;
	size_t p = replystr.rfind("RF");
	if (p == string::npos) return freqA;
	if (p < 3) return freqA;
	if (replystr[p-2] =='E') {
		// VFO A is active. Instead of A we use E
		long f = 0;
		for (size_t n = 2; n < 12; n++)
			f = f*10 + replystr[p + n] - '0';
		freqA = f;
	}
	return freqA;
}

void RIG_AOR5K::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "VE0000000000\r";
	for (int i = 11; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	wait_char('\r', 1, AOR5K_WAIT_TIME, "set VFO A", ASC);
}

long RIG_AOR5K::get_vfoB ()
{
	cmd = "RX\r";
	int ret = wait_char('\r', 34, AOR5K_WAIT_TIME, "get vfo B", ASC);
	if (ret < 34) return freqB;
	size_t p = replystr.rfind("RF");
	if (p == string::npos) return freqB;
	if (p < 3) return freqB;
	if (replystr[p-2] == 'B') {
		// VFO B active
		long f = 0;
		for (size_t n = 2; n < 12; n++)
			f = f*10 + replystr[p + n] - '0';
		freqB = f;
	}
	return freqB;
}

void RIG_AOR5K::set_vfoB (long freq)
{
	freqB = freq;
	cmd = "VB0000000000\r";
	for (int i = 11; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	wait_char('\r', 1, AOR5K_WAIT_TIME, "set VFO B", ASC);
}

// Volume control
void RIG_AOR5K::set_volume_control(int val) 
{
	cmd = "VL000\r";
	for (int i = 4; i > 1; i--) {
		cmd[i] += val % 10;
		val /= 10;
	}
	wait_char('\r', 1, AOR5K_WAIT_TIME, "set Vol", ASC);
}

int RIG_AOR5K::get_volume_control()
{
	cmd = "VL\r";
	int ret = wait_char('\r', 5, AOR5K_WAIT_TIME, "get volume", ASC);
	if (ret < 5) return progStatus.volume;
	size_t p = replystr.rfind("VL");
	if (p == string::npos) return 0;

	replystr[p + 5] = 0;
	int v = atoi(&replystr[p + 2]);
	return v;
}

void RIG_AOR5K::get_vol_min_max_step(int &min, int &max, int &step) {
                min = 0; max = 255; step = 1;
}

void RIG_AOR5K::set_modeA(int val)
{
	modeA = val;
	cmd = "MD0\r";
	cmd[2] = modenbr[val];
	wait_char('\r', 1, AOR5K_WAIT_TIME, "set mode A", ASC);
	//sendCommand(cmd);
	//showresp(INFO, ASC, "set mode A", cmd, replystr);
	//cmd="RX\r";
	
}

int RIG_AOR5K::get_modeA()
{
	cmd = "MD\r";
	int ret = wait_char('\r', 4, AOR5K_WAIT_TIME, "get mode A", ASC);
	if (ret < 4) return modeA;
	size_t p = replystr.rfind("MD");
	if (p == string::npos) return modeA;
	int md = replystr[p + 2] - '0';
	if (md > 8) md=0;
	return (modeA = md);
}

void RIG_AOR5K::set_modeB(int val)
{
	modeB = val;
	set_modeA(val); // Fake ModeB -- mode setting is not per VFO
}

int RIG_AOR5K::get_modeB()
{
	// Fake ModeB
	return (modeB = get_modeA());
}

int RIG_AOR5K::get_modetype(int n)
{
	return AOR5K_mode_type[n];
}

int  RIG_AOR5K::next_attenuator()
{
	switch (atten_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 22;
		case 22: return 0;
	}
	return 0;
}

void RIG_AOR5K::set_attenuator(int val)
{
	atten_level = val;
	switch (atten_level) {
		case 1:
			cmd = "AT1\r";
			atten_label("10 dB", true);
			break;
		case 2:
			cmd = "AT2\r";
			atten_label("20 dB", true);
			break;
		case 22:
			cmd = "ATF\r";
			atten_label("AUTO", true);
			break;
		case 0:
		default:
			cmd = "AT0\r";
			atten_label("0 dB", false);
			break;
	}
	wait_char('\r', 1, AOR5K_WAIT_TIME, "set BW A", ASC);
}

int RIG_AOR5K::get_attenuator()
{
	cmd = "AT\r";
	int ret = wait_char('\r', 4, AOR5K_WAIT_TIME, "get ATT", ASC);
	if (ret < 4) return atten_level;
	size_t p = replystr.rfind("AT");
	if (p == string::npos) return atten_level;
	
	if (replystr[p+2] == '1')
		atten_level = 22;
	else atten_level = replystr[p + 3] - '0';

	switch (atten_level) {
		case 0:	atten_label("0 dB", false); break;
		case 1: atten_label("10 dB", true); break;
		case 2: atten_label("20 dB", true); break;
		case 22: atten_label("AUTO", true); break;
	}
	return atten_level;
}

//SM $ (S-meter Read; GET only)
int RIG_AOR5K::get_smeter()
{
	cmd = "LM\r";
	int ret = wait_char('\r', 7, AOR5K_WAIT_TIME, "get Smeter", ASC);
	if (ret < 6) return 0;
	size_t p = replystr.rfind("LM");
	if (p == string::npos) return 0;
	
	replystr[p+5] ='\0';	


	int mtr = strtoul(&replystr[p+3], NULL, 16);  // encoded in hex. S-Meter mapping TBD

	return int(floor(float(mtr)/2.55));  // 0...100 is legal range
}

void RIG_AOR5K::set_noise(bool on)
{
	if (on) 
	   cmd = "NB1\r";
	else
	   cmd = "NB0\r";
	wait_char('\r', 1, AOR5K_WAIT_TIME, "set Noise Blanker", ASC);
}

int RIG_AOR5K::get_noise()
{
	cmd = "NB\r";
	int ret = wait_char('\r', 4, AOR5K_WAIT_TIME, "get Noise Blanker", ASC);
	if (ret < 4) return progStatus.noise;
	size_t p = replystr.rfind("NB");
	if (p == string::npos) return progStatus.noise;
	return (replystr[p+2] == '1' ? 1 : 0);
}

// FW $ (Filter Bandwidth and Number; GET/SET)
// K3 Extended SET/RSP format (K31): FWxxxx; where xxxx is 0-9999, the bandwidth
// in 10-Hz units. May be quantized and/or range limited based on the present 
// operating mode.

void RIG_AOR5K::set_bwA(int val)
{
	cmd = "BW0\r";
	bwA = val;
	cmd[2] = '0' + val;
	wait_char('\r', 4, AOR5K_WAIT_TIME, "set BW A", ASC);
}

int RIG_AOR5K::get_bwA()
{
	cmd = "BW\r";
	int ret = wait_char('\r', 4, AOR5K_WAIT_TIME, "get bandwidth A", ASC);
	if (ret < 4) return bwA;
	size_t p = replystr.rfind("BW");
	if (p == string::npos) return bwA;
	return (bwA = replystr[p+2] - '0');
}

void RIG_AOR5K::set_bwB(int val)
{
	bwB = val;
	set_bwA(val);
}

int RIG_AOR5K::get_bwB()
{
	return (bwB = get_bwA());
}


int RIG_AOR5K::get_agc() {
	cmd = "AC\r";
	int ret = wait_char('\r', 4, AOR5K_WAIT_TIME, "get AGC", ASC);
	if (ret < 4) return agcval;
	size_t p = replystr.rfind("AC");
	if (p == string::npos) return agcval;
	int agci = replystr[p+2] - '0';
	switch (agci) {
		case 0: agcval = 1; break;
		case 1: agcval = 2; break;
		case 2: agcval = 3 ; break;
		case 22: agcval = 0; break;	
	}
	return agcval;
}

int RIG_AOR5K::incr_agc() {
	agcval++;
	if (agcval >= 4) agcval = 0;
	switch(agcval) {
		case 0: cmd = "ACF\r"; break;
		case 1: cmd = "AC0\r"; break;
		case 2: cmd = "AC1\r"; break;
		case 3: cmd = "AC2\r"; break;
	}
	wait_char('\r', 1, AOR5K_WAIT_TIME, "set AGC", ASC);
	return agcval;
}
static const char *agcstrs[] = {"AGC", "AGF" , "AGM", "AGS"};
const char* RIG_AOR5K::agc_label() {
	return agcstrs[agcval];
}
int RIG_AOR5K::agc_val() {
	return agcval;
}

