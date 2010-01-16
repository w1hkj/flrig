#include "rigbase.h"
#include "util.h"
#include "debug.h"
#include "rig_io.h"

const char *szNORIG = "NONE";
const char *szNOMODES[] = {"LSB", "USB", NULL};
const char *szNOBWS[] = {"NONE", NULL};

rigbase::rigbase()
{
	name_ = szNORIG;
	modes_ = szNOMODES;
	bandwidths_ = szNOBWS;

	serloop_timing = 200; // msec, 5x / second

	stopbits = 2;

	comm_echo =
	has_rit =
	has_xit =
	has_bfo =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_bandwidth_control =
	has_micgain_control =
	has_mic_line_control =
	has_notch_control =
	has_noise_control =
	has_attenuator_control =
	has_preamp_control =
	has_ifshift_control =
	has_ptt_control =
	has_tune_control =
	has_swr_control =
	has_alc_control =
	has_rf_control =
	has_sql_control = 
	restore_mbw =
	has_agc_level =
	has_cw_wpm =
	has_cw_vol =
	has_cw_spot =
	has_vox_onoff =
	has_vox_gain =
	has_vox_anti =
	has_vox_hang =
	has_auto_notch = 
	has_compression = false;

	freq_ = 14070000L;
	mode_ = 1;
	bw_ = 0;

	deffreq_ = 14070000L;
	def_mode = 1;
	defbw_ = 0;

	max_power = 100;

}

string rigbase::to_bcd_be(long freq, int len)
{
	unsigned char a;
	int numchars = len / 2;
	string bcd = "";
	if (len & 1) numchars ++;
	for (int i = 0; i < numchars; i++) {
		a = 0;
		a |= freq % 10;
		freq /= 10;
		a |= (freq % 10)<<4;
		freq /= 10;
		bcd += a;
	}
	return bcd;
}

string rigbase::to_bcd(long freq, int len)
{
	string bcd_be = to_bcd_be(freq, len);
	string bcd = "";
	int bcdlen = bcd_be.size();
	for (int i = bcdlen - 1; i >= 0; i--)
		bcd += bcd_be[i];
	return bcd;
}

long rigbase::fm_bcd (char *bcd, int len)
{
	int i;
	long f = 0;
	int numchars = len/2;
	if (len & 1) numchars ++;
	for (i = 0; i < numchars; i++) {
		f *=10;
		f += (bcd[i] >> 4) & 0x0F;
		f *= 10;
		f += bcd[i] & 0x0F;
	}
	return f;
}


long rigbase::fm_bcd_be(char *bcd, int len)
{
	char temp;
	int numchars = len/2;
	if (len & 1) numchars++;
	for (int i = 0; i < numchars / 2; i++) {
		temp = bcd[i];
		bcd[i] = bcd[numchars -1 - i];
		bcd[numchars -1 - i] = temp;
	}
	return fm_bcd(bcd, len);
}

string rigbase::to_binary_be(long freq, int len)
{
	static string bin = "";
	for (int i = 0; i < len; i++) {
		bin += freq & 0xFF;
		freq >>= 8;
	}
	return bin;
}

string rigbase::to_binary(long freq, int len)
{
	static string bin = "";
	string bin_be = to_binary_be(freq, len);
	int binlen = bin_be.size();
	for (int i = binlen - 1; i >= 0; i--)
		bin += bin_be[i];
	return bin;
}

long rigbase::fm_binary(char *binary, int len)
{
	int i;
	long f = 0;
	for (i = 0; i < len; i++) {
		f *= 256;
		f += (unsigned char)binary[i];
	}
	return f;
}

long rigbase::fm_binary_be(char *binary_be, int len)
{
	unsigned char temp;
	int numchars = len/2;
	if (len & 1) numchars++;
	for (int i = 0; i < numchars / 2; i++) {
		temp = binary_be[i];
		binary_be[i] = binary_be[numchars -1 - i];
		binary_be[numchars -1 - i] = temp;
	}
	return fm_binary(binary_be, len);
}

string rigbase::to_decimal_be(long d, int len)
{
	static string sdec_be = "";
	for (int i = 0; i < len; i++) {
		sdec_be += (char)((d % 10) + '0');
		d /= 10;
	}
	return sdec_be;
}

string rigbase::to_decimal(long d, int len)
{
	static string sdec = "";
	string sdec_be = to_decimal_be(d, len);
	int bcdlen = sdec_be.size();
	for (int i = bcdlen - 1; i >= 0; i--)
		sdec += sdec_be[i];
	return sdec;
}

long rigbase::fm_decimal(char *decimal, int len)
{
	long d = 0;
	for (int i = 0; i < len; i++) {
		d *= 10;
		d += decimal[i] - '0';
	}
	return d;
}

long rigbase::fm_decimal_be(char *decimal_be, int len)
{
	unsigned char temp;
	int numchars = len/2;
	if (len & 1) numchars++;
	for (int i = 0; i < numchars / 2; i++) {
		temp = decimal_be[i];
		decimal_be[i] = decimal_be[numchars -1 - i];
		decimal_be[numchars -1 - i] = temp;
	}
	return fm_decimal(decimal_be, len);
}
