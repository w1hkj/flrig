// Kennwood TM-D710 and similar


#include "other/TMD710.h"

#define AOR5K_WAIT_TIME 200

const char TMD710name_[] = "TMD710";

const char *TMD710modes_[] = {
		"FM", "NFM", "AM", NULL};

//gibts nicht
static const char TMD710_mode_type[] =
	{'U', 'U', 'U'};

RIG_TMD710::RIG_TMD710() {
// base class values	
	name_ = TMD710name_;
	modes_ = TMD710modes_;
	bandwidths_ = NULL;
	comm_baudrate = BR38400;
	stopbits = 1;
	comm_retries = 3;
	comm_wait = 5;
	comm_timeout = 1000;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	modeA = 1;
//	bwA = 2;

	has_mode_control =
	has_ptt_control = true;

//	has_attenuator_control =
//	has_preamp_control =
//	has_power_out =
	has_power_control =
	has_sql_control =
/*	has_volume_control =
	has_bandwidth_control =
	has_micgain_control =
	has_notch_control =
	has_ifshift_control =
	has_tune_control =*/
//	has_swr_control = false;
	has_vfoAB = true;
//	has_split =
//	has_split_AB =
	precision = 10;
	ndigits = 7;

}

bool RIG_TMD710::check ()
{
	cmd = "DL 0\r"; // Enable dual mode
	int ret = wait_char('\r', 5, 100, "check and set Dual Band Mode", ASC);
	if (ret < 1) return false;
	return true;
}

// Works for TM-D710E
unsigned long int RIG_TMD710::get_vfoA ()
{
	cmd = "FO 0\r";
	int ret = wait_char('\r', 49, 100, "get VFO", ASC);
	if (ret < 49) return freqA;

	gett("get_vfoA");
	char frequency[11];
	long int f = 0;
	int p = 5;
	int n = 0;
	for (n = 0; n < 10; n++) frequency[n] = (replystr[p + n]);
	frequency[10] = '\0';
	f = atoi(frequency);
	freqA = f;

	char mode[2];
	mode[0] = (replystr[47]);
	mode[1] = '\0';
 	modeA = atoi(mode);


	return freqA;

	/*
1 	Band
					0 	Band A
					1 	Band B
					2 	TX A - RX B
					3 	TX B - RX A
2 	Frequency in Hz 10 digit. must be within selected band -> ok
3 	Step size 		0 	5 Khz
					1 	6.25 Khz
					2 	28,33 KHz
					3 	10 Khz
					4 	12.5 Khz
					5 	15 Khz
					6 	20 Khz
					7 	25 KHz
					8 	30 KHz
					9 	50 Khz
					A 	100 Khz
4 	Shift direction
					0 	simplex or split
					1 	up
					2 	down
5 	Reverse
					0 	off
					1 	on
6 	Tone status
					0 	off
					1 	on
7 	CTCSS status
					0 	off
					1 	on
8 	DCS status
					0 	off
					1 	on
9 	Tone frequency
					01 	67
					02 	69,3
					03 	71,9
					04 	74,4
					05 	77
					06 	79,7
					07 	82,5
					08 	85,4
					09 	88,5
					10 	91,5
					11 	94,8
					12 	97,4
					13 	100
					14 	103,5
					15 	107,2
					16 	110,9
					17 	114,8
					18 	118,8
					19 	123
					20 	127,3
					21 	131,8
					22 	136,5
					23 	141,3
					24 	146,2
					25 	151,4
					26 	156,7
					27 	162,2
					28 	167,9
					29 	173,8
					30 	179,9
					31 	186,2
					32 	192,8
					33 	203,5
					34 	240,7
					35 	210,7
					36 	218,1
					37 	225,7
					38 	229,1
					39 	233,6
					40 	241,8
					41 	250,3
					42 	254,1
10 	CTCSS frequency
					01 	67
					02 	69,3
					03 	71,9
					04 	74,4
					05 	77
					06 	79,7
					07 	82,5
					08 	85,4
					09 	88,5
					10 	91,5
					11 	94,8
					12 	97,4
					13 	100
					14 	103,5
					15 	107,2
					16 	110,9
					17 	114,8
					18 	118,8
					19 	123
					20 	127,3
					21 	131,8
					22 	136,5
					23 	141,3
					24 	146,2
					25 	151,4
					26 	156,7
					27 	162,2
					28 	167,9
					29 	173,8
					30 	179,9
					31 	186,2
					32 	192,8
					33 	203,5
					34 	240,7
					35 	210,7
					36 	218,1
					37 	225,7
					38 	229,1
					39 	233,6
					40 	241,8
					41 	250,3
					42 	254,1
11 	DCS frequency  -> seite down
12 	Offset frequency in Hz 8 digit
13 	Mode
					0 	FM
					1 	NFM
					2 	AM

	*/

}

unsigned long int RIG_TMD710::get_vfoB ()
{
	cmd = "FO 1\r";
	int ret = wait_char('\r', 49, 100, "get VFO", ASC);
	if (ret < 49) return freqB;

	gett("get_vfoB");
	char frequency[11];
	long int f = 0;
	int p = 5;
	int n = 0;
	for (n = 0; n < 10; n++) frequency[n] = (replystr[p + n]);
	frequency[10] = '\0';
	f = atoi(frequency);
	freqB = f;

	char mode[2];
	mode[0] = (replystr[47]);
	mode[1] = '\0';
 	modeB = atoi(mode);
	return freqB;
}

void RIG_TMD710::set_vfoA (unsigned long int freq)
{
	cmd = "FO 0\r";
	int ret = wait_char('\r', 49, 200, "get VFO A", ASC);
	if (ret < 49) return;
	cmd = replystr;
	char frequency[11];
	snprintf(frequency,11, "%010ld", freq);
	int n = 0;
	int p = 5;
	for (n = 0; n < 10; n++) (cmd[p + n])= frequency[n];
	wait_char('\r', 49, 200, "set frequency A", ASC);
	return;
}

void RIG_TMD710::set_vfoB (unsigned long int freq)
{
	cmd = "FO 1\r";
	int ret = wait_char('\r', 49, 200, "get VFO B", ASC);
	if (ret < 49) return;
	cmd = replystr;
	char frequency[11];
	snprintf(frequency,11, "%010ld", freq);
	int n = 0;
	int p = 5;
	for (n = 0; n < 10; n++) (cmd[p + n])= frequency[n];
	wait_char('\r', 49, 200, "set frequency B", ASC);
	return;
}

int RIG_TMD710::get_modetype(int n)
{
	return TMD710_mode_type[n];
}

void RIG_TMD710::set_modeA(int val)
{
	// AM is not available at 430 MHz
	cmd = "FO 0\r";
	int ret = wait_char('\r', 49, 200, "get VFO A", ASC);
	if (ret < 49) return;

	cmd = replystr;
	cmd[47] = val +'0';
	wait_char('\r', 49, 200, "set mode A", ASC);
	return;
}

void RIG_TMD710::set_modeB(int val)
{
	cmd = "FO 1\r";
	int ret = wait_char('\r', 49, 200, "get VFO B", ASC);
	if (ret < 49) return;

	cmd = replystr;
	cmd[47] = val +'0';
	wait_char('\r', 49, 200, "set mode B", ASC);
	return;
}

int RIG_TMD710::get_modeA()
{
	//information is available with cyclic VFO readings
	return modeA;
}

int RIG_TMD710::get_modeB()
{
	//information is available with cyclic VFO readings
	return modeB;
}

void RIG_TMD710::selectA()
{
	cmd = "BC 0,0\r";
	wait_char('\r', 3, 100, "set select A PTT/CTRL band", ASC);
	sett("selectA");
	inuse = onA;
}

void RIG_TMD710::selectB()
{
	cmd = "BC 1,1\r";
	wait_char('\r', 3, 100, "set select B PTT/CTRL band", ASC);
	sett("selectB");
	inuse = onB;
}

// Tranceiver PTT on/off
void RIG_TMD710::set_PTT_control(int val)
{
	if (val) sendCommand("TX\r");
	else	 sendCommand("RX\r");
	ptt_ = val;
}

int RIG_TMD710::get_PTT()
{
	return ptt_;
}

// Transceiver power level
double RIG_TMD710::get_power_control()
{
	char seite[2];
	cmd = "PC ";
	snprintf(seite,2, "%d", inuse);
	cmd += seite;
	cmd += '\r';
	int ret = wait_char('\r', 8, 100, "get power", ASC);
	if (ret < 1) return false;

	gett("get_power_control");
	int wert;
	int mtr = 30;
	wert = replystr[5] - '0';
	switch (wert) {
			case 0: mtr = 50; break;
			case 1: mtr = 10; break;
			case 2: mtr = 5; break;
		}
	return mtr;
}

void RIG_TMD710::set_power_control(double val)
{
	char mtrs = '2';
	if (val >= 35) mtrs = '0';
	if (val < 35) mtrs = '1';
	if (val < 10) mtrs = '2';
	char seite[2];
	cmd = "PC ";
	snprintf(seite,2, "%d", inuse);
	cmd += seite;
	cmd += ',';
	cmd += mtrs;
	cmd += '\r';
	wait_char('\r', 8, 100, "set power", ASC);
}

void RIG_TMD710::set_squelch(int val)
{
	char hexstr[3] = "00";
	char seite[2];
	cmd = "SQ ";
	snprintf(seite,2, "%d", inuse);
	cmd += seite;
	cmd += ',';
	snprintf(hexstr,3,"%02X", val);
	cmd += hexstr;
	cmd += '\r';
	wait_char('\r', 9, 100, "set power", ASC);
}

// needed for mingw compile which does not support the C++11 stoi template
namespace stoi_fix
{
	int stoi( const std::string& str, std::size_t* pos = 0, int base = 10 )
	{
		const char* begin = str.c_str() ;
		char* end = 0 ;
		long value = std::strtol( begin, &end, base ) ;
		if (pos) *pos = end - begin ;
		return value ;
	}
}

int  RIG_TMD710::get_squelch()
{
	char seite[2];
		cmd = "SQ ";
		snprintf(seite,2, "%d", inuse);
		cmd += seite;
		cmd += '\r';
		int ret = wait_char('\r', 7, 100, "get squelch", ASC);
		if (ret < 1) return false;
		gett("get_squelch_control");

		char hexstr[3];
		hexstr[0]= replystr[3];
		hexstr[1]= replystr[4];
		hexstr[2]= '\0';

		return stoi_fix::stoi (hexstr, 0, 16);
}
