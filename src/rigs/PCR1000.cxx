/*
 * Icom PCR-1000 driver
 * a part of flrig.
 *
 * Copyright 2009, Dave Freese, W1HKJ
 * Driver for PCR-1000 April 2012, Brian Miezejewski, k5hfi
 */

/* Todo:
 *
 *	1) Need on/off button to turn the radio on and off?
 *	2) Up the baud rate to 37800 after connect
 *	3) Support for fast mode.
 *	4) Scan support for scanning between a frequency pair
 *	5) Scan support for scanning a list of frequencies stored in flrig.
 *	6) Implement band scope.
 *	7) BFO support?
 *
 */

/* Notes:
 *	The Icom PCR-1000 differs for most of the other rigs controlled by flrig in
 *	that settings do not change at the rig, they are all controlled by flrig. There is
 *	no reason to poll for frequency, volume, mode, etc., they simply can't change on their own.
 *	On the other hand, things like s-meter reading do change, but the PCR-1000 will send these
 *	with no poll needed when the receiver is in "fast" mode.
 *
 *	Note that while the PCR-1000 does not have a second VFO, we have virtualized a second
 *	vfo in the PCR-1000 flrig implementation. Each VFO stores its frequency, mode, and bandwidth.
 *	The FREQMODE variable is used to store the VFO states.
 */


#include "config.h"
#include "PCR1000.h"
#include "support.h"

const char RIG_PCR1000::name[] = "PCR-1000";

//	   mode array Index Values :-         0      1      2     3     4     5
const char *RIG_PCR1000::modes[] = {   "LSB", "USB", "CW", "AM", "NFM", "WFM", NULL};
const char RIG_PCR1000::mode_chr[] =  { '1',   '2',   '3',  '4',  '5',  '6' };
const char RIG_PCR1000::mode_type[] = { 'L',   'U',   'U',  'U',  'U',  'U' };


//	   band width array Index Values :-    0      1    2     3     4
const char *RIG_PCR1000::band_widths[] = { "2.8k","6k","15k","50k","230k",NULL};

//----------------------------------------------------------------------

// Array used for the conversion of hex values to a character string
const char RIG_PCR1000::hex_chars[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' } ;

//----------------------------------------------------------------------

char 		RIG_PCR1000::volume_command[] 		= 		"J40XX\r\n" ;
char 		RIG_PCR1000::squelch_command[] 		= 		"J41XX\r\n" ;
char 		RIG_PCR1000::if_shift_command[] 	= 		"J43XX\r\n" ;
const char  RIG_PCR1000::noise_off_command[] 	= 		"J4600\r\n" ;
const char  RIG_PCR1000::noise_on_command[] 	= 		"J4601\r\n" ;
const char  RIG_PCR1000::att_off_command[] 		= 		"J4700\r\n" ;
const char  RIG_PCR1000::att_on_command[] 		= 		"J4701\r\n" ;
char 		RIG_PCR1000::check_power_command[] 	= 		"H1?" ;
char 		RIG_PCR1000::power_on_command[] 	= 		"H101\r\n" ;
char 		RIG_PCR1000::power_off_command[] 	= 		"H100\r\n" ;
const char 	RIG_PCR1000::get_smeter_command[] 	= 		"I1?" ;

//----------------------------------------------------------------------

//----------------------------------------------------------------------

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

//----------------------------------------------------------------------


/*
 * A PCR-1000 will start issuing repeating H100<cr><lf> to indicate its status as
 * off as soon as you connect to it. It will start at 9600-n-1 and uses rts/cts for
 * transaction control.
 */
void RIG_PCR1000::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;

	selectA();

	// Lets see if the radio is turned on.

	sendCommand(check_power_command,6);

	showresp(WARN, ASC, "Check Power", check_power_command , replystr);

	// If the radio is turned off, turn it on.

	if(replystr.rfind("H100")) {
		sendCommand(power_on_command,6);
		showresp(WARN, ASC, "Power ON", power_on_command , replystr);
	}

	// Set the radio with the default values
	set_volume_control(current_volume) ;
	set_squelch(sql);

	RIG_DEBUG = true;
}

//----------------------------------------------------------------------
RIG_PCR1000::RIG_PCR1000() : current_vfo(A) {

	// current_vfo = A ;
	name_ = name;
	modes_ = modes;
	bandwidths_ = band_widths;

	widgets = rig_widgets;

	comm_baudrate 	= BR9600;
	stopbits 		= 1;
	comm_retries 	= 2;
	comm_wait 		= 50;
	comm_timeout 	= 50;
	comm_rtscts 	= true;
	comm_rtsplus 	= false;
	comm_dtrplus 	= false;
	comm_catptt 	= false;
	comm_rtsptt 	= false;
	comm_dtrptt 	= false;

//	Defaults.

	A.imode = 1;
	A.freq = 14070000L;
	A.iBW = 0 ;

	B.imode = 1;
	B.freq = 14070000L;
	B.iBW = 0 ;

	sql 				= 127 ;		// Set squelch to a reasonable value
	if_shift 			= 0 ;		// IF shift off
	attenuator 			= 0 ;		// Attenuator is set to off
	noise 				= 0 ;		// Noise blanker to off
	current_volume 		= 25 ;		// Set volume to make at least a little noise, otherwise there is no
									// indication when the radio is switched on.


	has_micgain_control =
	has_notch_control =
	has_ifshift_control =
	has_swr_control = false;

	has_smeter =
	has_bandwidth_control =
	has_volume_control =
	has_mode_control =
	has_sql_control =
	has_noise_control =
	has_attenuator_control =
	has_ifshift_control = true;

	precision = 10 ;
	ndigits = 9 ;

}

//----------------------------------------------------------------------
void RIG_PCR1000::shutdown() {
	// Turn off the radio
	sendCommand(power_off_command,6);
	showresp(WARN, ASC, "Power ON", power_on_command , replystr);
}
//----------------------------------------------------------------------

/*
 * Set the frequency in the current mode command
 */

void RIG_PCR1000::setFreqModeBW(FREQMODE &freqMode) {

	RIG_PCR1000::freq_cmd myComm = {
			{'K','0'},
			{'0','0','0','7','2','0','0','0','0','0'},
			{'0','0'},
			{'0','0'},
			{'0','0','\r','\n','\0'}
		} ;

	// Set the mode

	myComm.mode[1] = mode_chr[ freqMode.imode ] ;

	// Set the frequency in the selected mode command

	long freq = freqMode.freq ;
	for( int pos = 9 ; pos >= 0 ; pos--,freq /= 10 ) {
		myComm.frequency[pos] = '0'+(freq%10) ;
	}

	// Set the band width

	myComm.band_width[1] = '0'+ freqMode.iBW ;


	// printf("Freq:%s",myComm.command);

	sendCommand(myComm.command,5);
	showresp(WARN, ASC, "set vfo", myComm.command, replystr);

}

//----------------------------------------------------------------------

/*
 * The PCR-1000 cannot change its frequency so it is always whatever we last set it to
 */

//----------------------------------------------------------------------
void RIG_PCR1000::set_vfoA (long freq)
{
	freqA = A.freq = freq;
	setFreqModeBW(A) ;
}

long RIG_PCR1000::get_vfoA ()
{
	return A.freq;
}


//----------------------------------------------------------------------
void RIG_PCR1000::set_vfoB (long freq)
{
	freqB = B.freq = freq;
	setFreqModeBW(B) ;
}

long RIG_PCR1000::get_vfoB ()
{
	return B.freq;
}

//======================================================================
// Some utility functions
//======================================================================
/*
 * This method converts a character to its hex values, i.e.
 * '0' = 0
 * '5' = 5
 * 'A' = 10
 *
 */
int RIG_PCR1000::hexTo(const char c) const {
	return ( c <= '9') ? c - '0' :  c - 'A' + 10 ;
}

/*
 * This method sets the value of ival into the first 2 characters in cptr, i.e.
 * 17 is converted to "11"
 * 37 is converted to "25"
 *
 */

void RIG_PCR1000::set2Hex(int ival, char *cptr) {
	cptr[0] = hex_chars[ival / 16] ;
	cptr[1] = hex_chars[ival % 16] ;
}

//----------------------------------------------------------------------
int RIG_PCR1000::get_smeter() {	 // returns 0-100

	int ret = sendCommand(get_smeter_command,6) ;

	showresp(WARN, ASC, "S meter", get_smeter_command, replystr);

	ret = hexTo(replystr.c_str()[3]) * 16 + hexTo(replystr.c_str()[4]) ;

	return (int)((float)ret * ( 100.0 / 255.0 ) ) ;
}

//----------------------------------------------------------------------
// Volume control return (rig sends back 0 .. 255)

int RIG_PCR1000::get_volume_control()
{
	return current_volume ;
}

//----------------------------------------------------------------------
void RIG_PCR1000::set_volume_control(int val) // 0 .. 100
{
	current_volume = val ;

	int ival = (int)(val * 2.55); // 0 .. 255

	set2Hex( ival, &(volume_command[3])) ;

	sendCommand(volume_command, 5);
	showresp(WARN, ASC, "set volume", volume_command, replystr);

}

//======================================================================
// Band width commands
//======================================================================
void RIG_PCR1000::set_bwA(int val) {
	A.iBW = bwA = val;
	setFreqModeBW(A);
}

int  RIG_PCR1000::get_bwA() {
	return A.iBW ;
}

void RIG_PCR1000::set_bwB(int val) {
	B.iBW = bwB = val;
	setFreqModeBW(B);
}

int  RIG_PCR1000::get_bwB() {
	return B.iBW ;
}

//======================================================================
// mode commands
//======================================================================
void RIG_PCR1000::set_modeA(int val)
{
	modeA = A.imode = val ;
	setFreqModeBW(A);
}

//----------------------------------------------------------------------
int RIG_PCR1000::get_modeA()
{
	return A.imode;
}

//----------------------------------------------------------------------
void RIG_PCR1000::set_modeB(int val)
{
	modeB = B.imode = val ;
	setFreqModeBW(B);
}

//----------------------------------------------------------------------
int RIG_PCR1000::get_modeB()
{
	return B.imode;
}

//----------------------------------------------------------------------
int RIG_PCR1000::get_modetype(int n)
{
	return mode_type[n];
}


//======================================================================
// mode commands
//======================================================================
//----------------------------------------------------------------------
void RIG_PCR1000::selectA() {
	current_vfo = A ;
}
//----------------------------------------------------------------------
void RIG_PCR1000::selectB() {
	current_vfo = B ;
}
//----------------------------------------------------------------------
void RIG_PCR1000::A2B() {
	B.freq = A.freq ;
	B.iBW = A.iBW ;
	B.imode = A.imode ;
	B.src = A.src ;
}
//----------------------------------------------------------------------
void RIG_PCR1000::swapAB() {
	FREQMODE T ;

	T.freq = B.freq ;
	T.iBW = B.iBW ;
	T.imode = B.imode ;
	T.src = B.src ;

	B.freq = A.freq ;
	B.iBW = A.iBW ;
	B.imode = A.imode ;
	B.src = A.src ;

	A.freq = T.freq ;
	A.iBW = T.iBW ;
	A.imode = T.imode ;
	A.src = T.src ;
}

//======================================================================
// Squelch commands
//======================================================================
//----------------------------------------------------------------------
void RIG_PCR1000::set_squelch(int val) {

	sql = val ;
	set2Hex( val, &(squelch_command[3])) ;
	sendCommand(squelch_command, 5);
	showresp(WARN, ASC, "S meter", squelch_command, replystr);
}

int  RIG_PCR1000::get_squelch() {
	return sql ;
}

//======================================================================
// IF shift commands
//======================================================================
//----------------------------------------------------------------------

/*
 * Since the PCR1000 IF shift shifts in 10 hertz increments we display and store
 * the actual shift. Just dived by 10 and and 80 before its set.
 *
 */
void RIG_PCR1000::set_if_shift(int val) {

	if_shift = val ;
	set2Hex( val/10 + 80, &(if_shift_command[3])) ;
	sendCommand(if_shift_command, 5);
	showresp(WARN, ASC, "S meter", if_shift_command, replystr);
}


bool RIG_PCR1000::get_if_shift(int &val) {
	val = if_shift ;
	return (if_shift == if_shift_mid)?false:true ;
}

//======================================================================
// Attenuator commands
//======================================================================
//----------------------------------------------------------------------


void RIG_PCR1000::set_attenuator(int val) {
	if( val != attenuator ) {
		if( val ) {
			// Turn att on
			sendCommand(att_on_command, 5);
			showresp(WARN, ASC, "S meter", att_on_command, replystr);
			attenuator = 1 ;
		} else {
			// Turn att off
			sendCommand(att_off_command, 5);
			showresp(WARN, ASC, "S meter", att_off_command, replystr);
			attenuator = 0 ;
		}
	}
}

int  RIG_PCR1000::get_attenuator() {
	return attenuator ;
}


//======================================================================
// Noise reduction commands (Doesn't do much on PCR-1000 :-( )
//======================================================================
//----------------------------------------------------------------------

void RIG_PCR1000::set_noise(bool on) {
	if( on ) {
		// Turn on
		sendCommand(noise_on_command, 5);
		showresp(WARN, ASC, "Noise Reduction ON", noise_on_command, replystr);
		noise = 1 ;
	} else {
		// Turn off
		sendCommand(noise_off_command, 5);
		showresp(WARN, ASC, "Noise Reduction OFF", noise_off_command, replystr);
		noise = 0 ;
	}
}

int  RIG_PCR1000::get_noise() {
	return noise ;
}


