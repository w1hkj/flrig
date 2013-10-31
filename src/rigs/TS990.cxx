/*
 * Kenwood TS990 driver
 *
 * a part of flrig
 *
 * Copyright 2013
 *     Dave Freese, W1HKJ
 *     Andy Burnett, G0HIX
 *
 */

#include "config.h"

#include "TS990.h"
#include "support.h"

static const char TS990name_[] = "TS-990";

static const char *TS990modes_[] = {
"LSB",    "USB",    "CW",    "FM",     "AM",
"FSK",    "PSK",    "CW-R",  "FSK-R",  "PSK-R",
"LSB-D1", "USB-D1", "FM-D1", "AM-D1",
"LSB-D2", "USB-D2", "FM-D2",  "AM-D2",
"LSB-D3", "USB-D3", "FM-D3",  "AM-D3",  NULL};

static const char TS990_mode_chr[] =  {
'1', '2', '3', '4', '5',
'6', 'A', '7', '9', 'B',
'C', 'D', 'E', 'F',
'G', 'H', 'I', 'J',
'K', 'L', 'M', 'N' };

static const char TS990_mode_type[] = {
'L', 'U', 'U', 'U', 'U',
'U', 'U', 'L', 'U', 'U',
'L', 'U', 'U', 'U',
'L', 'U', 'U', 'U',
'L', 'U', 'U', 'U' };

//----------------------------------------------------------------------
static const char *TS990_empty[] = { "N/A", NULL };

//======================================================================
static const char *TS990_SSB_widths[] = {
  "50",   "80",  "100",  "150",  "200",
 "250",  "300",  "400",  "500",  "600",
"1000", "1500", "2000", "2200", "2400",
"2600", "2800", "3000", NULL };

static const char *TS990_CAT_ssb_widths[] = {
"00;", "01;", "02;", "03;", "04;",
"05;", "06;", "07;", "08;", "09;",
"10;", "11;", "12;", "13;", "14;",
"15;", "16;", "17;", NULL };

static const char *TS990_SSB_W_tooltip = "filter width";
static const char *TS990_SSB_W_btn_label = "W";

static const char *TS990_SSB_shift[] = {
"1000", "1100", "1200", "1300", "1400",
"1500", "1600", "1700", "1800", "1900",
"2000", "2100", "2210", NULL };

static const char *TS990_CAT_ssb_shift[] = {
"00;", "01;", "02;", "03;", "04;",
"05;", "06;", "07;", "08;", "09;",
"10;", "11;", "12;", NULL };

static const char *TS990_SSB_S_tooltip = "shift frequency";
static const char *TS990_SSB_S_btn_label = "S";

//======================================================================
// OTHER Modes
//======================================================================

static const char *TS990_OTHER_hi[] = {
"1000", "1200", "1400", "1600", "1800",
"2000", "2200", "2400", "2600", "2800",
"3000", "3400", "4000", "5000", NULL };

static const char *TS990_CAT_other_hi[] = {
"00;", "01;", "02;", "03;", "04;",
"05;", "06;", "07;", "08;", "09;",
"10;", "11;", "12;", "13;", NULL };

static const char *TS990_OTHER_hi_tooltip = "hi cutoff";
static const char *TS990_OTHER_btn_hi_label = "H";

static const char *TS990_OTHER_lo[] = {
"0", "50", "100", "200", "300",
"400", "500", "600", "700", "800",
"900", "1000", NULL };

static const char *TS990_CAT_other_lo[] = {
"00;", "01;", "02;", "03;", "04;",
"05;", "06;", "07;", "08;", "09;",
"10;", "11;", NULL };

static const char *TS990_OTHER_lo_tooltip = "lo cutoff";
static const char *TS990_OTHER_btn_lo_label = "L";

//======================================================================

static const char *TS990_CW_widths[] = {
  "50",   "80",  "100",  "150", "200",
 "250",  "300",  "400",  "500", "600",
"1000", "1500", "2000", "2500",  NULL };

static const char *TS990_CAT_cw_widths[] = {
"00;", "01;", "02;", "03;", "04;",
"05;", "06;", "07;", "08;", "09;",
"10;", "11;", "12;", "13;", NULL };

static const char *TS990_CW_W_tooltip = "filter width";
static const char *TS990_CW_W_btn_label = "W";

static const char *TS990_CW_shift[] = {
"-800", "-750", "-700", "-650", "-600", "-550", "-500", "-450", "-400",
 "-350", "-300", "-250", "-200", "-150", "-100", "-50", "0",
"50", "100", "150", "200", "250", "300", "350", "400", "450", "500",
"550", "600", "650", "700", "750", "800", NULL };

static const char *TS990_CAT_cw_shift[] = {
"00;", "01;", "02;", "03;", "04;", "05;", "06;",
"07;", "08;", "09;", "10;", "11;", "12;","13;",
"14;", "15;", "16;", "17;", "18;", "19;", "20;",
"21;", "22;","23;", "24;", "25;","26;", "27;",
"28;", "29;", "30;", "31;", "32;", NULL };

static const char *TS990_CW_S_tooltip = "shift frequency";
static const char *TS990_CW_S_btn_label = "S";


//======================================================================
static const char *TS990_AM_lo[] = {
"0", "100", "200", "300", NULL };

static const char *TS990_CAT_am_lo[] = {
"00;", "01;", "02;", "03;", NULL};

static const char *TS990_AM_lo_tooltip = "lo cutoff";
static const char *TS990_AM_btn_lo_label = "L";

static const char *TS990_AM_hi[] = {
"2500", "3000", "4000", "5000", NULL };

static const char *TS990_CAT_am_hi[] = {
"00;", "01;", "02;", "03;", NULL};

static const char *TS990_AM_hi_tooltip = "hi cutoff";
static const char *TS990_AM_btn_hi_label = "H";
//----------------------------------------------------------------------


//======================================================================

static const char *TS990_FSKwidths[] = {
"250", "300", "400", "500", "1000", "1500", NULL};

static const char *TS990_FSKbw[] = {
"00;", "01;", "02;", "03;", "04;", "05;", NULL };

//======================================================================

static const char *TS990_PSKwidths[] = {
"50",   "80",  "100",  "150", "200", "250",  "300",  "400",  "500", "600",
"1000", "1500", NULL};

static const char *TS990_PSKbw[] = {
"00;", "01;", "02;", "03;", "04;",
"05;", "06;", "07;", "08;", "09;",
"10;", "11;", NULL };

//======================================================================

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 },
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)btnNotch,    214, 125,  50 },
	{ (Fl_Widget *)sldrNOTCH,   266, 125, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 145, 156 },
	{ (Fl_Widget *)sldrPOWER,   266, 165, 156 },
	{ (Fl_Widget *)btnNR,         2, 165,  50 },
	{ (Fl_Widget *)sldrNR,       54, 165, 156 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

void RIG_TS990::initialize()
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
	rig_widgets[9].W = btnNR;
	rig_widgets[10].W = sldrNR;

	RIG_DEBUG = true;
		cmd = "AC000;"; sendCommand(cmd);
	selectA();
	get_preamp();
	get_attenuator();
	check_menu_0607();
	check_menu_0608();
	

}
//======================================================================
//======================================================================

RIG_TS990::RIG_TS990() {

	name_ = TS990name_;
	modes_ = TS990modes_;
	bandwidths_ = TS990_empty;

	dsp_lo     = TS990_OTHER_lo;
	lo_tooltip = TS990_OTHER_lo_tooltip;
	lo_label   = TS990_OTHER_btn_lo_label;

	dsp_hi     = TS990_OTHER_hi;
	hi_tooltip = TS990_OTHER_btn_hi_label;
	hi_label   = TS990_OTHER_btn_hi_label;

	widgets = rig_widgets;

	comm_baudrate = BR115200;
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

	B.imode = A.imode = 1;
	B.iBW = A.iBW = 0x8903;
	B.freq = A.freq = 14107500;
	can_change_alt_vfo = true;

	nb_level = 1;


	has_auto_notch =
	has_notch_control =
	has_sql_control =
	has_swr_control =
	has_noise_reduction =
	has_noise_reduction_control =
	has_alc_control =
	has_dsp_controls =
	has_smeter =
	has_power_out =
	has_split =
	has_split_AB =
	has_noise_control =
	has_micgain_control =
	has_rf_control =
	has_volume_control =
	has_power_control =
	has_tune_control =
	has_attenuator_control =
	has_preamp_control =
	has_mode_control =
	has_bandwidth_control =
	has_ifshift_control =
	has_ptt_control = true;

	rxtxa = true;

	precision = 1;
	ndigits = 8;

}

//======================================================================
//======================================================================

void RIG_TS990::selectA()
{
	cmd = "CB0;";
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "Rx A, Tx A", cmd, replystr);
	rxtxa = true;
	get_attenuator();
	get_preamp();
	get_smeter();
	get_power_out();
	get_volume_control();
	get_rf_gain();
	get_noise_reduction_val();
	get_auto_notch();
	get_agc();
}

void RIG_TS990::selectB()
{
	cmd = "CB1;";
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "Rx B, Tx B", cmd, replystr);
	rxtxa = false;
	get_attenuator();
	get_preamp();
	get_smeter();
	get_power_out();
	get_volume_control();
	get_rf_gain();
	get_noise_reduction_val();
	get_auto_notch();
	get_agc();


}

//======================================================================
//	Get Attenuator
// The TS990 actually has 4 attenuator settings.
// RA00; = Off,  RA01; = 6dB,  RA02; = 12dB,  RA03; = 18dB
void RIG_TS990::set_attenuator(int val) {

	if (useB) {

	if (att_level == 0) {			// If attenuator level = 0 (off)
		att_level = 1;				// then turn it on, at 6dB
		cmd = "RA11;";				// this is the command...
		atten_label("Att 6", true);	// show it in the button...
	}
	else if (att_level == 1) {		// If attenuator level = 1 (6dB)
		att_level = 2;				// then make it 12dB
		cmd = "RA12;";
		atten_label("Att 12", true);
	}
	else if (att_level == 2) {		// if it's 12dB
		att_level = 3;				// go to 18dB
		cmd = "RA13;";
		atten_label("Att 18", true);
	}
	else if (att_level == 3) {		// If it's 18dB
		att_level = 0;				// Loop back to none.
		cmd = "RA10;";
		atten_label("Att", false);
	}

	sendCommand(cmd);
	showresp(WARN, ASC, "set Att B", cmd, replystr);

	} else {

	if (att_level == 0) {
		att_level = 1;
		cmd = "RA01;";
		atten_label("Att 6", true);
	}
	else if (att_level == 1) {
		att_level = 2;
		cmd = "RA02;";
		atten_label("Att 12", true);
	}
	else if (att_level == 2) {
		att_level = 3;
		cmd = "RA03;";
		atten_label("Att 18", true);
	}
	else if (att_level == 3) {
		att_level = 0;
		cmd = "RA00;";
		atten_label("Att", false);
	}

	sendCommand(cmd);
	showresp(WARN, ASC, "set Att A", cmd, replystr);

}
}

//----------------------------------------------------------------------
//	Modified to read and show the actual radio setting, in the button.
//----------------------------------------------------------------------
int RIG_TS990::get_attenuator() {

	if (useB) {

	cmd = "RA1;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get Att B", cmd, replystr);
	if (ret < 5) return att_on;
	size_t p = replystr.rfind("RA");
	//if (p == string::npos) return att_on;

	if (replystr[p + 2] == '1' && replystr[p + 3] == '0') {
		att_on = 0;						// Attenuator is OFF
		att_level = 0;					// remember it...
		atten_label("Att", false);		// show it...
	}
	else if (replystr[p + 2] == '1' && replystr[p + 3] == '1') {
		att_on = 1;						// Attenuator is ON, 6dB
		att_level = 1;					// remember the level
		atten_label("Att 6", true);		// show it...
	}
	else if (replystr[p + 2] == '1' && replystr[p + 3] == '2') {
		att_on = 1;						// .. still ON, 12dB
		att_level = 2;					// remember this level
		atten_label("Att 12", true);	// show it.
	}
	else if (replystr[p + 2] == '1' && replystr[p + 3] == '3') {
		att_on = 1;						// .. still ON 18dB
		att_level = 3;					// remember...
		atten_label("Att 18", true);	// show this too..


	return att_on;			// let the rest of the world know.
}

	} else {


	cmd = "RA0;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get Att A", cmd, replystr);
	if (ret < 5) return att_on;
	size_t p = replystr.rfind("RA");
	//if (p == string::npos) return att_on;

	if (replystr[p + 2] == '0' && replystr[p + 3] == '0') {
		att_on = 0;
		att_level = 0;
		atten_label("Att", false);
	}
	else if (replystr[p + 2] == '0' && replystr[p + 3] == '1') {
		att_on = 1;
		att_level = 1;
		atten_label("Att 6", true);
	}
	else if (replystr[p + 2] == '0' && replystr[p + 3] == '2') {
		att_on = 1;
		att_level = 2;
		atten_label("Att 12", true);
	}
	else if (replystr[p + 2] == '0' && replystr[p + 3] == '3') {
		att_on = 1;
		att_level = 3;
		atten_label("Att 18", true);

}
}
	return att_on;

}


//----------------------------------------------------------------------
//Get PreAmp
//----------------------------------------------------------------------

void RIG_TS990::set_preamp(int val)
{

	if (useB) {

	preamp_level = val;
	if (val) cmd = "PA11;";
	else     cmd = "PA10;";
	sendCommand(cmd, 0);


	}else {

	preamp_level = val;
	if (val) cmd = "PA01;";
	else     cmd = "PA00;";
	sendCommand(cmd, 0);
}
}

int RIG_TS990::get_preamp()
{
	if (useB) {

	cmd = "PA1;";
	int ret = waitN(5, 20, "get preamp", ASC);

	if (ret < 5) return 0;
	size_t p = replystr.rfind("PA");
	if (p == string::npos) return 0;

	if (replystr[p  + 3] == '1')
		preamp_level = 1;
	else
		preamp_level = 0;
	return preamp_level;

} else {

	cmd = "PA0;";
	int ret = waitN(5, 20, "get preamp", ASC);

	if (ret < 5) return 0;
	size_t p = replystr.rfind("PA");
	if (p == string::npos) return 0;

	if (replystr[p  + 3] == '1')
		preamp_level = 1;
	else
		preamp_level = 0;
	return preamp_level;
}

}

//----------------------------------------------------------------------

void RIG_TS990::set_split(bool val)
{
	split = val;
	if (useB) {
		if (val) {
			cmd = "MV10;TB0;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B, Tx on A", cmd, replystr);
		} else {
			cmd = "MV10;TB1;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B, Tx on B", cmd, replystr);
		}
	} else {
		if (val) {
			cmd = "MV00;TB1;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A, Tx on B", cmd, replystr);
		} else {
			cmd = "MV00;TB0;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A, Tx on A", cmd, replystr);
		}
	}
	Fl::awake(highlight_vfo, (void *)0);
}


int RIG_TS990::get_split()
{
	size_t p;
	int split = 0;
	char rx, tx;
// tx vfo
	cmd = rsp = "TB";
	cmd.append(";");
	waitN(4, 20, "get split tx vfo", ASC);
	p = replystr.rfind(rsp);
	if (p == string::npos) return split;
	tx = replystr[p+2];

// rx vfo
	cmd = rsp = "CB";
	cmd.append(";");
	waitN(4, 20, "get split rx vfo", ASC);

	p = replystr.rfind(rsp);
	if (p == string::npos) return split;
	rx = replystr[p+2];
 //split test
	split = (tx == '1' ? 2 : 0) + (rx == '1' ? 1 : 0);
	
	return split;
}
//----------------------------------------------------------------------
const char * RIG_TS990::get_bwname_(int n, int md)
{
	static char bwname[20];
	if (n > 256) {
		int hi = (n >> 8) & 0x7F;
		int lo = n & 0x7F;
		snprintf(bwname, sizeof(bwname), "%s/%s",
			(md == 0 || md == 1 || md == 3) ? TS990_OTHER_lo[lo] :
			(md == 4 || md == 13 || md == 17 || md == 21) ? TS990_AM_lo[lo] :
			(md == 2 ||md==7) ? TS990_CAT_cw_widths [lo]:
			(md == 5 ||md==8) ? TS990_FSKwidths [lo]:
			(md == 6 ||md==9) ? TS990_PSKwidths [lo]:
			TS990_SSB_shift[lo],
			(md == 0 || md == 1 || md == 3) ? TS990_OTHER_hi[hi] :
			(md == 4 || md == 13 || md == 17 || md == 21) ? TS990_AM_hi[hi] :
			(md == 2 ||md==7) ? TS990_CAT_cw_shift [hi]:
			(md == 5 ||md==8) ? TS990_FSKwidths [hi]:
			(md == 6 ||md==9) ? TS990_PSKwidths [hi]:
			TS990_SSB_widths[hi] );

	}
	return bwname;
}


long RIG_TS990::get_vfoA ()
{
	cmd = "FA;";
	int ret = waitN(14, 20, "get vfoA", ASC);

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

void RIG_TS990::set_vfoA (long freq)
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

long RIG_TS990::get_vfoB ()
{
	cmd = "FB;";
	int ret = waitN(14, 20, "get vfoB", ASC);

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

void RIG_TS990::set_vfoB (long freq)
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

int RIG_TS990::get_smeter()
{
	if (useB) {

	int mtr = 0;
	cmd = "SM1;";
	int ret = waitN(8, 20, "get", ASC);
	if (ret < 8) return 0;
	size_t p = replystr.find("SM1");
	if (p == string::npos) return 0;

	replystr[p + 7] = 0;
	mtr = atoi(&replystr[p + 3]);
	mtr *= 50;
	mtr /= 15;
	if (mtr > 100) mtr = 100;
	return mtr;

} else {

	int mtr = 0;
	cmd = "SM0;";
	int ret = waitN(8, 20, "get", ASC);
	if (ret < 8) return 0;
	size_t p = replystr.find("SM0");
	if (p == string::npos) return 0;

	replystr[p + 7] = 0;
	mtr = atoi(&replystr[p + 3]);
	mtr *= 50;
	mtr /= 15;
	if (mtr > 100) mtr = 100;
	return mtr;
}

}

int RIG_TS990::get_power_out()
{
	if (useB) {

	int mtr = 0;
	cmd = "SM1;";
	int ret = waitN(8, 20, "get power", ASC);

	if (ret < 8) return mtr;
	size_t p = replystr.rfind("SM1");
	if (p == string::npos) return mtr;

	mtr = atoi(&replystr[p + 3]);
	mtr *= 50;
	mtr /= 18;
	if (mtr > 200) mtr = 200;

	return mtr;

} else {

	int mtr = 0;
	cmd = "SM0;";
	int ret = waitN(8, 20, "get power", ASC);

	if (ret < 8) return mtr;
	size_t p = replystr.rfind("SM0");
	if (p == string::npos) return mtr;

	mtr = atoi(&replystr[p + 3]);
	mtr *= 50;
	mtr /= 18;
	if (mtr > 200) mtr = 200;

	return mtr;
}


}

static bool read_alc = false;
static int alc_val = 0;

int RIG_TS990::get_swr(void)
{

	int mtr = 0;

	read_alc = false;

	cmd = "RM;";
	int ret = waitN(16, 20, "get swr/alc", ASC);
	if (ret < 16) return 0;

	size_t p = replystr.find("RM1");
	if (p != string::npos) {
		replystr[p + 7] = 0;
		alc_val = atoi(&replystr[p + 3]);
		alc_val *= 100;
		alc_val /= 15;
		if (alc_val > 100) alc_val = 100;
		read_alc = true;
	}

	p = replystr.find("RM2");
	if (p == string::npos) return 0;

	replystr[p + 7] = 0;
	mtr = atoi(&replystr[p + 3]);
	mtr *= 50;
	mtr /= 15;
	if (mtr > 100) mtr = 100;

	return mtr;
}

int RIG_TS990::get_alc(void)
{
	if (read_alc) {
		read_alc = false;
		return alc_val;
	}
	cmd = "RM;";
	int ret = waitN(8, 20, "get alc", ASC);
	if (ret < 8) return 0;

	size_t p = replystr.find("RM1");
	if (p == string::npos) return 0;

	replystr[p + 7] = 0;
	alc_val = atoi(&replystr[p + 3]);
	alc_val *= 100;
	alc_val /= 15;
	if (alc_val > 100) alc_val = 100;
	return alc_val;
}

// Transceiver power level
void RIG_TS990::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd, 0);
}

int RIG_TS990::get_power_control()
{
	cmd = "PC;";
	int ret = waitN(6, 20, "get pwr ctrl", ASC);

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

// Volume control return 0 ... 100
int RIG_TS990::get_volume_control()
{
	if (useB) {

	cmd = "AG1;";
	int ret = waitN(7, 20, "get vol ctrl", ASC);

	if (ret < 7) return 0;
	size_t p = replystr.rfind("AG");
	if (p == string::npos) return 0;

	replystr[p + 6] = 0;
	int val = atoi(&replystr[p + 3]);
	return (int)(val / 2.55);

	} else {

	cmd = "AG0;";
	int ret = waitN(7, 20, "get vol ctrl", ASC);

	if (ret < 7) return 0;
	size_t p = replystr.rfind("AG");
	if (p == string::npos) return 0;

	replystr[p + 6] = 0;
	int val = atoi(&replystr[p + 3]);
	return (int)(val / 2.55);
	}

}

void RIG_TS990::set_volume_control(int val)
{
	if (useB) {

	int ivol = (int)(val * 2.55);
	cmd = "AG1000;";
	for (int i = 5; i > 2; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendCommand(cmd, 0);

	} else {

	int ivol = (int)(val * 2.55);
	cmd = "AG0000;";
	for (int i = 5; i > 2; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendCommand(cmd, 0);
	}
}

// Tranceiver PTT on/off
void RIG_TS990::set_PTT_control(int val)
{
	if (val) {
		if (data_mode)
			cmd = "TX1;";
		else
			cmd = "TX0;";
	} else
		cmd = "RX;";
	sendCommand(cmd, 0);
}

void RIG_TS990::tune_rig()
{
	cmd = "AC111;";
	sendCommand(cmd, 0);
}



//----------------------------------------------------------------------
void RIG_TS990::set_modeA(int val)
{
	active_mode = A.imode = val;
	cmd = "OM0";
	cmd += TS990_mode_chr[val];
	cmd += ';';
	sendCommand(cmd, 0);
	showresp(ERR, ASC, "set mode main band", cmd, replystr);
	set_widths(val);
}

int RIG_TS990::get_modeA()
{
	int md = A.imode;
	cmd = "OM0;";
	int ret = waitN(5, 20, "get mode main band", ASC);

	if (ret < 5) return A.imode;

	size_t p = replystr.rfind("OM");
	if (p == string::npos) return A.imode;

	switch (replystr[p + 3]) {
		case '1' : md = tsLSB; break;
		case '2' : md = tsUSB; break;
		case '3' : md = tsCW; break;
		case '4' : md = tsFM; break;
		case '5' : md = tsAM; break;
		case '6' : md = tsFSK; break;
		case '7' : md = tsCWR; break;
		case '9' : md = tsFSKR; break;
		case 'A' : md = tsPSK; break;
		case 'B' : md = tsPSKR; break;
		case 'C' : md = tsLSBD1; break;
		case 'D' : md = tsUSBD1; break;
		case 'E' : md = tsFMD1; break;
		case 'F' : md = tsAMD1; break;
		case 'G' : md = tsLSBD2; break;
		case 'H' : md = tsUSBD2; break;
		case 'I' : md = tsFMD2; break;
		case 'J' : md = tsAMD2; break;
		case 'K' : md = tsLSBD3; break;
		case 'L' : md = tsUSBD3; break;
		case 'M' : md = tsFMD3; break;
		case 'N' : md = tsAMD3; break;
		default : md = A.imode;

	}
	if (md != A.imode) {
		active_mode = A.imode = md;
		set_widths(md);
	}
	get_agc();
	return A.imode;
}

void RIG_TS990::set_modeB(int val)
{
	active_mode = B.imode = val;
	cmd = "OM1";
	cmd += TS990_mode_chr[val];
	cmd += ';';
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set mode sub band", cmd, replystr);
	set_widths(val);
}

int RIG_TS990::get_modeB()
{

	int md = B.imode;
	cmd = "OM1;";
	int ret = waitN(5, 20, "get mode sub band", ASC);

	if (ret < 5) return B.imode;
	size_t p = replystr.rfind("OM");
	if (p == string::npos) return B.imode;

	switch (replystr[p + 3]) {
		case '1' : md = tsLSB; break;
		case '2' : md = tsUSB; break;
		case '3' : md = tsCW; break;
		case '4' : md = tsFM; break;
		case '5' : md = tsAM; break;
		case '6' : md = tsFSK; break;
		case '7' : md = tsCWR; break;
		case '9' : md = tsFSKR; break;
		case 'A' : md = tsPSK; break;
		case 'B' : md = tsPSKR; break;
		case 'C' : md = tsLSBD1; break;
		case 'D' : md = tsUSBD1; break;
		case 'E' : md = tsFMD1; break;
		case 'F' : md = tsAMD1; break;
		case 'G' : md = tsLSBD2; break;
		case 'H' : md = tsUSBD2; break;
		case 'I' : md = tsFMD2; break;
		case 'J' : md = tsAMD2; break;
		case 'K' : md = tsLSBD3; break;
		case 'L' : md = tsUSBD3; break;
		case 'M' : md = tsFMD3; break;
		case 'N' : md = tsAMD3; break;
		default : md = B.imode;
	}
	if (md != B.imode) {
		active_mode = B.imode = md;
		set_widths(md);
	}
	get_agc();
	return B.imode;
}

//----------------------------------------------------------------------

void RIG_TS990::set_mic_gain(int val)
{
	cmd = "MG000;";
	for (int i = 3; i > 0; i--) {
		cmd[1+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd, 0);
}


int RIG_TS990::get_mic_gain()
{
	int val = 0;
	cmd = "MG;";
	int ret = waitN(6, 20, "get mic ctrl", ASC);

	if (ret >= 6) {
		size_t p = replystr.rfind("MG");
		if (p == string::npos) return val;
		replystr[p + 5] = 0;
		val = atoi(&replystr[p + 2]);
	}
	return val;
}

void RIG_TS990::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

//======================================================================
//  Check SSB Mode Filter Setting - EX00607;
//	Response EX00607 000; for Hi/Lo Cut or EX00607 001; for Shift/Width.
//======================================================================


void  RIG_TS990::check_menu_0607()
{

	menu_0607 = false;
	cmd = "EX00607;"; sendCommand(cmd);
	int ret = waitN(12, 20, "Check SSB Hi/Lo", ASC);
	if (ret >= 12) {
		size_t p = replystr.rfind("EX00607");
		if (p != string::npos)
			menu_0607 = (replystr[p+10] == '1');
	}
	if (menu_0607) {
			dsp_lo     = TS990_SSB_widths;
			dsp_hi     = TS990_SSB_shift;
			lo_tooltip = TS990_SSB_W_tooltip;
			lo_label   = TS990_SSB_W_btn_label;
			hi_tooltip = TS990_SSB_S_tooltip;
			hi_label   = TS990_SSB_S_btn_label;
			B.iBW = A.iBW = 0x8E05;
	} else {
			dsp_lo = TS990_OTHER_lo;
			dsp_hi = TS990_OTHER_hi;
			lo_tooltip = TS990_OTHER_lo_tooltip;
			lo_label   = TS990_OTHER_btn_lo_label;
			hi_tooltip = TS990_OTHER_hi_tooltip;
			hi_label   = TS990_OTHER_btn_hi_label;
			B.iBW = A.iBW = 0x8903;
	}
}

//======================================================================
//  Check Data Mode Filter Setting - EX00608;
//	Response EX00608 000; for Hi/Lo Cut or EX00608 001; for Shift/Width.
//======================================================================

void RIG_TS990::check_menu_0608()
{

	menu_0608 = false;
	cmd = "EX00608;"; sendCommand(cmd);
	int ret = waitN(12, 20, "Check SSB Hi/Lo", ASC);
	if (ret >= 12) {
		size_t p = replystr.rfind("EX00608");
		if (p != string::npos)
			menu_0608 = (replystr[p+10] == '1');
	}
	if (menu_0608) {
			dsp_lo     = TS990_SSB_widths;
			dsp_hi     = TS990_SSB_shift;
			lo_tooltip = TS990_SSB_W_tooltip;
			lo_label   = TS990_SSB_W_btn_label;
			hi_tooltip = TS990_SSB_S_tooltip;
			hi_label   = TS990_SSB_S_btn_label;
			B.iBW = A.iBW = 0x8E05;
	} else {
			dsp_lo = TS990_OTHER_lo;
			dsp_hi = TS990_OTHER_hi;
			lo_tooltip = TS990_OTHER_lo_tooltip;
			lo_label   = TS990_OTHER_btn_lo_label;
			hi_tooltip = TS990_OTHER_hi_tooltip;
			hi_label   = TS990_OTHER_btn_hi_label;
			B.iBW = A.iBW = 0x8903;
	}
}




//----------------------------------------------------------------------
//======================================================================
// Bandpass filter commands
//======================================================================

int RIG_TS990::set_widths(int val)
{


		int bw=0;

		if (val == 0 || val== 1 ) {
		bandwidths_ = TS990_empty;
		if (menu_0607) {
			dsp_lo     = TS990_SSB_widths;
			dsp_hi     = TS990_SSB_shift;
			lo_tooltip = TS990_SSB_W_tooltip;
			lo_label   = TS990_SSB_W_btn_label;
			hi_tooltip = TS990_SSB_S_tooltip;
			hi_label   = TS990_SSB_S_btn_label;
			bw = 0x8E05;
			return bw;
	} else {
			dsp_lo = TS990_OTHER_lo;
			dsp_hi = TS990_OTHER_hi;
			lo_tooltip = TS990_OTHER_lo_tooltip;
			lo_label   = TS990_OTHER_btn_lo_label;
			hi_tooltip = TS990_OTHER_hi_tooltip;
			hi_label   = TS990_OTHER_btn_hi_label;
			bw = 0x8903;
			return bw;
	}

	} else if (val == 10 || val == 11 || val == 14 || val == 15 || val == 18 || val == 19) {
		bandwidths_ = TS990_empty;
		if (menu_0608) {
			dsp_lo     = TS990_SSB_widths;
			dsp_hi     = TS990_SSB_shift;
			lo_tooltip = TS990_SSB_W_tooltip;
			lo_label   = TS990_SSB_W_btn_label;
			hi_tooltip = TS990_SSB_S_tooltip;
			hi_label   = TS990_SSB_S_btn_label;
			bw = 0x8E05;
			return bw;
	} else {
			dsp_lo = TS990_OTHER_lo;
			dsp_hi = TS990_OTHER_hi;
			lo_tooltip = TS990_OTHER_lo_tooltip;
			lo_label   = TS990_OTHER_btn_lo_label;
			hi_tooltip = TS990_OTHER_hi_tooltip;
			hi_label   = TS990_OTHER_btn_hi_label;
			bw = 0x8903;
			return bw;
	}




	} else if (val == 2 || val ==7) {
			bandwidths_ = TS990_empty;
			dsp_lo = TS990_CW_widths;
			dsp_hi = TS990_CW_shift;
			lo_tooltip = TS990_CW_W_tooltip;
			lo_label   = TS990_CW_W_btn_label;
			hi_tooltip = TS990_CW_S_tooltip;
			hi_label   = TS990_CW_S_btn_label;
			bw = 0x9005;
			return bw;



	} else if (val ==5 || val ==8) {
			bandwidths_ = TS990_FSKwidths;
			dsp_lo = TS990_empty;
			dsp_hi = TS990_empty;
			bw = 1;
			return bw;


	} else if (val == 6 || val ==9) {
			bandwidths_ = TS990_PSKwidths;
			dsp_lo = TS990_empty;
			dsp_hi = TS990_empty;
			bw = 1;
			return bw;


	} else if (val ==4 || val == 13 || val == 17 || val == 21) {
			bandwidths_ = TS990_empty;
			dsp_lo = TS990_AM_lo;
			dsp_hi = TS990_AM_hi;
			lo_tooltip = TS990_AM_lo_tooltip;
			lo_label   = TS990_AM_btn_lo_label;
			hi_tooltip = TS990_AM_hi_tooltip;
			hi_label   = TS990_AM_btn_hi_label;
			bw = 0x8201;
			return bw;

		} else if (val == 3 || val == 12 || val == 16 || val == 22) {
			bandwidths_ = TS990_empty;
			dsp_lo = TS990_OTHER_lo;
			dsp_hi = TS990_OTHER_hi;
			lo_tooltip = TS990_OTHER_lo_tooltip;
			lo_label   = TS990_OTHER_btn_lo_label;
			hi_tooltip = TS990_OTHER_hi_tooltip;
			hi_label   = TS990_OTHER_btn_hi_label;
			bw = 0x8201;
			return bw;
	}
	return bw;
}

const char **RIG_TS990::bwtable(int m)
{
	switch (m) {
		case tsLSB: case tsUSB: case tsFM:
		case tsFMD1: case tsFMD2: case tsFMD3:
		case tsLSBD1: case tsLSBD2: case tsLSBD3:
		case tsUSBD1: case tsUSBD2: case tsUSBD3:
		case tsAM: case tsAMD1: case tsAMD2: case tsAMD3:
		case tsCW: case tsCWR:
			return NULL;

		case tsFSK: case tsFSKR:
			return TS990_FSKwidths;

		case tsPSK: case tsPSKR:
			return TS990_PSKwidths;

	}
	return NULL;
}

const char **RIG_TS990::lotable(int m)
{
	switch (m) {
		case tsLSB: case tsUSB: case tsFM:
		case tsFMD1: case tsFMD2: case tsFMD3:
			return TS990_OTHER_lo;
		case tsCW: case tsCWR:
			return TS990_CW_widths;
		case tsFSK: case tsFSKR:
			return TS990_FSKwidths;
		case tsPSK: case tsPSKR:
			return TS990_PSKwidths;
		case tsAM: case tsAMD1: case tsAMD2: case tsAMD3:
			return TS990_AM_lo;

			}
	return TS990_SSB_shift;
}

const char **RIG_TS990::hitable(int m)
{
	switch (m) {
		case tsLSB: case tsUSB: case tsFM:
		case tsFMD1: case tsFMD2: case tsFMD3:
			return TS990_OTHER_hi;
		case tsCW: case tsCWR:
			return TS990_CW_shift;
		case tsFSK: case tsFSKR:
			return NULL;
		case tsPSK: case tsPSKR:
			return NULL;
		case tsAM: case tsAMD1: case tsAMD2: case tsAMD3:
			return TS990_AM_hi;

	}
	return TS990_SSB_widths;
}

int RIG_TS990::adjust_bandwidth(int val)
{
	int retval = 0x8903;
	switch (val) {
		case tsAM: case tsAMD1: case tsAMD2: case tsAMD3:
			retval = 0x8201;
		case tsLSB: case tsUSB: case tsFM:
		case tsFMD1: case tsFMD2: case tsFMD3:
			retval = 0x8903;
		case tsLSBD1: case tsLSBD2: case tsLSBD3:
		case tsUSBD1: case tsUSBD2: case tsUSBD3:
			retval = 0x8E05;
		case tsCW: case tsCWR:
			retval = 0x9005;
		case tsFSK: case tsFSKR:
			retval = 1;
		case tsPSK: case tsPSKR:
			retval = 2;

		}
	return retval;


}

int RIG_TS990::def_bandwidth(int val)
{
	return adjust_bandwidth(val);
}

void RIG_TS990::set_bwA(int val)

{
	if ( A.imode ==0 || A.imode == 1 ) {
			if (val < 256) return;
			if (menu_0607) {
			A.iBW = val;
			cmd = "SL0";
			cmd += TS990_CAT_ssb_widths[A.iBW  & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set SSB Data lower", cmd, replystr);
			cmd = "SH0";
			cmd += TS990_CAT_ssb_shift[(A.iBW >> 8) & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set SET SSB Data upper", cmd, replystr);
			return;

		} else {
			A.iBW = val;
			cmd = "SL0";
			cmd += TS990_CAT_other_lo[A.iBW  & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set SSB lower", cmd, replystr);
			cmd = "SH0";
			cmd += TS990_CAT_other_hi[(A.iBW >> 8) & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set SET SSB upper", cmd, replystr);
			return;
		}


		} else if ( A.imode == 10 || A.imode == 11 || A.imode == 14 ||
	 A.imode == 15 || A.imode == 18 || A.imode == 19) {
			if (val < 256) return;
			if (menu_0608) {
			A.iBW = val;
			cmd = "SL0";
			cmd += TS990_CAT_ssb_widths[A.iBW  & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set SSB Data lower", cmd, replystr);
			cmd = "SH0";
			cmd += TS990_CAT_ssb_shift[(A.iBW >> 8) & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set SET SSB Data upper", cmd, replystr);
			return;

		} else {
			A.iBW = val;
			cmd = "SL0";
			cmd += TS990_CAT_other_lo[A.iBW  & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set SSB lower", cmd, replystr);
			cmd = "SH0";
			cmd += TS990_CAT_other_hi[(A.iBW >> 8) & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set SET SSB upper", cmd, replystr);
			return;
		}


		} else if (A.imode == 4 || A.imode == 13 || A.imode == 17 || A.imode == 21 ){
			if (val < 256) return;
			A.iBW = val;
			cmd = "SL0";
			cmd += TS990_CAT_am_lo[A.iBW & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set AM lower", cmd, replystr);
			cmd = "SH0";
			cmd += TS990_CAT_am_hi[(A.iBW >> 8) & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set AM upper", cmd, replystr);
			return;

		} else if (A.imode == 3 || A.imode == 12 || A.imode == 18 || A.imode == 22 ){
			if (val < 256) return;
			A.iBW = val;
			cmd = "SL0";
			cmd += TS990_CAT_other_lo[A.iBW & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set FM lower", cmd, replystr);
			cmd = "SH0";
			cmd += TS990_CAT_other_hi[(A.iBW >> 8) & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set FM upper", cmd, replystr);
			return;

		} else if (A.imode == 2 || A.imode == 7 ) {
			if (val < 256) return;
			A.iBW = val;
			cmd = "SL0";
			cmd += TS990_CAT_cw_widths[A.iBW & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set cw width", cmd, replystr);
			cmd = "SH0";
			cmd += TS990_CAT_cw_shift[(A.iBW >> 8) & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set cw shift", cmd, replystr);
			return;

	} else if (A.imode == 5 || A.imode == 8) {
			if (val > 256) return;
			A.iBW = val;
			cmd = "SL0";
			cmd += TS990_FSKbw[A.iBW];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set FSK bw", cmd, replystr);
			return;



	} else if (A.imode == 6 || A.imode == 9) {
			if (val > 256) return;
			A.iBW = val;
			cmd = "SL0";
			cmd += TS990_PSKbw[A.iBW];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set PSK bw", cmd, replystr);
			return;

	}
}

void RIG_TS990::set_bwB(int val)

{
	if ( B.imode ==0 || B.imode == 1 ) {
			if (val < 256) return;
			if (menu_0607) {
			B.iBW = val;
			cmd = "SL1";
			cmd += TS990_CAT_ssb_widths[B.iBW  & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set SSB Data lower", cmd, replystr);
			cmd = "SH1";
			cmd += TS990_CAT_ssb_shift[(B.iBW >> 8) & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set SET SSB Data upper", cmd, replystr);
			return;

		} else {
			B.iBW = val;
			cmd = "SL1";
			cmd += TS990_CAT_other_lo[B.iBW  & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set SSB lower", cmd, replystr);
			cmd = "SH1";
			cmd += TS990_CAT_other_hi[(B.iBW >> 8) & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set SET SSB upper", cmd, replystr);
			return;
		}


		} else if ( B.imode == 10 || B.imode == 11 || B.imode == 14 ||
	 B.imode == 15 || B.imode == 18 || B.imode == 19) {
			if (val < 256) return;
			if (menu_0608) {
			B.iBW = val;
			cmd = "SL1";
			cmd += TS990_CAT_ssb_widths[B.iBW  & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set SSB Data lower", cmd, replystr);
			cmd = "SH1";
			cmd += TS990_CAT_ssb_shift[(B.iBW >> 8) & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set SET SSB Data upper", cmd, replystr);
			return;

		} else {
			B.iBW = val;
			cmd = "SL1";
			cmd += TS990_CAT_other_lo[B.iBW  & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set SSB lower", cmd, replystr);
			cmd = "SH1";
			cmd += TS990_CAT_other_hi[(B.iBW >> 8) & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set SET SSB upper", cmd, replystr);
			return;
		}


		} else if (B.imode == 4 || B.imode == 13 || B.imode == 17 || B.imode == 21 ){
			if (val < 256) return;
			B.iBW = val;
			cmd = "SL1";
			cmd += TS990_CAT_am_lo[B.iBW & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set AM lower", cmd, replystr);
			cmd = "SH1";
			cmd += TS990_CAT_am_hi[(B.iBW >> 8) & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set AM upper", cmd, replystr);
			return;

		} else if (B.imode == 3 || B.imode == 12 || B.imode == 18 || B.imode == 22 ){
			if (val < 256) return;
			B.iBW = val;
			cmd = "SL1";
			cmd += TS990_CAT_other_lo[B.iBW & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set FM lower", cmd, replystr);
			cmd = "SH1";
			cmd += TS990_CAT_other_hi[(B.iBW >> 8) & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set FM upper", cmd, replystr);
			return;

		} else if (B.imode == 2 || B.imode == 7 ) {
			if (val < 256) return;
			B.iBW = val;
			cmd = "SL1";
			cmd += TS990_CAT_cw_widths[B.iBW & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set cw width", cmd, replystr);
			cmd = "SH1";
			cmd += TS990_CAT_cw_shift[(B.iBW >> 8) & 0x7F];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set cw shift", cmd, replystr);
			return;

	} else if (B.imode == 5 || B.imode == 8) {
			if (val > 256) return;
			B.iBW = val;
			cmd = "SL1";
			cmd += TS990_FSKbw[B.iBW];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set FSK bw", cmd, replystr);
			return;



	} else if (B.imode == 6 || B.imode == 9) {
			if (val > 256) return;
			B.iBW = val;
			cmd = "SL1";
			cmd += TS990_PSKbw[B.iBW];
			sendCommand(cmd,0);
			showresp(WARN, ASC, "set PSK bw", cmd, replystr);
			return;

	}
}


int RIG_TS990::get_bwA()
{

	size_t p;




			if (A.imode == 2 || A.imode ==7) {
			int lo = A.iBW & 0x7F, hi = (A.iBW >> 8) & 0x7F;
			cmd = "SL0;";
			waitN(6, 20, "get CW lower", ASC);
			p = replystr.rfind("SL");
			if (p != string::npos)
				lo = fm_decimal(&replystr[3], 2); //2 2
			cmd = "SH0;";
			waitN(6, 20, "get CW upper", ASC);
			p = replystr.rfind("SH");
			if (p != string::npos)
				hi = fm_decimal(&replystr[3], 2); //2 2
			A.iBW = ((hi << 8) | (lo & 0x7F )) | 0x8000;
			return A.iBW;

		}else if (A.imode ==5 || A.imode ==8) {
			int lo = A.iBW & 0x7F;
			cmd = "SL0;";
			waitN(6, 20, "get FSK Width", ASC);
			p = replystr.rfind("SL");
			if (p != string::npos)
				lo = fm_decimal(&replystr[3], 2); //2 2
				A.iBW = (lo & 0x7F);
				return A.iBW;


		} else if (A.imode == 6 || A.imode == 9) {
			int lo = A.iBW & 0x7F;
			cmd = "SL0;";
			waitN(6, 20, "get PSK Width", ASC);
			p = replystr.rfind("SL");
			if (p != string::npos)
				lo = fm_decimal(&replystr[3], 2); //2 2
				A.iBW = (lo & 0x7F);
				return A.iBW;



		} else if (A.imode == 0 || A.imode == 1 || A.imode == 10 || A.imode == 11 ||
		A.imode == 14 || A.imode == 15 || A.imode == 18 || A.imode == 19) {
			int lo = A.iBW & 0x7F, hi = (A.iBW >> 8) & 0x7F;
			cmd = "SL0;";
			waitN(6, 20, "get SSB lower", ASC);
			p = replystr.rfind("SL");
			if (p != string::npos)
				lo = fm_decimal(&replystr[3], 2); //2 2
			cmd = "SH0;";
			waitN(6, 20, "get SSB upper", ASC);
			p = replystr.rfind("SH");
			if (p != string::npos)
				hi = fm_decimal(&replystr[3], 2); //2 2
			A.iBW = ((hi << 8) | (lo & 0x7F )) | 0x8000;
			return A.iBW;




		} else if (A.imode == 4 || A.imode == 13 || A.imode == 17 || A.imode == 21) {

			int lo = A.iBW & 0x7F, hi = (A.iBW >> 8) & 0x7F;
			cmd = "SL0;";
			waitN(6, 20, "get AM lo", ASC);
			p = replystr.rfind("SL");
			if (p != string::npos)
				lo = fm_decimal(&replystr[3], 2); //2 2
			cmd = "SH0;";
			waitN(6, 20, "get AM hi", ASC);
			p = replystr.rfind("SH");
			if (p != string::npos)
				hi = fm_decimal(&replystr[3], 2); //2 2
			A.iBW = ((hi << 8) | (lo & 0x7F)) | 0x8000;
			return A.iBW;


		} else if (A.imode == 3 || A.imode == 12 || A.imode == 16 || A.imode == 22) {

			int lo = A.iBW & 0x7F, hi = (A.iBW >> 8) & 0x7F;
			cmd = "SL0;";
			waitN(6, 20, "get FM lo", ASC);
			p = replystr.rfind("SL");
			if (p != string::npos)
				lo = fm_decimal(&replystr[3], 2); //2 2
			cmd = "SH0;";
			waitN(6, 20, "get FM hi", ASC);
			p = replystr.rfind("SH");
			if (p != string::npos)
				hi = fm_decimal(&replystr[3], 2); //2 2
			A.iBW = ((hi << 8) | (lo & 0x7F)) | 0x8000;
			return A.iBW;

	}
	return A.iBW;
}



int RIG_TS990::get_bwB()
{

	size_t p;





			if (B.imode == 2 || B.imode ==7) {
			int lo = B.iBW & 0x7F, hi = (B.iBW >> 8) & 0x7F;
			cmd = "SL1;";
			waitN(6, 20, "get CW lower", ASC);
			p = replystr.rfind("SL");
			if (p != string::npos)
				lo = fm_decimal(&replystr[3], 2); //2 2
			cmd = "SH1;";
			waitN(6, 20, "get CW upper", ASC);
			p = replystr.rfind("SH");
			if (p != string::npos)
				hi = fm_decimal(&replystr[3], 2); //2 2
			B.iBW = ((hi << 8) | (lo & 0x7F )) | 0x8000;
			return B.iBW;

		}else if (B.imode ==5 || B.imode ==8) {
			int lo = B.iBW & 0x7F;
			cmd = "SL1;";
			waitN(6, 20, "get FSK Width", ASC);
			p = replystr.rfind("SL");
			if (p != string::npos)
				lo = fm_decimal(&replystr[3], 2); //2 2
				B.iBW = (lo & 0x7F);
				return B.iBW;


		} else if (B.imode == 6 || B.imode == 9) {
			int lo = B.iBW & 0x7F;
			cmd = "SL1;";
			waitN(6, 20, "get PSK Width", ASC);
			p = replystr.rfind("SL");
			if (p != string::npos)
				lo = fm_decimal(&replystr[3], 2); //2 2
				B.iBW = (lo & 0x7F);
				return B.iBW;



		} else if (B.imode == 0 || B.imode == 1 || B.imode == 10 || B.imode == 11 ||
		B.imode == 14 || B.imode == 15 || B.imode == 18 || B.imode == 19) {
			int lo = B.iBW & 0x7F, hi = (B.iBW >> 8) & 0x7F;
			cmd = "SL1;";
			waitN(6, 20, "get SSB lower", ASC);
			p = replystr.rfind("SL");
			if (p != string::npos)
				lo = fm_decimal(&replystr[3], 2); //2 2
			cmd = "SH1;";
			waitN(6, 20, "get SSB upper", ASC);
			p = replystr.rfind("SH");
			if (p != string::npos)
				hi = fm_decimal(&replystr[3], 2); //2 2
			B.iBW = ((hi << 8) | (lo & 0x7F )) | 0x8000;
			return B.iBW;




		} else if (B.imode == 4 || B.imode == 13 || B.imode == 17 || B.imode == 21) {

			int lo = B.iBW & 0x7F, hi = (B.iBW >> 8) & 0x7F;
			cmd = "SL1;";
			waitN(6, 20, "get AM lo", ASC);
			p = replystr.rfind("SL");
			if (p != string::npos)
				lo = fm_decimal(&replystr[3], 2); //2 2
			cmd = "SH1;";
			waitN(6, 20, "get AM hi", ASC);
			p = replystr.rfind("SH");
			if (p != string::npos)
				hi = fm_decimal(&replystr[3], 2); //2 2
			B.iBW = ((hi << 8) | (lo & 0x7F)) | 0x8000;
			return B.iBW;


		} else if (B.imode == 3 || B.imode == 12 || B.imode == 16 || B.imode == 22) {

			int lo = B.iBW & 0x7F, hi = (B.iBW >> 8) & 0x7F;
			cmd = "SL1;";
			waitN(6, 20, "get FM lo", ASC);
			p = replystr.rfind("SL");
			if (p != string::npos)
				lo = fm_decimal(&replystr[3], 2); //2 2
			cmd = "SH1;";
			waitN(6, 20, "get FM hi", ASC);
			p = replystr.rfind("SH");
			if (p != string::npos)
				hi = fm_decimal(&replystr[3], 2); //2 2
			B.iBW = ((hi << 8) | (lo & 0x7F)) | 0x8000;
			return B.iBW;

	}
	return B.iBW;
}






int RIG_TS990::get_modetype(int n)
{
	return TS990_mode_type[n];
}


void RIG_TS990::set_noise(bool val) //Now Setting AGC
{

		if (useB) {

	if (nb_level == 2) {
		nb_level = 3;
			nb_label("AGC F", false);
			cmd = "GC13;";
			sendCommand(cmd, 0);
	} else if (nb_level == 3) {
		nb_level = 1;
		nb_label("AGC S", false);
			cmd = "GC11;";
		sendCommand(cmd, 0);
	} else if (nb_level == 1) {
		nb_level = 2;
		nb_label("AGC M", false);
			cmd = "GC12;";
			sendCommand(cmd, 0);
		}


	 } else {

	if (nb_level == 2) {
		nb_level = 3;
			nb_label("AGC F", false);
			cmd = "GC03;";
			sendCommand(cmd, 0);
	} else if (nb_level == 3) {
		nb_level = 1;
		nb_label("AGC S", false);
			cmd = "GC01;";
		sendCommand(cmd, 0);
	} else if (nb_level == 1) {
		nb_level = 2;
		nb_label("AGC M", false);
			cmd = "GC02;";
			sendCommand(cmd, 0);
		}
	}
}

//----------------------------------------------------------------------
int  RIG_TS990::get_agc()
{
	if (useB) {

	int val = 0;

	cmd = "GC1;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get AGC", cmd, replystr);
	if (ret < 5) return val;
	size_t p = replystr.rfind("GC");
	if (p == string::npos) return val;

	if (replystr[p + 3] == '1' ) {
		nb_label("AGC S", false);

	} else if (replystr[p + 3] == '2' ) {
		nb_label("AGC M", false);

	} else if (replystr[p + 3] == '3' ) {
		nb_label("AGC F", false);
	}
		return val;

	} else {

	int val = 0;

	cmd = "GC0;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get AGC", cmd, replystr);
	if (ret < 5) return val;
	size_t p = replystr.rfind("GC");
	if (p == string::npos) return val;

	if (replystr[p + 3] == '1' ) {
		nb_label("AGC S", false);

	} else if (replystr[p + 3] == '2' ) {
		nb_label("AGC M", false);

	} else if (replystr[p + 3] == '3' ) {
		nb_label("AGC F", false);
	}

	return val;

}

}

//----------------------------------------------------------------------

void RIG_TS990::set_squelch(int val)
{
	if (useB) {

	cmd = "SQ1";
	cmd.append(to_decimal(abs(val),3)).append(";");
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set squelch", cmd, replystr);

	} else {

	cmd = "SQ0";
	cmd.append(to_decimal(abs(val),3)).append(";");
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set squelch", cmd, replystr);
	}
}

int  RIG_TS990::get_squelch()
{
	if (useB) {

	int val = 0;
	cmd = "SQ1;";
	int ret = waitN(7, 20, "get squelch", ASC);

	if (ret >= 7) {
		size_t p = replystr.rfind("SQ1");
		if (p == string::npos) return val;
		replystr[p + 6] = 0;
		val = atoi(&replystr[p + 3]);
	}
	return val;

} else {

	int val = 0;
	cmd = "SQ0;";
	int ret = waitN(7, 20, "get squelch", ASC);

	if (ret >= 7) {
		size_t p = replystr.rfind("SQ0");
		if (p == string::npos) return val;
		replystr[p + 6] = 0;
		val = atoi(&replystr[p + 3]);
	}
	return val;
	}
}

void RIG_TS990::get_squelch_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 255; step = 1;
}

void RIG_TS990::set_rf_gain(int val)
{
	if (useB) {

	cmd = "RG1";
	cmd.append(to_decimal(val,3)).append(";");
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set rf gain", cmd, replystr);

} else {

	cmd = "RG0";
	cmd.append(to_decimal(val,3)).append(";");
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set rf gain", cmd, replystr);
}
}

int  RIG_TS990::get_rf_gain()
{
	if (useB) {

	int val = progStatus.rfgain;
	cmd = "RG1;";
	int ret = waitN(7, 20, "get rf gain", ASC);
	if (ret < 7) return val;
	size_t p = replystr.rfind("RG");
	if (p != string::npos)
		val = fm_decimal(&replystr[p+3], 3);
	return val;

} else {

	int val = progStatus.rfgain;
	cmd = "RG0;";
	int ret = waitN(7, 20, "get rf gain", ASC);
	if (ret < 7) return val;
	size_t p = replystr.rfind("RG");
	if (p != string::npos)
		val = fm_decimal(&replystr[p+3], 3);
	return val;
}
}

void RIG_TS990::get_rf_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 255;
	step = 1;
}

static bool nr_on = false;

void RIG_TS990::set_noise_reduction(int val)
{
	if (useB) {

	cmd.assign("NR1").append(val ? "1" : "0" ).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET noise reduction", cmd, replystr);
	if (val) nr_on = true;
	else nr_on = false;

} else {

	cmd.assign("NR0").append(val ? "1" : "0" ).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET noise reduction", cmd, replystr);
	if (val) nr_on = true;
	else nr_on = false;
}
}

int  RIG_TS990::get_noise_reduction()
{
	if (useB) {

	int val;
	cmd = rsp = "NR1";
	cmd.append(";");
	waitN(5, 20, "GET noise reduction", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	val = replystr[p+3] - '0';
	if (val == 1) nr_on = true;
	else nr_on = false;
	return val;

	} else {

	int val;
	cmd = rsp = "NR0";
	cmd.append(";");
	waitN(5, 20, "GET noise reduction", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	val = replystr[p+3] - '0';
	if (val == 1) nr_on = true;
	else nr_on = false;
	return val;
}
}

void RIG_TS990::set_noise_reduction_val(int val)
{
	if (useB) {

	cmd.assign("RL11").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_val", cmd, replystr);

	} else {

	cmd.assign("RL10").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_val", cmd, replystr);
}
}

int  RIG_TS990::get_noise_reduction_val()
{

	if (useB) {

	if (!nr_on) return 0;
	int val = 1;
	cmd = rsp = "RL11";
	cmd.append(";");
	waitN(7, 20, "GET noise reduction val", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return val;
	val = fm_decimal(&replystr[p+4], 2);
	return val;

	} else {

	if (!nr_on) return 0;
	int val = 1;
	cmd = rsp = "RL10";
	cmd.append(";");
	waitN(7, 20, "GET noise reduction val", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return val;
	val = fm_decimal(&replystr[p+4], 2);
	return val;
}

}

void RIG_TS990::set_auto_notch(int v)
{
	if (useB) {

	cmd.assign("NT1").append(v ? "1" : "0" ).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET Auto Notch", cmd, replystr);

} else {

	cmd.assign("NT0").append(v ? "1" : "0" ).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET Auto Notch", cmd, replystr);
}
}




int  RIG_TS990::get_auto_notch()
{
	if (useB) {

	cmd = "NT1;";
	waitN(5, 20, "get auto notch", ASC);
	size_t p = replystr.rfind("NT");
	if (p == string::npos) return 0;
	if (replystr[p+3] == '1') return 1;
	return 0;

	} else {

	cmd = "NT0;";
	waitN(5, 20, "get auto notch", ASC);
	size_t p = replystr.rfind("NT");
	if (p == string::npos) return 0;
	if (replystr[p+3] == '1') return 1;
	return 0;
}
}

void RIG_TS990::set_notch(bool on, int val)
{
	if (useB) {

	if (on) {
		cmd.assign("NT12;");
		sendCommand(cmd);
		showresp(WARN, ASC, "Set notch ON", cmd, replystr);
		int bp = (int)(val * 127.0 / 3000);
		if (bp == 0) bp = 1;
		cmd.assign("BP1").append(to_decimal(bp, 3)).append(";");
		sendCommand(cmd);
		showresp(WARN, ASC, "set notch freq", cmd, replystr);
	} else {
		cmd.assign("NT10;");
		sendCommand(cmd);
		showresp(WARN, ASC, "Set notch OFF", cmd, replystr);
	}

	} else {

	if (on) {
		cmd.assign("NT02;");
		sendCommand(cmd);
		showresp(WARN, ASC, "Set notch ON", cmd, replystr);
		int bp = (int)(val * 127.0 / 3000);
		if (bp == 0) bp = 1;
		cmd.assign("BP0").append(to_decimal(bp, 3)).append(";");
		sendCommand(cmd);
		showresp(WARN, ASC, "set notch freq", cmd, replystr);
	} else {
		cmd.assign("NT00;");
		sendCommand(cmd);
		showresp(WARN, ASC, "Set notch OFF", cmd, replystr);
	}
	}
}

bool  RIG_TS990::get_notch(int &val)
{
	if (useB) {

	val = 1500;
	cmd = "NT1;";
	waitN(5, 20, "get notch state", ASC);
	size_t p = replystr.rfind("NT");
	if (p == string::npos)
		return 0;
	if (replystr[p+3] == '2') {
		cmd.assign("BP1;");
		waitN(7, 20, "get notch freq", ASC);
		size_t p = replystr.rfind("BP1");
		if (p != string::npos)
			val = (int)(atoi(&replystr[p+3]) * 3000 / 127.0);

		return 1;
	}
	return 0;

} else {

	val = 1500;
	cmd = "NT0;";
	waitN(5, 20, "get notch state", ASC);
	size_t p = replystr.rfind("NT");
	if (p == string::npos)
		return 0;
	if (replystr[p+3] == '2') {
		cmd.assign("BP0;");
		waitN(7, 20, "get notch freq", ASC);
		size_t p = replystr.rfind("BP0");
		if (p != string::npos)
			val = (int)(atoi(&replystr[p+3]) * 3000.0 / 127.0);

		return 1;
	}
	return 0;
}
}

void RIG_TS990::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 20;
	max = 3000;
	step = 10;

}


//----------------------------------------------------------------------
void RIG_TS990::set_if_shift(int val)  //Now doing Monitor
{

	progStatus.shift_val = val;
	cmd = "ML";
	cmd.append(to_decimal(val,3)).append(";");
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set Mon Level", cmd, replystr);
}

bool RIG_TS990::get_if_shift(int &val)
{
{
		cmd = "ML;";
		waitN(6, 20, "get Mon Level", ASC);
		size_t p = replystr.rfind("ML");
		if (p != string::npos) {
			val = fm_decimal(&replystr[p+2], 3);
		} else
			val = progStatus.shift_val;
		return true;
	}
	val = progStatus.shift_val;
	return false;
}


void RIG_TS990::get_if_min_max_step(int &min, int &max, int &step)
{
	if_shift_min = min = 0;
	if_shift_max = max = 255;
	if_shift_step = step = 1;
	if_shift_mid = 10;
}
