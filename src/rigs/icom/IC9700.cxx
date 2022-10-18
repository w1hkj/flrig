// ----------------------------------------------------------------------------
// Copyright (C) 2019
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

#include <sstream>

#include "icom/IC9700.h"

#include "support.h"

//=============================================================================
// IC-9700

// JBA - Enable this for Sub band to be treated as VFO B if rig is in dualwatch.
// By doing this, the flrig gui control the two freqs shown in the rig display (my preference).
// Otherwise, flrig controls VFOs of Main Band only. 
// Bottom line is I'm not exactly sure how flrig should behave if rig is in dualwatch
// nor do I know how flrig treats other rigs with two receivers.
#define DUALWATCH_SUB_AS_B

const char IC9700name_[] = "IC-9700";

static int nummodes = 9;

enum {
	LSB9700, USB9700, AM9700, FM9700, DV9700,
	CW9700, CW9700R, RTTY9700, RTTY9700R,
	LSB9700D, USB9700D, AM9700D, FM9700D, DVR9700 };

const char *IC9700modes_[] = {
"LSB", "USB", "AM", "FM", "DV", 
"CW", "CW-R", "RTTY", "RTTY-R",
"LSB-D", "USB-D", "AM-D", "FM-D", "DV-R", NULL};

const char IC9700_mode_nbr[] = {
	0x00, 0x01, 0x02, 0x05, 0x17,
	0x03, 0x07, 0x04, 0x08,
	0x00, 0x01, 0x02, 0x05, 0x17 };

const char IC9700_mode_type[] = {
	'L', 'U', 'U', 'U', 'U',
	'L', 'U', 'L', 'U',
	'L', 'U', 'U', 'U' };

const char *IC9700_ssb_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000", "3100", "3200", "3300", "3400", "3500",
"3600",
NULL };
static int IC9700_bw_vals_SSB[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,
40, WVALS_LIMIT};

const char *IC9700_am_bws[] = {
"200",   "400",  "600",  "800", "1000", "1200", "1400", "1600", "1800", "2000",
"2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000",
"4200", "4400", "4600", "4800", "5000", "5200", "5400", "5600", "5800", "6000",
"6200", "6400", "6600", "6800", "7000", "7200", "7400", "9700", "7800", "8000",
"8200", "8400", "8600", "8800", "9000", "9200", "9400", "9600", "9800", "10000", 
NULL };
static int IC9700_bw_vals_AM[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,
40,41,42,43,44,45,46,47,48,49,
WVALS_LIMIT};

const char *IC9700_rtty_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700",
NULL };
static int IC9700_bw_vals_RTTY[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,  WVALS_LIMIT};

const char *IC9700_fixed_bws[] = { "FIXED", NULL };
static int IC9700_bw_vals_fixed[] = { 1, WVALS_LIMIT};

static GUI IC9700_widgets[]= {
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

void RIG_IC9700::initialize()
{
	IC9700_widgets[0].W = btnVol;
	IC9700_widgets[1].W = sldrVOLUME;
	IC9700_widgets[2].W = btnAGC;
	IC9700_widgets[3].W = sldrRFGAIN;
	IC9700_widgets[4].W = sldrSQUELCH;
	IC9700_widgets[5].W = btnNR;
	IC9700_widgets[6].W = sldrNR;
	IC9700_widgets[7].W = btnLOCK;
	IC9700_widgets[8].W = sldrINNER;
	IC9700_widgets[9].W = btnCLRPBT;
	IC9700_widgets[10].W = sldrOUTER;
	IC9700_widgets[11].W = btnNotch;
	IC9700_widgets[12].W = sldrNOTCH;
	IC9700_widgets[13].W = sldrMICGAIN;
	IC9700_widgets[14].W = sldrPOWER;

	btn_icom_select_11->activate();
	btn_icom_select_12->activate();
	btn_icom_select_13->activate();

	choice_rTONE->activate();
	choice_tTONE->activate();
}

RIG_IC9700::RIG_IC9700() {
	defaultCIV = 0xA2;
	adjustCIV(defaultCIV);

	name_ = IC9700name_;
	modes_ = IC9700modes_;

	bandwidths_ = IC9700_ssb_bws;
	bw_vals_ = IC9700_bw_vals_SSB;

	_mode_type = IC9700_mode_type;

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

	A.freq = 14070000ULL;
	A.imode = USB9700D;
	A.iBW = 34;
	B.freq = 7070000ULL;
	B.imode = USB9700D;
	B.iBW = 34;

	def_freq = 14070000ULL;
	def_mode = USB9700D;
	def_bw = 34;

	widgets = IC9700_widgets;

	has_extras = true;

	has_cw_wpm = true;
	has_cw_spot_tone = true;
	has_cw_qsk = true;
	has_cw_break_in = true;

	has_vox_onoff = true;
	has_vox_gain = true;
	has_vox_anti = true;
	has_vox_hang = true;

	has_compON = true;
	has_compression = true;

	has_split = true;
	has_split_AB = true;

	has_micgain_control = true;
	has_bandwidth_control = true;

	has_smeter = true;

	has_voltmeter = true;

	has_power_out = true;
	has_swr_control = true;
	has_alc_control = true;
	has_sql_control = true;
	has_agc_control = true;

	has_power_control = true;
	has_volume_control = true;
	has_mode_control = true;

	has_attenuator_control = true;
	has_preamp_control = true;

	has_noise_control = true;
	has_nb_level = true;

	has_noise_reduction = true;
	has_noise_reduction_control = true;

	has_auto_notch = true;
	has_notch_control = true;
	has_pbt_controls = true;

	has_rf_control = true;

	has_ptt_control = true;
	has_tune_control = true;

	ICOMmainsub = true;

	has_xcvr_auto_on_off = true;

	has_band_selection = true;

        // JBA - these two lines broke sat mode when setting the mode of the sub R.
        // I'm not quite sure what they actually do but they are used in support.cxx and xml_server
        // A toggle added in get_sat_mode to correct this.
	can_change_alt_vfo = true;
	has_a2b = true;

	precision = 1;
	ndigits = 10;

};

//======================================================================
// IC9700 unique commands
//======================================================================

//static int ret = 0;
static bool xcvr_is_on = false;

void RIG_IC9700::set_xcvr_auto_on()
{
	cmd = pre_to;
	cmd += '\x19'; cmd += '\x00';

	get_trace(1, "getID()");

	cmd.append(post);
	RigSerial->failed(0);

	if (waitFOR(8, "get ID") == false) {
		cmd.clear();
		int fes[] = { 2, 2, 2, 3, 7, 13, 25, 50, 75, 150, 150, 150 };
		if (progStatus.serial_baudrate >= 0 && progStatus.serial_baudrate <= 11) {
			cmd.append( fes[progStatus.serial_baudrate], '\xFE');
		}
		RigSerial->WriteBuffer(cmd.c_str(), cmd.length());

		cmd.assign(pre_to);
		cmd += '\x18'; cmd += '\x01';
		set_trace(1, "power_on()");
		cmd.append(post);
		RigSerial->failed(0);

		if (waitFB("Power ON")) {
			isett("power_on()");
			xcvr_is_on = true;
			cmd = pre_to; cmd += '\x19'; cmd += '\x00';
			get_trace(1, "getID()");
			cmd.append(post);
			int i = 0;
			for (i = 0; i < 100; i++) { // 10 second total timeout
				if (waitFOR(8, "get ID", 100) == true) {
					RigSerial->failed(0);
					return;
				}
				update_progress(i / 2);
				Fl::awake();
			}
			RigSerial->failed(0);
			return;
		}

		isett("power_on()");
		RigSerial->failed(1);
		xcvr_is_on = false;
		return;
	}
	xcvr_is_on = true;
}

void RIG_IC9700::set_xcvr_auto_off()
{
	cmd.clear();
	cmd.append(pre_to);
	cmd += '\x18'; cmd += '\x00';
	cmd.append(post);
	waitFB("Power OFF", 200);
}

void RIG_IC9700::selectA()
{
  // JBA - select Main Band if rig is in sat mode and VFO A if it is not
  // This is a change from version 1.4.5
#ifdef DUALWATCH_SUB_AS_B
  int sat_mode = get_sat_mode() + get_dualwatch();
#else
  int sat_mode = get_sat_mode();
#endif
  
  cmd = pre_to;
  cmd += '\x07';
  if( sat_mode )
    cmd += '\xD0';          // Select Main band
  else
    cmd += '\x00';          // Select VFO A
  cmd.append(post);
  waitFB("select A");
  set_trace(2, "selectA()", str2hex(replystr.c_str(), replystr.length()));
  inuse = onA;

}

void RIG_IC9700::selectB()
{
  // JBA - select Sub Band if rig is in sat mode and VFO B if it is not
  // This is a change from version 1.4.5
#ifdef DUALWATCH_SUB_AS_B
  int sat_mode = get_sat_mode() + get_dualwatch();
#else
  int sat_mode = get_sat_mode();
#endif
  
  cmd = pre_to;
  cmd += '\x07';
  if( sat_mode )
    cmd += '\xD1';          // Select Sub band
  else
    cmd += '\x01';          // Select VFO A 
  cmd.append(post);
  waitFB("select B");
  set_trace(2, "selectB()", str2hex(replystr.c_str(), replystr.length()));
  inuse = onB;
  
}

bool RIG_IC9700::check ()
{
	std::string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	bool ok = waitFOR(11, "check vfo");
	get_trace(2, "check()", str2hex(replystr.c_str(), replystr.length()));
	return ok;
}

static int ret = 0;

unsigned long long RIG_IC9700::get_vfoA ()
{
#ifdef DUALWATCH_SUB_AS_B
  int sat_mode = get_sat_mode() + get_dualwatch();
#else
  int sat_mode = get_sat_mode();
#endif
  
  if( sat_mode ){
      
      // Rig is in satellite mode - command 0x25 doesn't work in sat mode
      // See pg 23 of IC9700 CIV manual
      if (inuse == onB)
        return A.freq;
      std::string resp = pre_fm;
      resp += '\x03';
      cmd = pre_to;
      cmd += '\x03';
      cmd.append( post );
      if (waitFOR(11, "get vfo A")) {
        size_t p = replystr.rfind(resp);
        if (p != std::string::npos) {
          if (replystr[p+5] == -1)
            A.freq = 0;
          else
            A.freq = fm_bcd_be(replystr.substr(p+5), 10);
	}
      }
      get_trace(2, "get_vfoA()", str2hex(replystr.c_str(), replystr.length()));
        
  } else {
    
      // Rig is not in satellite mode
      // The 0x25 command only affects the Main Band VFOs
	std::string resp;

	cmd.assign(pre_to).append("\x25");
	resp.assign(pre_fm).append("\x25");

	if (inuse == onB) {
		cmd  += '\x01';
		resp += '\x01';
	} else {
		cmd  += '\x00';
		resp += '\x00';
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

  }

	return A.freq;
}

void RIG_IC9700::set_vfoA (unsigned long long freq)
{
  A.freq = freq;

#ifdef DUALWATCH_SUB_AS_B
  int sat_mode = get_sat_mode() + get_dualwatch();
#else
  int sat_mode = get_sat_mode();
#endif
    
  if( sat_mode ){
      
      // Rig is in satellite mode - command 0x25 doesn't work in sat mode
      cmd = pre_to;
      cmd += '\x05';
      cmd.append( to_bcd_be( freq, 10 ) );
      cmd.append( post );
      waitFB("set vfo A");
      set_trace(2, "set_vfoA()", str2hex(replystr.c_str(), replystr.length()));
        
  } else {
    
      // Rig is not in satellite mode 
	cmd.assign(pre_to).append("\x25");
	if (inuse == onB) cmd += '\x01';
	else      cmd += '\x00';

	cmd.append( to_bcd_be( freq, 10) );
	cmd.append( post );

	set_trace(1, "set_vfoA");
	waitFB("set vfo A");
	seth();
      
  }
}

unsigned long long RIG_IC9700::get_vfoB ()
{
#ifdef DUALWATCH_SUB_AS_B
  int sat_mode = get_sat_mode() + get_dualwatch();
#else
  int sat_mode = get_sat_mode();
#endif
    
  if( sat_mode ){
      
      // Rig is in satellite mode - command 0x25 doesn't work in sat mode
      if (inuse != onB) return B.freq;
      std::string resp = pre_fm;
      resp += '\x03';
      cmd = pre_to;
      cmd += '\x03';
      cmd.append( post );
      if (waitFOR(11, "get vfo B")) {
        size_t p = replystr.rfind(resp);
        if (p != std::string::npos) {
          if (replystr[p+5] == -1)
            A.freq = 0;
          else
            B.freq = fm_bcd_be(replystr.substr(p+5), 10);
	}
      }
      get_trace(2, "get_vfoB()", str2hex(replystr.c_str(), replystr.length()));
        
  } else {

      // Rig is not in satellite mode 
	std::string resp;

	cmd.assign(pre_to).append("\x25");
	resp.assign(pre_fm).append("\x25");

	if (inuse == onB) {
		cmd  += '\x00';
		resp += '\x00';
	} else {
		cmd  += '\x01';
		resp += '\x01';
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

  }

	return B.freq;
}

void RIG_IC9700::set_vfoB (unsigned long long freq)
{
  B.freq = freq;

#ifdef DUALWATCH_SUB_AS_B
  int sat_mode = get_sat_mode() + get_dualwatch();
#else
  int sat_mode = get_sat_mode();
#endif
    
  if( sat_mode ){
    
      // Rig is in satellite mode - command 0x25 doesn't work in sat mode
      cmd = pre_to;
      cmd += '\x05';
      cmd.append( to_bcd_be( freq, 10 ) );
      cmd.append( post );
      waitFB("set vfo B");
      set_trace(2, "set_vfoB()", str2hex(replystr.c_str(), replystr.length()));
        
  } else {

      // Rig is not in satellite mode 
	cmd.assign(pre_to).append("\x25");
	if (inuse == onB) cmd += '\x00';
	else      cmd += '\x01';

	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );

	set_trace(1, "set_vfoB");
	waitFB("set vfo B");
	seth();

  }
  
}

void RIG_IC9700::set_modeA(int val)
{
	A.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC9700_mode_nbr[val];
	cmd.append( post );
	waitFB("set mode A");
	std::stringstream ss;
	ss << "set_modeA(" << val << ") [" << IC9700modes_[val] << "] " <<
		str2hex(replystr.c_str(), replystr.length());
	set_trace(1, ss.str().c_str());

	cmd = pre_to;
	cmd += '\x1A'; cmd += '\x06';
	if (A.imode > RTTY9700R) cmd += '\x01';
	else cmd += '\x00';
	cmd.append( post);
	waitFB("data mode");
	set_trace(2, "set_data_modeA()", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC9700::get_modeA()
{
	int md = A.imode;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	std::string resp = pre_fm;
	resp += '\x04';
	int ret = waitFOR(8, "get mode/bw A");

	std::stringstream ss;
	ss << "get_modeA() " << str2hex(replystr.c_str(), replystr.length());
	get_trace(2, "get_modeA()", ss.str().c_str());

	if (ret) {
		size_t p = replystr.find(resp);
		if (p != std::string::npos) {

			if (replystr[p+5] == -1) { A.imode = 0; return A.imode; }

			for (md = 0; md < nummodes; md++)
				if (replystr[p+5] == IC9700_mode_nbr[md])
					break;
			if (md == nummodes) {
				checkresponse();
				return A.imode;
			}
		}
	} else {
		checkresponse();
		return A.imode;
	}

	if (md < CW9700) { // check for DATA  / DVR
		cmd = pre_to;
		cmd += "\x1A\x06";
		cmd.append(post);
		resp = pre_fm;
		resp += "\x1A\x06";
		if (waitFOR(9, "data mode?")) {
			size_t p = replystr.rfind(resp);
			if (p != std::string::npos) {
				if ((replystr[p+6] & 0x01) == 0x01) {
					if (md == LSB9700) md = LSB9700D;
					if (md == USB9700) md = USB9700D;
					if (md == AM9700) md = AM9700D;
					if (md == FM9700) md = FM9700D;
					if (md == DV9700) md = DVR9700;
				}
			}
		}
		get_trace(2,"get_data_modeA", str2hex(replystr.c_str(), replystr.length()));
	}

	A.imode = md;

	return A.imode;
}

void RIG_IC9700::set_modeB(int val)
{
	B.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC9700_mode_nbr[val];
	cmd.append( post );
	waitFB("set mode B");
	std::stringstream ss;
	ss << "set_modeB(" << val << ") [" << IC9700modes_[val] << "] " <<
		str2hex(replystr.c_str(), replystr.length());
	set_trace(1, ss.str().c_str());

	cmd = pre_to;
	cmd += '\x1A'; cmd += '\x06';
	if (B.imode > RTTY9700R) cmd += '\x01';
	else cmd += '\x00';
	cmd.append( post);
	waitFB("data mode");
	set_trace(2, "set_data_modeB()", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC9700::get_modeB()
{
	int md = B.imode;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	std::string resp = pre_fm;
	resp += '\x04';
	int ret = waitFOR(8, "get mode/bw B");

	std::stringstream ss;
	ss << "get_modeA() " << str2hex(replystr.c_str(), replystr.length());
	get_trace(1, ss.str().c_str());

	if (ret) {
		size_t p = replystr.find(resp);
		if (p != std::string::npos) {

			if (replystr[p+5] == -1) { B.imode = 0; return B.imode; }

			for (md = 0; md < nummodes; md++)
				if (replystr[p+5] == IC9700_mode_nbr[md])
					break;
			if (md == nummodes) {
				checkresponse();
				return B.imode;
			}
		}
	} else {
		checkresponse();
		return B.imode;
	}

	if (md < CW9700) { // check for DATA  / DVR
		cmd = pre_to;
		cmd += "\x1A\x06";
		cmd.append(post);
		resp = pre_fm;
		resp += "\x1A\x06";
		if (waitFOR(9, "data mode?")) {
			size_t p = replystr.rfind(resp);
			if (p != std::string::npos) {
				if ((replystr[p+6] & 0x01) == 0x01) {
					if (md == LSB9700) md = LSB9700D;
					if (md == USB9700) md = USB9700D;
					if (md == AM9700) md = AM9700D;
					if (md == FM9700) md = FM9700D;
					if (md == DV9700) md = DVR9700;
				}
			}
		}
		get_trace(2,"get_data_modeB", str2hex(replystr.c_str(), replystr.length()));
	}

	B.imode = md;

	return B.imode;
}

int RIG_IC9700::get_bwA()
{
	if (A.imode == DV9700 || A.imode == DVR9700 ||
		A.imode == FM9700 || A.imode == FM9700D) return 0;

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	std::string resp = pre_fm;
	resp.append("\x1a\x03");
	int ret = waitFOR(8, "get_bw A");

	get_trace(2, "get_bwA()", str2hex(replystr.c_str(), replystr.length()));

	if (ret) {
		size_t p = replystr.rfind(resp);
		A.iBW = fm_bcd(replystr.substr(p+6), 2);
	}
	return A.iBW;
}

void RIG_IC9700::set_bwA(int val)
{
	A.iBW = val;
	if (A.imode == DV9700 || A.imode == DVR9700 ||
		A.imode == FM9700 || A.imode == FM9700D) return;

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(A.iBW, 2));
	cmd.append(post);

	waitFB("set bw A");

	std::stringstream ss;
	const char *bwstr = IC9700_ssb_bws[val];
	if ((A.imode == AM9700) || (A.imode == AM9700D)) bwstr = IC9700_am_bws[val];
	if ((A.imode == RTTY9700) || (A.imode == RTTY9700R)) bwstr = IC9700_rtty_bws[val];
	 ss << "set_bwA(" << val << ") [" << bwstr << "] " <<
		str2hex(replystr.data(), replystr.length());
	set_trace(1, ss.str().c_str());

}

int RIG_IC9700::get_bwB()
{
	if (B.imode == DV9700 || B.imode == DVR9700 ||
		B.imode == FM9700 || B.imode == FM9700D) return 0;

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	std::string resp = pre_fm;
	resp.append("\x1a\x03");
	int ret = waitFOR(8, "get_bw B");

	get_trace(2, "get_bwB()", str2hex(replystr.c_str(), replystr.length()));

	if (ret) {
		size_t p = replystr.rfind(resp);
		B.iBW = fm_bcd(replystr.substr(p+6), 2);
	}
	return B.iBW;
}

void RIG_IC9700::set_bwB(int val)
{
	B.iBW = val;
	if (B.imode == DV9700 || B.imode == DVR9700 ||
		B.imode == FM9700 || B.imode == FM9700D) return;

	if (B.imode == 5) return;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(B.iBW, 2));
	cmd.append(post);
	waitFB("set bw B");

	std::stringstream ss;
	const char *bwstr = IC9700_ssb_bws[val];
	if ((B.imode == AM9700) || (B.imode == AM9700D)) bwstr = IC9700_am_bws[val];
	if ((B.imode == RTTY9700) || (B.imode == RTTY9700R)) bwstr = IC9700_rtty_bws[val];
	ss << "set_bwB(" << val << ") [" << bwstr << "] " <<
		str2hex(replystr.data(), replystr.length());
	set_trace(1, ss.str().c_str());

}

int RIG_IC9700::adjust_bandwidth(int m)
{
	int bw = 0;
	switch (m) {
		case AM9700: case AM9700D:
			bandwidths_ = IC9700_am_bws;
			bw_vals_ = IC9700_bw_vals_AM;
			bw = 19;
			break;
		case CW9700: case CW9700R:
			bandwidths_ = IC9700_ssb_bws;
			bw_vals_ = IC9700_bw_vals_SSB;
			bw = 12;
			break;
		case RTTY9700: case RTTY9700R:
			bandwidths_ = IC9700_ssb_bws;
			bw_vals_ = IC9700_bw_vals_RTTY;
			bw = 12;
			break;
		case FM9700: case FM9700D :
		case DV9700: case DVR9700 :
			bandwidths_ = IC9700_fixed_bws;
			bw_vals_ = IC9700_bw_vals_fixed;
			bw = 0;
			break;
		case LSB9700: case USB9700:
		case LSB9700D: case USB9700D:
		default:
			bandwidths_ = IC9700_ssb_bws;
			bw_vals_ = IC9700_bw_vals_SSB;
			bw = 34;
	}
	return bw;
}

const char ** RIG_IC9700::bwtable(int m)
{
	const char ** table;
	switch (m) {
		case AM9700: case AM9700D:
			table = IC9700_am_bws;
			break;
		case FM9700: case FM9700D :
		case DV9700: case DVR9700 :
			table = IC9700_fixed_bws;
			break;
		case CW9700: case CW9700R:
		case RTTY9700: case RTTY9700R:
		case LSB9700: case USB9700:
		case LSB9700D: case USB9700D:
		default:
			table = IC9700_ssb_bws;
	}
	return table;
}

int RIG_IC9700::def_bandwidth(int m)
{
	int bw = 0;
	switch (m) {
		case AM9700: case AM9700D:
			bw = 19;
			break;
		case DV9700: case DVR9700:
		case FM9700: case FM9700D:
			bw = 0;
			break;
		case RTTY9700: case RTTY9700R:
			bw = 12;
			break;
		case CW9700: case CW9700R:
			bw = 12;
			break;
		case LSB9700: case USB9700:
		case LSB9700D: case USB9700D:
		default:
			bw = 34;
	}
	return bw;
}

void RIG_IC9700::set_mic_gain(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set mic gain");
	set_trace(2, "set_mic_gain()", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC9700::get_mic_gain()
{
	int val = 0;
	std::string cstr = "\x14\x0B";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get mic")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p + 6));
	}
	get_trace(2, "get_mic_gain()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC9700::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_IC9700::set_compression(int on, int val)
{
	if (on) {
		cmd.assign(pre_to).append("\x14\x0E");
		cmd.append(bcd255(val));
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
	set_trace(2, "set_compression()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9700::set_vox_onoff()
{
	cmd.assign(pre_to).append("\x16\x46");
	if (progStatus.vox_onoff) {
		cmd += '\x01';
		cmd.append( post );
		waitFB("set vox ON");
	} else {
		cmd += '\x00';
		cmd.append( post );
		waitFB("set vox OFF");
	}
	set_trace(2, "set_vox_onoff()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9700::set_vox_gain()
{
	cmd.assign(pre_to).append("\x1A\x05");
	cmd +='\x01';
	cmd +='\x25';
	cmd.append(bcd255(progStatus.vox_gain));
	cmd.append( post );
	waitFB("SET vox gain");
	set_trace(2, "set_vox_gain()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9700::set_vox_anti()
{
	cmd.assign(pre_to).append("\x1A\x05");
	cmd +='\x01';
	cmd +='\x26';
	cmd.append(bcd255(progStatus.vox_anti));
	cmd.append( post );
	waitFB("SET anti-vox");
	set_trace(2, "set_vox_anti()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9700::set_vox_hang()
{
	cmd.assign(pre_to).append("\x1A\x05");	//values 00-20 = 0.0 - 2.0 sec
	cmd +='\x01';
	cmd +='\x27';
	cmd.append(to_bcd((int)(progStatus.vox_hang / 10 ), 2));
	cmd.append( post );
	waitFB("SET vox hang");
	set_trace(2, "set_vox_hang()", str2hex(replystr.c_str(), replystr.length()));
}

// these need to be written and tested
void  RIG_IC9700::get_vox_onoff()
{
}

void  RIG_IC9700::get_vox_gain()
{
}

void  RIG_IC9700::get_vox_anti()
{
}

void  RIG_IC9700::get_vox_hang()
{
}

// CW controls

void RIG_IC9700::set_cw_wpm()
{
	cmd.assign(pre_to).append("\x14\x0C"); // values 0-255
	cmd.append(to_bcd(round((progStatus.cw_wpm - 6) * 255 / (60 - 6)), 3));
	cmd.append( post );
	waitFB("SET cw wpm");
	set_trace(2, "set_cw_wpm()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9700::set_cw_qsk()
{
	int n = round(progStatus.cw_qsk * 10); // values 0-255
	cmd.assign(pre_to).append("\x14\x0F");
	cmd.append(to_bcd(n, 3));
	cmd.append(post);
	waitFB("Set cw qsk delay");
	set_trace(2, "set_cw_qsk()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9700::set_cw_spot_tone()
{
	cmd.assign(pre_to).append("\x14\x09"); // values 0=300Hz 255=900Hz
	int n = round((progStatus.cw_spot_tone - 300) * 255.0 / 600.0);
	if (n > 255) n = 255;
	if (n < 0) n = 0;
	cmd.append(to_bcd(n, 3));
	cmd.append( post );
	waitFB("SET cw spot tone");
	set_trace(2, "set_cw_spot_tone()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9700::set_cw_vol()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x24';
	cmd.append(to_bcd((int)(progStatus.cw_vol * 2.55), 3));
	cmd.append( post );
	waitFB("SET cw sidetone volume");
	set_trace(2, "set_cw_vol()", str2hex(replystr.c_str(), replystr.length()));
}

// Volume control val 0 ... 100
void RIG_IC9700::set_volume_control(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set vol");
	set_trace(2, "set_volume_control()", str2hex(replystr.c_str(), replystr.length()));
}

/*

I:12:20:22: get vol ans in 0 ms, OK
cmd FE FE 7A E0 14 01 FD
ans FE FE 7A E0 14 01 FD
FE FE E0 7A 14 01 00 65 FD
 0  1  2  3  4  5  6  7  8
*/
int RIG_IC9700::get_volume_control()
{
	int val = 0;
	std::string cstr = "\x14\x01";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(9, "get vol")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p + 6));
	}
	get_trace(2, "get_volume_control()", str2hex(replystr.c_str(), replystr.length()));
	return (val);
}

void RIG_IC9700::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_IC9700::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set power");
	set_trace(2, "set_power_control()", str2hex(replystr.c_str(), replystr.length()));
}

double RIG_IC9700::get_power_control()
{
	int val = progStatus.power_level;
	std::string cstr = "\x14\x0A";
	std::string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);
	if (waitFOR(9, "get power")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p+6));
	}
	get_trace(2, "get_power_control()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC9700::get_pc_min_max_step(double &min, double &max, double &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_IC9700::set_rf_gain(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set RF");
	set_trace(2, "set_rf_gain()", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC9700::get_rf_gain()
{
	int val = progStatus.rfgain;
	std::string cstr = "\x14\x02";
	std::string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);
	if (waitFOR(9, "get RF")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p + 6));
	}
	get_trace(2, "get_rf_gain()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC9700::get_rf_min_max_step(double &min, double &max, double &step)
{
	min = 0; max = 100; step = 1;
}

int RIG_IC9700::get_smeter()
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
			mtr = (int)ceil(mtr /2.40);
			if (mtr > 100) mtr = 100;
		}
	}
	get_trace(2, "get_smeter()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
}

double RIG_IC9700::get_voltmeter()
{
	std::string cstr = "\x15\x15";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	int mtr = 0;
	double val = 0;

	get_trace(1, "get_voltmeter()");
	int ret = waitFOR(9, "get voltmeter");
	geth();

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			val = 6.0 * mtr / 228.0 + 9.7;
			return val;
		}
	}
	return -1;
}

int RIG_IC9700::get_power_out()
{
	std::string cstr = "\x15\x11";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get pout")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr /2.15);
			if (mtr > 100) mtr = 100;
		}
	}
	get_trace(2, "get_power_out()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
}

int RIG_IC9700::get_swr()
{
	std::string cstr = "\x15\x12";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get SWR")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr /2.40);
			if (mtr > 100) mtr = 100;
		}
	}
	get_trace(2, "get_swr()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
}

int RIG_IC9700::get_alc()
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
	get_trace(2, "get_alc()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
}

void RIG_IC9700::set_notch(bool on, int val)
{
	int notch = (int)(val * 256.0 / 3000.0);

	cmd = pre_to;
	cmd.append("\x16\x48");
	cmd += on ? '\x01' : '\x00';
	cmd.append(post);
	waitFB("set notch");

	cmd = pre_to;
	cmd.append("\x14\x0D");
	cmd.append(to_bcd(notch,3));
	cmd.append(post);
	waitFB("set notch val");

	std::stringstream ss;
	ss << "set_notch(" << on << ", " << val << ") " << str2hex(replystr.data(), replystr.length());
	set_trace(2, "set_notch()", ss.str().c_str());

}

bool RIG_IC9700::get_notch(int &val)
{
	bool on = false;
	val = 0;

	std::string cstr = "\x16\x48";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get notch")) {
		get_trace(2, "get_notch()", str2hex(replystr.c_str(), replystr.length()));
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
			get_trace(2, "get_notch_val()", str2hex(replystr.c_str(), replystr.length()));
			size_t p = replystr.rfind(resp);
			if (p != std::string::npos)
				val = (int)ceil(fm_bcd(replystr.substr(p+6),3) * 3000.0 / 255.0);
		}
	}
	return on;
}

void RIG_IC9700::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 3000;
	step = 20;
}

void RIG_IC9700::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd += val ? 0x01 : 0x00;
	cmd.append( post );
	waitFB("set AN");
	set_trace(2, "set_auto_notch()", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC9700::get_auto_notch()
{
	std::string cstr = "\x16\x41";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get AN")) {
		get_trace(2, "get_auto_notch()", str2hex(replystr.c_str(), replystr.length()));
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+6] == 0x01) {
				auto_notch_label("AN", true);
				return true;
			} else {
				auto_notch_label("AN", false);
				return false;
			}
		}
	}
	return progStatus.auto_notch;
}

void RIG_IC9700::set_split(bool val)
{
	split = val;
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB(val ? "set split ON" : "set split OFF");
	set_trace(2, "set_split()", str2hex(replystr.c_str(), replystr.length()));
}

int  RIG_IC9700::get_split()
{
	return split;
}

static int agcval = 1;
int  RIG_IC9700::get_agc()
{
	cmd = pre_to;
	cmd.append("\x16\x12");
	cmd.append(post);
	if (waitFOR(8, "get AGC")) {
		size_t p = replystr.find(pre_fm);
		if (p == std::string::npos) return agcval;
		return (agcval = replystr[p+6]); // 1 = FAST, 2 = MID, 3 = SLOW
	}
	get_trace(2, "get_agc()", str2hex(replystr.c_str(), replystr.length()));
	return agcval;
}

int RIG_IC9700::incr_agc()
{
	agcval++;
	if (agcval == 4) agcval = 1;
	cmd = pre_to;
	cmd.append("\x16\x12");
	cmd += agcval;
	cmd.append(post);
	waitFB("set AGC");
	set_trace(2, "incr_agc()", str2hex(replystr.c_str(), replystr.length()));
	return agcval;
}


static const char *agcstrs[] = {"FST", "MID", "SLO"};
const char *RIG_IC9700::agc_label()
{
	return agcstrs[agcval - 1];
}

int  RIG_IC9700::agc_val()
{
	return (agcval);
}

void RIG_IC9700::set_attenuator(int val)
{
	if (val) {
		atten_label("10 dB", true);
		atten_level = 1;
	} else {
		atten_label("ATT", false);
		atten_level = 0;
	}

	cmd = pre_to;
	cmd += '\x11';
	cmd += atten_level ? '\x10' : '\x00';
	cmd.append( post );
	waitFB("set att");
	std::stringstream ss;
	ss << "set_attenuator(" << val << ") " << str2hex(replystr.data(), replystr.length());
	set_trace(1, ss.str().c_str());

}

int RIG_IC9700::next_attenuator()
{
	if (atten_level) return 0;
	return 1;
}

int RIG_IC9700::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	std::string resp = pre_fm;
	resp += '\x11';
	if (waitFOR(7, "get ATT")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (!replystr[p+5]) {
				atten_label("ATT", false);
				atten_level = 0;
				return 0;
			} else {
				atten_label("10 dB", true);
				atten_level = 1;
				return 1;
			}
		}
	}
	get_trace(2, "get_attenuator()", str2hex(replystr.data(), replystr.length()));
	return 0;
}

int RIG_IC9700::next_preamp()
{
	switch (preamp_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 3;
		default:
		case 3: return 0;
	}
	return 0;
}

void RIG_IC9700::set_preamp(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';

	switch (val) {
		default:
		case 0 :
			preamp_label("P0/E0", false);
			preamp_level = 0;
			break;
		case 1 :
			preamp_label("P1/E0", true);
			preamp_level = 1;
			break;
		case 2 :
			preamp_label("P0/E1", true);
			preamp_level = 2;
			break;
		case 3 :
			preamp_label("P1/E1", true);
			preamp_level = 3;
			break;
	}
	cmd += preamp_level;

	cmd.append( post );
	waitFB("set Pre");

	std::stringstream ss;
	ss << "set_preamp(" << val << ") " << str2hex(replystr.data(), replystr.length());
	set_trace(1, ss.str().c_str());

}

int RIG_IC9700::get_preamp()
{
	std::string cstr = "\x16\x02";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get Pre")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			switch (replystr[p+6]) {
			default:
			case 0 :
				preamp_label("P0/E0", false);
				preamp_level = 0;
				break;
			case 1 :
				preamp_label("P1/E0", true);
				preamp_level = 1;
				break;
			case 2 :
				preamp_label("P0/E1", true);
				preamp_level = 2;
				break;
			case 3 :
				preamp_label("P1/E1", true);
				preamp_level = 3;
				break;
			}
		}
	}
	get_trace(2, "get_preamp()", str2hex(replystr.c_str(), replystr.length()));

	return preamp_level;
}

// Tranceiver PTT on/off
void RIG_IC9700::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	waitFB("set ptt");

	std::stringstream ss;
	ss << "set_PTT_control(" << val << ") " << str2hex(replystr.data(), replystr.length());
	set_trace(1, ss.str().c_str());

	ptt_ = val;
}

int RIG_IC9700::get_PTT()
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
	get_trace(2, "get_PTT()", str2hex(replystr.data(), replystr.length()));
	return ptt_;
}

void RIG_IC9700::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set noise");
	set_trace(2, "set_noise()", str2hex(replystr.data(), replystr.length()));
}

int RIG_IC9700::get_noise()
{
	int val = progStatus.noise;
	std::string cstr = "\x16\x22";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(8, "get noise")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			val = replystr[p+6];
		}
	}
	get_trace(2, "get_noise()", str2hex(replystr.data(), replystr.length()));
	return val;
}

void RIG_IC9700::set_nb_level(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x12");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set NB level");
	set_trace(2, "set_nb_level()", str2hex(replystr.data(), replystr.length()));
}

int  RIG_IC9700::get_nb_level()
{
	int val = progStatus.nb_level;
	std::string cstr = "\x14\x12";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get NB level")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p+6));
	}
	get_trace(2, "get_nb_level()", str2hex(replystr.data(), replystr.length()));
	return val;
}

void RIG_IC9700::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set NR");
	get_trace(2, "get_noise_reduction()", str2hex(replystr.data(), replystr.length()));
}

int RIG_IC9700::get_noise_reduction()
{
	std::string cstr = "\x16\x40";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(8, "get NR")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			return (replystr[p+6] ? 1 : 0);
	}
	get_trace(2, "get_noise_reduction()", str2hex(replystr.data(), replystr.length()));
	return progStatus.noise_reduction;
}

/*

I:12:06:50: get NR ans in 0 ms, OK
cmd FE FE 7A E0 16 40 FD
ans FE FE 7A E0 16 40 FD
FE FE E0 7A 16 40 01 FD
 0  1  2  3  4  5  6  7

I:12:06:50: get NRval ans in 0 ms, OK
cmd FE FE 7A E0 14 06 FD
ans FE FE 7A E0 14 06 FD
FE FE E0 7A 14 06 00 24 FD
 0  1  2  3  4  5  6  7  8

*/

void RIG_IC9700::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	val *= 16;
	val += 8;
	cmd.append(to_bcd(val, 3));
	cmd.append(post);
	waitFB("set NRval");
	set_trace(2, "set_noise_reduction_val()", str2hex(replystr.data(), replystr.length()));
}

int RIG_IC9700::get_noise_reduction_val()
{
	int val = progStatus.noise_reduction_val;
	std::string cstr = "\x14\x06";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get NRval")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			val = fm_bcd(replystr.substr(p+6),3);
			val -= 8;
			val /= 16;
		}
	}
	get_trace(2, "get_noise_reduction_val()", str2hex(replystr.data(), replystr.length()));
	return val;
}

void RIG_IC9700::set_squelch(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set Sqlch");
	set_trace(2, "set_squelch()", str2hex(replystr.data(), replystr.length()));
}

int  RIG_IC9700::get_squelch()
{
	int val = progStatus.squelch;
	std::string cstr = "\x14\x03";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get squelch")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p+6));
	}
	get_trace(2, "get_squelch()", str2hex(replystr.data(), replystr.length()));
	return val;
}

void RIG_IC9700::set_if_shift(int val)
{
	int shift;
	sh_ = val;
	if (val == 0) sh_on_ = false;
	else sh_on_ = true;

	shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x07");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set IF on/off");
	set_trace(2, "set_if_shift()", str2hex(replystr.data(), replystr.length()));

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set IF val");
	set_trace(2, "set_if_shift_val()", str2hex(replystr.data(), replystr.length()));
}

bool RIG_IC9700::get_if_shift(int &val) {
	val = sh_;
	return sh_on_;
}

void RIG_IC9700::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -50;
	max = +50;
	step = 1;
}

void RIG_IC9700::set_pbt_inner(int val)
{
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x07");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set PBT inner");
	set_trace(2, "set_pbt_inner()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9700::set_pbt_outer(int val)
{
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set PBT outer");
	set_trace(2, "set_pbt_outer()", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC9700::get_pbt_inner()
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
	get_trace(2, "get_pbt_inner()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

int RIG_IC9700::get_pbt_outer()
{
	int val = 0;
	std::string cstr = "\x14\x08";
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
	get_trace(2, "get_pbt_outer()", str2hex(replystr.c_str(), replystr.length()));
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

void RIG_IC9700::get_band_selection(int v)
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
			if ((bandmode == 0) && banddata) bandmode = 9;
			if ((bandmode == 1) && banddata) bandmode = 10;
			if ((bandmode == 2) && banddata) bandmode = 11;
			if ((bandmode == 3) && banddata) bandmode = 12;
			if (inuse == onB) {
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

void RIG_IC9700::set_band_selection(int v)
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


// JBA - routine to determine if rig is in satellite mode
// Be sure to call this before forming another command as
// the variable cmd is common to all of these routines.
int RIG_IC9700::get_sat_mode()
{
  int ret;
  int iret=-1;
  std::string resp;

  // Read sat mode
  cmd = pre_to;
  cmd.append("\x16\x5a");
  cmd.append(post);
  resp = pre_fm;
  resp.append("\x16\x5a");
  ret = waitFOR(8, "get_sat_mode");
  get_trace(2, "get_sat_mode()", str2hex(replystr.c_str(), replystr.length()));

  if (ret) {
    size_t p = replystr.rfind(resp);
    iret = fm_bcd(replystr.substr(p+6), 2);
  }

  // These two flags are used elsewhere and broke sat mode - only allow them to
  // be set if not in sat mode
  if( iret ) 
    can_change_alt_vfo = has_a2b = false;
  else 
    can_change_alt_vfo = has_a2b = true;

  return iret;
}



// JBA - routine to determine if rig is in dualwatch mode.
// Be sure to call this before forming another command as
// the variable cmd is common to all of these routines.
int RIG_IC9700::get_dualwatch()
{
  int ret;
  int iret=-1;
  std::string resp;

  // Read sat mode
  cmd = pre_to;
  cmd.append("\x16\x59");
  cmd.append(post);
  resp = pre_fm;
  resp.append("\x16\x59");
  ret = waitFOR(8, "get_dualwatch");
  get_trace(2, "get_dualwatch()", str2hex(replystr.c_str(), replystr.length()));

  if (ret) {
    size_t p = replystr.rfind(resp);
    iret = fm_bcd(replystr.substr(p+6), 2);
  }

  return iret;
}



