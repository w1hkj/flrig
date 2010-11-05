/*
 * Icom IC-728
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "IC728.h"

//=============================================================================
// IC-728
//
const char IC728name_[] = "IC-728";
const char *IC728modes_[] = { "LSB", "USB", "AM", "CW", "RTTY", "FM", NULL};
const char *IC728_widths[] = { "NARR", "WIDE", NULL};

RIG_IC728::RIG_IC728() {
	name_ = IC728name_;
	modes_ = IC728modes_;
	bandwidths_ = IC728_widths;
	comm_baudrate = BR1200;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_echo = true;
	comm_rtscts = false;
	comm_rtsplus = true;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	modeA = 1;
	bwA = 0;

	defaultCIV = 0x38;
	adjustCIV(defaultCIV);

	has_attenuator_control =
	has_preamp_control =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_bandwidth_control =
	has_micgain_control =
	has_notch_control =
	has_ifshift_control =
	has_ptt_control =
	has_tune_control =
	has_swr_control = false;
};

//=============================================================================

long RIG_IC728::get_vfoA ()
{
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	if (!sendCommand(cmd, 10)) {
		checkresponse(10);
		return freqA;
	}
	freqA = fm_bcd_be(&replystr[5], 8);
	return freqA;
}

void RIG_IC728::set_vfoA (long freq)
{
	freqA = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 8 ) );
	cmd.append( post );
	sendCommand(cmd, 6);
	checkresponse(6);
}

