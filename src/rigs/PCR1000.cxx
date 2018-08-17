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
/*
 * Driver for PCR-1000 April 2012, Brian Miezejewski, k5hfi
 * patched 1/23/2017, Dave, G0WBX
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
 *	The XCVR_STATE variable is used to store the VFO states.
 */


#include "config.h"
#include "PCR1000.h"
#include "support.h"

//----------------------------------------------------------------------
inline string str(string s) 
{
	size_t p;
	while((p = s.find('\r')) != string::npos)
		s.replace(p, 1, "<cr>");
	while((p = s.find('\n')) != string::npos)
		s.replace(p, 1, "<lf>");
	return s;
}

#define strace(s, s1) set_trace(3, s, str(s1).c_str(), str(replystr).c_str());
#define gtrace(s, s1) get_trace(3, s, str(s1).c_str(), str(replystr).c_str());
 
//----------------------------------------------------------------------

const char RIG_PCR1000::name[] = "PCR-1000";

//	   mode array Index Values :-         0      1      2     3     4     5
const char *RIG_PCR1000::modes[] =    { "LSB", "USB", "AM", "CW", "NFM", "WFM", NULL};
const char RIG_PCR1000::mode_chr[] =  { '0',   '1',   '2',  '3',  '5',   '6' };
const char RIG_PCR1000::mode_type[] = { 'L',   'U',   'U',  'U',  'U',   'U' };

//	   band width array Index Values :-    0      1    2     3     4
const char *RIG_PCR1000::band_widths[] = { "2.8k","6k","15k","50k","230k",NULL};
static int PCR1000_bw_vals[] = {1,2,3,4,5,WVALS_LIMIT};

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
	modes_ = modes;
	bandwidths_ = band_widths;
	bw_vals_ = PCR1000_bw_vals;

	selectA();

	// Lets see if the radio is turned on.

	sendCommand(check_power_command,6);

	showresp(WARN, ASC, "Check Power", check_power_command , replystr);
	gtrace("Check Power", check_power_command);

	// If the radio is turned off, turn it on.

	if (replystr.rfind("H100") != std::string::npos) {
		sendCommand(power_on_command,6);
		showresp(WARN, ASC, "Power ON", power_on_command , replystr);
		strace("Power ON", power_on_command);
	}

	// Set the radio with the default values
	set_volume_control(current_volume) ;
	set_squelch(sql);
	set_if_shift(if_shift);	// mid = off					// wbx

}

bool RIG_PCR1000::check()
{
	return true;
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
	strace("Power OFF", power_off_command);
	showresp(WARN, ASC, "Power OFF", power_off_command , replystr);
}
//----------------------------------------------------------------------

/*
 * Set the frequency in the current mode command
 */

void RIG_PCR1000::setFreqModeBW(XCVR_STATE &freqMode) {

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
	strace("set vfo", myComm.command);
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
	gtrace("S meter", get_smeter_command);

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
	strace("set volume", volume_command);
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
	XCVR_STATE T ;

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
	showresp(WARN, ASC, "Set Squelch", squelch_command, replystr);
	strace("set squelch", squelch_command);
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
+ * the actual shift. Just divide by 10 and add 80 before its set.
 * 
 * The "and add 80" above is decimal, it should be in Hex, so that's 128 Decimal
 * The showresp text lable was wrong too, it's been corrected.
 *
 */
void RIG_PCR1000::set_if_shift(int val) {

	if_shift = val ;
	set2Hex( val/10 + 128, &(if_shift_command[3])) ;
	sendCommand(if_shift_command, 5);
	showresp(WARN, ASC, "IF Shift", if_shift_command, replystr);
	strace("if shift", if_shift_command);
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
			showresp(WARN, ASC, "Set Attenuator ON", att_on_command, replystr);		// wbx
			strace("set att ON", att_on_command);
			attenuator = 1 ;
		} else {
			// Turn att off
			sendCommand(att_off_command, 5);
			showresp(WARN, ASC, "Set Attenuator OFF", att_off_command, replystr);	// wbx
			strace("set att OFF", att_off_command);
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
		strace("nr ON", noise_on_command);
		noise = 1 ;
	} else {
		// Turn off
		sendCommand(noise_off_command, 5);
		showresp(WARN, ASC, "Noise Reduction OFF", noise_off_command, replystr);
		strace("nr OFF", noise_off_command);
		noise = 0 ;
	}
}

int  RIG_PCR1000::get_noise() {
	return noise ;
}

//======================================================================
// Command structures for the PCR1000
//
// From http://www.gm4jjj.co.uk/PCR1000.html
//======================================================================
/*
PCR1000 Command List

The ICOM PCR1000 Computer Controlled Radio Command Protocol has not been
published by ICOM, however I have compiled this list from various sources.

Warning: I take no responsibility for the accuracy of any of the following,
you use it at your own risk! - Make sure you have a backup of your EEPROM
before trying any experimentation!

Updated 10 JAN 2002
 
All commands are sent is ASCII in the format: Command + CR + LF
CR = chr$(13)
LF = chr$(10)
 
EXCEPT when in AUTOUPDATE mode (see G3xx command), when NO CR + LF
IS REQUIRED after the Command
 
Note that no G0XX is returned in autoupdate mode
 
Some responses from the PCR1000 have an added character at the end
of the response string. It is usually a duplicate of the last
character of the string and can be discarded. (A bug, I suppose)
 
DTR and RTS set high by software.
 
When radio is first turned on, the software send the following commands:
initial boot up at 9600 Baud.
 
H101		Turn Radio ON
G105		Set Baud rate to 38400 Baud
G300		Set Autoupdate to OFF
H1?			Is radio still ON? Responds H100 for OFF, H101 for ON
H101		Radio ON command
G4?			Possible Inquire Firmware Revision? US and UK version
			returns G410
G301		Auto Update ON
GE?			Inquire Country/Region returns GE09 in FCC USA version 
			and GE02 in EUR/AUS version
GD?			Is DSP Installed?
			Returns GD00 if NO, GD01 if Yes

K00857937500050200	Set Frequency and mode and filter

J4100		Set Squelch
J5100		See Tone Squelch
J5000		Set VSC off
J4380		Set IF Shift to mid range
J4500		Set AGC OFF
J4600		Set Noise Blanker OFF
J4700		Set Attenuator OFF
J4A80		Not known
LD82000		Set Tracking Filter to Automatic

J8001J8101J8200J8301 DSP Packet. See DSP commands below

J4000	Set Volume

ME0000120050100012500	Set Bandscope ON to +- 200 Khz and 12.5 Khz step

' This is returned when Bandscope first turned ON
NE100000000000000000000000000000000000
NE110000000000000000000000000000000000
NE120000000000000000000000000000000000
NE130000000000000000000000000000000000
NE140000000000000000000000000000000000
NE150000000000000000000000000000000000
NE160000000000000000000000000000000000
NE170000000000000000000000000000000000
NE190000000000000000000000000000000000
NE1A0000000000000000000000000000000000
NE1B0000000000000000000000000000000000
NE1C0000000000000000000000000000000000
NE1D0000000000000000000000000000000000
NE1E0000000000000000000000000000000000
NE1F0000000000000000000000000000000000
___________________________________________________
 
Command Status:
G0xx
where xx = 00 when command is Good, 01 when command is Bad
 
Signal Update:
G3xx
00 = off (Program needs to Poll status) See I1? to I3? below
01 = on (Radio sends Status when a parameter changes) See Ix? commands
02 = binary mode (Update off)
03 = binary mode (Update on)
 
Inquire signal information. These commands can be Polled or are
returned as a packet if Autoupdate is ON (See G301) and one of the
values changes.
 
I0? Squelch Status returns 04 = Closed, 07 = Open
I1? Signal Strength returns 00 to FF
I2? Signal Centering returns 00 = Low, 80 = Centered, FF = High
I3? DTMF Tone
returns I300 if NO DTMF tone present
returns I31 + ASCII digit 0 to F (* = E, # = F)
 
Alive:
H1? Is radio alive? Radio responds H101 (on) or H100 (off)
 
Volume:
J40xx xx Range 00 to FF
 
Squelch:
J41xx xx Range 00 to FF
 
IF Shift:
J43xx xx Range 00 to FF
 
AGC:
J45xx xx = 01 for ON, 00 for OFF
 
NB:
J46xx xx = 01 for ON, 00 for OFF
 
Attenuator:
J47xx xx = 01 for ON, 00 for OFF
 
VSC:
J50xx xx = 01 for ON, 00 for OFF
 
T Squelch on = J51tt (tt=tone , 01=67Hz, 33=254.1Hz)
T Squelch off = J5100
 
Frequency: K0GMMMKKKHHHmmff00
where K0 G MMM KKK HHH mm ff 00
G=GHz
MMM=MHz
KKK=KHz
HHH=Hz
mm = mode
ff = Filter
00 = Padding Bytes (always there)
Mode Settings:
00 = LSB
01 = USB
02 = AM
03 = CW
04 = Not used or Unknown
05 = NFM
06 = WFM
 
Filter Settings:
00 = 3 Khz (actually 2.8 Khz) (CW USB LSB AM)
01 = 6 Khz (CW USB LSB AM NFM)
02 = 15 Khz (AM NFM)
03 = 50 Khz (AM NFM WFM)
04 = 230 Khz (WFM)
 
Radio Replies
I0xx Squelch Status xx=04 Closed, 07 Open
I1ss ss is Signal Strength 00 to FF
I200 Signal Frequency < Display Frequency
I280 Signal Frequency = Display Frequency
I2FF Signal Frequency > Display Frequency
I300 No DTMF Present
I31t t is DTMF tone (* = E, # = F)
 
Baud Rate
G1xx
where xx is:
00 = 300
01 = 1200
02 = 4800
03 = 9600
04 = 19200
05 = 38400
 
Signal Update
G3xx
00 = off (Program needs to inquire signal strength, DTMF tone, Center, etc)
01 = on (Radio sends signal strength, DTMF tone, etc as needed)
02 = binary mode (Update off)
03 = binary mode (Update on)
 
 
__________________________________________________________
Icom PCR1000 Band Scope commands.
 
The basic command to turn the bandscope function On is:
 
ME0000120050100012500 + CR + LF
 
The command breaks down this way:
 
ME00001 20050100012500
ME00001 is the preamble. It's always the same.
 
ME00001 20 050100012500
20 is the number of samples. It must be a 2 digit HEX number
represented in ASCII. Add leading 0 if necessary. Calculate
this number by dividing the Scope Bandwidth by the step size. For
example in the +- 200 Khz span the total bandwidth is 400 Khz. If the
step size is 12.5 khz then 400/12.5 is 32 or 20 Hex. If you get a non
integer answer for the initial division then increment the sample
number by 1 or 2 (sample should be an EVEN number). You can
arbitrarily set the sample higher(SLIGHTLY) to allow the display to
be moved inward from the edges of the scope display.
 
ME0000120 05 0100012500
05 is a sample rate value that determines how fast the
scope is swept and in the Icom software is either 05 or 28. This is
interpreted as a hex number and must be 2 digits. The practical values
for this runs from 01 (very fast and resource intensive) to about
70 (very slow and nearly useless). Putting 00 here locks the PCR1000
and software up. In the Icom software the number of samples
determine this value. Sample numbers above 10 hex use 05 and those
10 Hex or lower use 28 Hex.
 
ME000012005 01 00012500
01 This is the On/Off characters. If they are 00 then
the bandscope is OFF. If they are 01 the bandscope is ON
 
ME00001200501 00 012500
00 is a padding value and must be there.
 
ME0000120050100 012500
012500 is the step size expressed in HERTZ. It must
be 6 digits long, padded with LEADING ZEROS. Examples are 001000 for
1000 hertz (1 Khz), 030000 for 30 Khz and 100000 for 100 Khz. The
bandscope accepts values down to at least 10 hertz but the Icom
software displays a LIMIT warning at 1 Khz and at 100 Khz.
 
The Band Scope commands appear to be insensitive to mode and bandwidth
except for the fact that it doesn't work in USB, LSB or CW. It has
been hypothesized that the product detector circuitry is used by the
Band Scope.
 
Typical Band Scope Commands:
 
ME00001C8050100001000 +- 100 Khz @ 1 Khz
ME0000164050100001000 +- 50
ME0000132050100001000 +- 25
 
ME00001A0050100002500 +- 200 Khz @ 2.5 Khz
ME0000150050100002500 +- 100
ME0000128050100002500 +- 50
ME0000114050100002500 +- 25
 
ME0000150050100005000 +- 200 Khz @ 5.0 Khz
ME0000128050100005000 +- 100
ME0000114050100005000 +- 50
ME000010A280100005000 +- 25
 
ME0000140050100006250 +- 200 Khz @ 6.25 Khz
ME0000120050100006250 +- 100
ME0000110280100006250 +- 50
ME0000108280100006250 +- 25
 
ME000012E050100009000 +- 200 Khz @ 9.0 Khz
ME0000118050100009000 +- 100
ME000010C280100009000 +- 50
ME0000106280100009000 +- 25
 
ME0000128050100010000 +- 200 Khz @ 10.0 Khz
ME0000114050100010000 +- 100
ME000010A280100010000 +- 50
ME0000106280100010000 +- 25
 
ME0000120050100012500 +- 200 Khz @ 12.5 Khz
ME0000110280100012500 +- 100
ME0000110280100012500 +- 50
ME0000108280100012500 +- 25
 
ME0000114050100020000 +- 200 Khz @ 20.0 Khz
ME000010A280100020000 +- 100
ME0000106280100020000 +- 50
ME0000104280100020000 +- 25
 
ME0000110280100025000 +- 200 Khz @ 25.0 Khz
ME0000108280100025000 +- 100
ME0000104280100025000 +- 50
ME0000100280100025000 +- 25
 
ME000010E280100030000 +- 200 Khz @ 30.0 Khz
ME0000108280100030000 +- 100
ME0000104280100030000 +- 50
ME0000100280100030000 +- 25
 
ME0000108280100050000 +- 200 Khz @ 50.0 Khz
ME0000104280100050000 +- 100
ME0000100280100050000 +- 50 Note 00 sample size This is invalid!
 
ME0000104280100100000 +- 200 Khz @ 100.0 Khz
ME0000100280100100000 +- 100 Invalid
ME0000100280100100000 +- 50 Invalid
 
**********************************************************************
 
The data is returned in 37 byte packets. The packets begin with the
Letters NE1 followed by the 2 digit hex packet number. The Packet
numbers run from 00 to F0 (must be 2 digits). Typical packet numbers
would be NE100, NE170, NE180 and NE1F0.
These numbers are followed by 32 bytes that contain signal level
information for EACH sample (16 per packet).
 
Each byte is a hex number (in ascii) that can run from 00 to FF. The
bytes in packet NE180 represent the first 16 samples UP from the
displayed frequency in ascending order. The bytes in packet NE170
represent the 16 samples BELOW the displayed frequency in descending
order. For example the following:
 
NE18020202020202020202020202020202020
 
NE1 80 20202020202020202020202020202020
NE1 is the fixed preamble
 
NE1 80 20202020202020202020202020202020
80 is the packet number
 
NE180 20 202020202020202020202020202020
20 is the sample signal level (20 Hex) at the displayed frequency
 
NE18020 20 2020202020202020202020202020
This is the next sample level UP from the displayed freq
 
The next 14 values represent the next 14 sample levels. If less samples
are needed, the higher sample levels are set to 00. If more then 16
sanple levels are needed Up from the center freq, then the next packet
NE190 hold the values.
 
For the following:
NE17000000000000000001111111111111111
 
NE1 7000000000000000001111111111111111
NE1 is the fixed preamble
 
NE1 70 00000000000000001111111111111111
70 is the packet number. Packet 70 is the first packet BELOW the
center frequency.
 
NE170000000000000000011111111111111 11
11 This is the first sample level
BELOW the center frequency.
 
NE1700000000000000000111111111111 11 11
11 This is the next LOWER sample
level and so on. In this example, only 16 (10 Hex) samples were
specified. 8 samples are provided here below center freq and the
corresponding 8 above center freq would be in the NE180 packet.
 
If more then the 32 samples that can be displayed with NE170 and NE180
were specified then additional packets would be sent. For example if
48(decimal) samples were specified then the following packets would be
returned: NE160 would have 8 samples (in the UPPER 8)
NE170 would have 16 samples
NE180 would have 16 samples
NE190 would have 8 samples (in the LOWER 8)
 
Note that they are sent in ascending order from NE160 to NE190.
A rough indication of the number of packets needed for a given sample
size is (Number of samples)/16 plus 1. If the number is ODD then add
1 more packet.
 
Sample in order sent (This is a continous string):
NE1600000000000000000000030180FA61F14
NE1701F2B0C0F7E030C2B85088E080F2B4314
NE1801B8E181830085FEC6603083001143003
NE19001030101012701000000000000000000
 
When Band Scope is first turned ON or is turned OFF, ALL 16 packets
are returned with ALL samples set to 00.
 
 
___________________________________________________________
The DSP commands below have to be sent as a packet
followed by a Cr + Lf
 
sample packet
 
J8001J8101J820FJ8301
| | is always there and never changes
 
J8001J8101J820FJ8301
| | DSP ON J8100 is DSP Off
 
J8001J8101J820FJ8301
| | ANR on and set to max would be J8200 if off
 
J8001J8101J820FJ8301
| | Notch turned ON J8301 turns notch ON
 
 
With this in mind here is the DSP Command Set:
 
On startup the software sends GD?
Returns GD00 if NO DSP installed
Returns GD01 if DSP Installed
 
Autoupdate must be ON (send G301 + cr + lf):
 
J8001 Always the same
 
J81xx where xx is 00 if DSP is OFF and 01 if ON
 
J82xx This is the ANR function (Automatic Noise Reduction)
xx is 00 if ANR is OFF. If ON, xx varies from 01 to 0F
when you turn a knob on the new DSP Popup panel.
 
J83xx is the Automatic Notch filter. xx is 00 if notch
is OFF and 01 if ON.
 
The following data is written to the PCR1000.ini file.
 
DSPON with either 0 or 1 for Off/On
DSPANF with either 0 or 1 for Off/On
DSPNR with either 0 or 1 for Off/On
DSPNRLEVEL with value 0 to 15 for Noise Reduction level
 
----------------------------------------------------------
 
COUNTRY/REGION Table
GE? (Returns contents of Address 7E of the EEPROM)
 
JAPAN 00
USA 01
EUR/AUS 02
FRA 03
DEN 04
CAN 05
GENE1 06
GENE2 07
FCC JPN 08
FCC USA 09
FCC EUR/AUS 0A
FCC FRA 0B
FCC DEN 0C
FCC CAN 0D
FCC GENE1 0E
FCC GENE2 OF
 
TRACKING FILTER
LD820x xx=00 automatic tracking, Range 01 to FF manual setting of filter
 
EEPROM UNLOCKING
 
Don't play with these unless you have a verified backup of your own PCR-1000 EEPROM contents
 
You need to unlock the protection even to READ the EEPROM contents.
 
The Unlock Codes are:
GC01
GCF0
 
Note the response to both the above commands is G001
 
Read Command:
LD0xx? xx= eeprom address 00 to 7F
Replies LD0xxyy yy = data in location xx
 
Write Command:
LD0xxyy xx = address yy = data to write
Replies G000 if OK
 
 
To put protection back on again turn the PCR-1000 off and on again.
Use the software command H100 or the switch.
The baud rate will then return to 9600.
 
 
Average Values
Use these if you have a corrupted EEPROM and no backup
 
 
Crash Pattern
This is what the PCR-1000 produces if it crashes and corrupts the EEPROM
 
EEPROM Locations (DATA IN DECIMAL)
 
ADDRESS AVERAGE CRASHED COMMENTS
(HEX) (DEC) (DEC)
00 0 0 RESERVED
01 0 0 RESERVED
02 82 82 CHECK PATTERN
03 88 88 CHECK PATTERN
04 99 128 REFERENCE XTAL SHIFT (CENTRE =128)
05 0 0 RESERVED
06 0 0 RESERVED
07 0 0 RESERVED
08 63 0 FM LEVEL S0 REF-VOLT
09 79 48 FM LEVEL S3 REF-VOLT
0A 95 80 FM LEVEL S5 REF-VOLT
0B 117 112 FM LEVEL S7 REF-VOLT
0C 140 144 FM LEVEL S9 REF-VOLT
0D 166 176 FM LEVEL S9+20 REF-VOLT
0E 188 208 FM LEVEL S9+40 REF-VOLT
0F 210 240 FM LEVEL S9+60 REF-VOLT
10 54 0 WFM LEVEL S0 REF-VOLT
11 67 48 WFM LEVEL S3 REF-VOLT
12 80 80 WFM LEVEL S5 REF-VOLT
13 103 112 WFM LEVEL S7 REF-VOLT
14 126 144 WFM LEVEL S9 REF-VOLT
15 152 176 WFM LEVEL S9+20 REF-VOLT
16 177 208 WFM LEVEL S9+40 REF-VOLT
17 199 240 WFM LEVEL S9+60 REF-VOLT
18 21 0 SCOPE LEVEL S0 REF-VOLT
19 39 48 SCOPE LEVEL S3 REF-VOLT
1A 45 80 SCOPE LEVEL S5 REF-VOLT
1B 55 112 SCOPE LEVEL S7 REF-VOLT
1C 57 144 SCOPE LEVEL S9 REF-VOLT
1D 61 176 SCOPE LEVEL S9+20 REF-VOLT
1E 64 208 SCOPE LEVEL S9+40 REF-VOLT
1F 67 240 SCOPE LEVEL S9+60 REF-VOLT
20 21 115 FM CENTERMETER LOW
21 57 46 FM CENTERMETER HIGH
22 115 115 RESERVED
23 146 146 RESERVED
24 11 166 FM NOISESQL THRESHOLD LEVEL
25 11 36 FM NOISESQL TIGHT LEVEL
26 2 0 FM NOISESQL SETTING T2
27 4 0 FM NOISESQL SETTING T3
28 0 0 RESERVED
29 0 0 RESERVED
2A 96 96 CTCSS-DET JUDGEMENT LEVEL (CLOSE)
2B 160 160 CTCSS-DET JUDGEMENT LEVEL (OPEN)
2C 98 144 BPF0 LEVEL S9 REFERENCE
2D 137 144 BPF1 LEVEL S9 REFERENCE
2E 111 144 BPF2 LEVEL S9 REFERENCE
2F 106 144 BPF3 LEVEL S9 REFERENCE
30 125 144 BPF4 LEVEL S9 REFERENCE 50.02MHZ
31 130 144 BPF4 LEVEL S9 REFERENCE 58.28MHZ
32 129 144 BPF4 LEVEL S9 REFERENCE 58.32MHZ
33 135 144 BPF4 LEVEL S9 REFERENCE 88.02MHZ
34 138 144 BPF4 LEVEL S9 REFERENCE 108.28MHZ
35 138 144 BPF4 LEVEL S9 REFERENCE 108.32MHZ
36 141 144 BPF4 LEVEL S9 REFERENCE 130.02MHZ
37 140 144 BPF4 LEVEL S9 REFERENCE 149.98MHZ
38 118 144 BPF5 LEVEL S9 REFERENCE 150.02MHZ
39 122 144 BPF5 LEVEL S9 REFERENCE 183.28MHZ
3A 121 144 BPF5 LEVEL S9 REFERENCE 183.32MHZ
3B 122 144 BPF5 LEVEL S9 REFERENCE 216.02MHZ
3C 119 144 BPF5 LEVEL S9 REFERENCE 265.68MHZ
3D 117 144 BPF5 LEVEL S9 REFERENCE 265.72MHZ
3E 117 144 BPF5 LEVEL S9 REFERENCE 300.02MHZ
3F 110 144 BPF5 LEVEL S9 REFERENCE 349.98HZ
40 124 144 BPF6 LEVEL S9 REFERENCE 350.02HZ
41 123 144 BPF6 LEVEL S9 REFERENCE 383.28MHZ
42 123 144 BPF6 LEVEL S9 REFERENCE 383.32MHZ
43 125 144 BPF6 LEVEL S9 REFERENCE 433.32MHZ
44 123 144 BPF6 LEVEL S9 REFERENCE 483.28MHZ
45 123 144 BPF6 LEVEL S9 REFERENCE 483.32MHZ
46 121 144 BPF6 LEVEL S9 REFERENCE 558.32MHZ
47 119 144 BPF6 LEVEL S9 REFERENCE 633.28MHZ
48 119 144 BPF6 LEVEL S9 REFERENCE 633.32MHZ
49 116 144 BPF6 LEVEL S9 REFERENCE 699.98MHZ
4A 103 144 BPF7 LEVEL S9 REFERENCE 700.02MHZ
4B 107 144 BPF7 LEVEL S9 REFERENCE 750.02MHZ
4C 110 144 BPF7 LEVEL S9 REFERENCE 799.98MHZ
4D 110 144 BPF7 LEVEL S9 REFERENCE 800.02MHZ
4E 120 144 BPF7 LEVEL S9 REFERENCE 916.68MHZ
4F 119 144 BPF7 LEVEL S9 REFERENCE 916.72MHZ
50 123 144 BPF7 LEVEL S9 REFERENCE 1016.68MHZ
51 122 144 BPF7 LEVEL S9 REFERENCE 1016.72MHZ
52 112 144 BPF7 LEVEL S9 REFERENCE 1166.68MHZ
53 111 144 BPF7 LEVEL S9 REFERENCE 1166.72MHZ
54 110 144 BPF7 LEVEL S9 REFERENCE 1299.98MHZ
55 0 0 RESERVED
56 0 0 RESERVED
57 0 0 RESERVED
58 35 128 BPF4 TUNING PEAK-POINT 50.02MHZ
59 35 128 BPF4 TUNING PEAK-POINT 58.28MHZ
5A 117 128 BPF4 TUNING PEAK-POINT 58.32MHZ
5B 110 128 BPF4 TUNING PEAK-POINT 88.02MHZ
5C 114 128 BPF4 TUNING PEAK-POINT 108.28MHZ
5D 188 128 BPF4 TUNING PEAK-POINT 108.32MHZ
5E 191 128 BPF4 TUNING PEAK-POINT 130.02MHZ
5F 204 128 BPF4 TUNING PEAK-POINT 149.98MHZ
60 70 128 BPF5 TUNING PEAK-POINT 150.02MHZ
61 43 128 BPF5 TUNING PEAK-POINT 183.28MHZ
62 118 128 BPF5 TUNING PEAK-POINT 183.32MHZ
63 101 28 BPF5 TUNING PEAK-POINT 216.02MHZ
64 92 128 BPF5 TUNING PEAK-POINT 265.68MHZ
65 180 128 BPF5 TUNING PEAK-POINT 265.72MHZ
66 173 128 BPF5 TUNING PEAK-POINT 300.02MHZ
67 177 128 BPF5 TUNING PEAK-POINT 349.98MHZ
68 46 128 BPF6 TUNING PEAK-POINT 350.02MHZ
69 32 128 BPF6 TUNING PEAK-POINT 383.28MHZ
6A 113 128 BPF6 TUNING PEAK-POINT 383.32MHZ
6B 95 128 BPF6 TUNING PEAK-POINT 433.32MHZ
6C 83 128 BPF6 TUNING PEAK-POINT 483.28MHZ
6D 156 128 BPF6 TUNING PEAK-POINT 483.32MHZ
6E 132 128 BPF6 TUNING PEAK-POINT 558.32MHZ
6F 112 128 BPF6 TUNING PEAK-POINT 633.28MHZ
70 187 128 BPF6 TUNING PEAK-POINT 633.32MHZ
71 185 128 BPF6 TUNING PEAK-POINT 699.98MHZ
72 75 128 BPF7 TUNING PEAK-POINT 700.02MHZ
73 66 128 BPF7 TUNING PEAK-POINT 750.02MHZ
74 57 128 BPF7 TUNING PEAK-POINT 799.98MHZ
75 146 128 BPF7 TUNING PEAK-POINT 800.02MHZ
76 79 128 BPF7 TUNING PEAK-POINT 916.68MHZ
77 160 128 BPF7 TUNING PEAK-POINT 916.72MHZ
78 122 128 BPF7 TUNING PEAK-POINT 1016.68MHZ
79 194 128 BPF7 TUNING PEAK-POINT 1016.72MHZ
7A 127 128 BPF7 TUNING PEAK-POINT 1166.68MHZ
7B 202 128 BPF7 TUNING PEAK-POINT 1166.72MHZ
7C 170 128 BPF7 TUNING PEAK-POINT 1299.98MHZ
7D 0 0 RESERVED
7E * 7 COUNTRY/REGION (*See Table of values)
7F 2 7 RESERVED
 
 
 
Unknowns
---------
Still looking for the purpose of the following PCR commands:-
 
LE20050
LE20040
 
Above used by icom in their EEPROM routines in their software for setting up
the radio and also some third party software I have seen. Is this understood or
is it just being copied?
 
 
GCD0
GE07
 
H800 * see below
 
LD840?
LD846?
LD84A? * see below
 
 
LD842 * see below
LD844 * see below
LD848
LD860
LD862
 
 
G4? returns G410
 
J4A80
 
If anyone fill in the gaps that would be great!
Update:- Some more info has come in! - Thanks to the guys on the PCR-1000 list.
There are some questions about the following commands:

 

LD840? (always 0)

LD842? (current signal strength)

LD844? (centering info?)

LD846? (usually 0x60)

LD848? (always 0)

LD84A? (old LD842?)

LD84C? (always 0)

LD84E? (always 0)

LD860? (always 0)

LD862? (always 0)

LD864? (always 0)

LD842? returns the current signal strength level, same as for I1?

LD84A? returns the previous signal strength. I believe this is what the radio uses to determine if the signal strength level has changed (as you probably noticed, it only kicks out a new I1xx message in G301 mode when the signal strength changes).

LD844? returns something relevant to frequency. If I set the PCR-1000 to 144.35, NBFM, 6Khz filter, and set my HT for 144.35 Mhz, when I transmit on the HT, this value goes to 0x25. Moving the HT to 144.355 causes the value to increase to a nominal 0x37, and moving the HT to 144.345 causes the value to decrease to a nominal 0x15. Interesting relationship between these values: 0x37 - 0x25 = 0x12, and 0x25 - 0x15 = 0x10. This is 18 decimal and 16 decimal respectively. Two pretty close numbers for shifting +/- 5Khz. I expect this register has something to do with the centering information.

I'm not completely sure about this, but every document except 1 has the auto tracking register mis-coded. It's generally listed as LD8200 to turn off, and LD8201 to set to manual. It's actually LD82000 (note three 0's) to turn off, and can be varied from LD82001 to LD820FF. I don't know exactly what the tracking thingie does, but it has a pronounced affect turning it off or on. - Editor's note this has now been corrected in this document, it is the tracking of the RF stage with frequency I believe.

There's also a mention of H800 in the documents. H8 will accept values from 00 to FF, and has an odd effect on the audio. Values greater than or equal to 0x81 cause a very brief (100ms or so) drop out on the audio. Values 0x80 and below don't do this. Interestingly enough, when G301 is in effect, changing H8 causes an H9 message to be output. H9 can be read with the H9? command. I've only seen 0x00, 0x01, and 0x10 be reported back.

The following command sequence seems to have something to do with squelch and signal strength. H800; J4180; H881; H9? (reports H901), J4100. Now the squelch should be open, but isn't. Setting H800 will open the squelch again. Now do H8FF; H9? Audio opens, and H9? reports H910. Curious. All my experimenting is in NBFM mode, so perhaps some of these other registers come into play in other modes.

Something else that's not mentioned in any of the documentation is that when G301 mode is on, commands don't generate a response unless they're interrogative (i.e. GD? or H1?).
*/
