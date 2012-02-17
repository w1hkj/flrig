#include "rigbase.h"
#include "util.h"
#include "debug.h"
#include "rig_io.h"
#include "support.h"


const char *szNORIG = "NONE";
const char *szNOMODES[] = {"LSB", "USB", NULL};
const char *szNOBWS[] = {"NONE", NULL};
const char *szDSPLO[] = {"NONE", NULL};
const char *szDSPHI[] = {"NONE", NULL};
const char *szdsptooltip = "dsp tooltip";
const char *szbtnlabel = " ";
const int  ibw_val = -1;

static GUI basewidgets[] = { {NULL, 0, 0} };

rigbase::rigbase()
{
	name_ = szNORIG;
	modes_ = szNOMODES;
	bandwidths_ = szNOBWS;
	dsp_lo = szDSPLO;
	lo_tooltip = szdsptooltip;
	lo_label = szbtnlabel;
	dsp_hi = szDSPHI;
	hi_tooltip = szdsptooltip;
	hi_label = szbtnlabel;
	bw_vals_ = &ibw_val;

	widgets = basewidgets;

	serloop_timing = 200; // msec, 5x / second

	stopbits = 2;

	CIV = 0;
	defaultCIV = 0;
	USBaudio = false;

	has_xcvr_auto_on_off =
	comm_echo =
	has_vfo_adj =
	has_rit =
	has_xit =
	has_bfo =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_bandwidth_control =
	has_dsp_controls =
	has_micgain_control =
	has_mic_line_control =
	has_notch_control =
	allow_notch_changes =
	has_noise_control =
	has_noise_reduction_control =
	has_noise_reduction =
	has_attenuator_control =
	has_preamp_control =
	has_ifshift_control =
	has_ptt_control =
	has_tune_control =
	has_swr_control =
	has_alc_control =
	has_rf_control =
	has_sql_control = 
	has_data_port =
	restore_mbw =

	has_extras = 
	has_nb_level =
	has_agc_level =
	has_cw_wpm =
	has_cw_vol =
	has_cw_spot =
	has_cw_spot_tone =
	has_cw_qsk =
	has_cw_weight =
	has_cw_keyer =
	has_vox_onoff =
	has_vox_gain =
	has_vox_anti =
	has_vox_hang =
	has_vox_on_dataport =
	has_auto_notch = 
	has_compression =
	has_compON =
	use_line_in =
	has_bpf_center =
	has_special =  
	has_smeter =
	has_power_out =
	has_split =
	has_a2b =
	has_band_selection =
	has_get_info = false;

	A.freq = 14070000L;
	A.imode = 1;
	A.iBW = 0;
	B.freq = 14070000L;
	B.imode = 1;
	B.iBW = 0;
	inuse = onA;
	precision = 1;
	can_change_alt_vfo = false;

	freqA = 14070000L;
	modeA = 1;
	bwA = 0;
	freqB = 14070000L;
	modeB = 1;
	bwB = 0;

	def_freq = 14070000L;
	def_mode = 1;
	def_bw = 0;
	bpf_center = 0;
	pbt = 0;

	max_power = 100;

	if_shift_min = -1500;
	if_shift_max = 1500;
	if_shift_step = 10;
	if_shift_mid = 0;
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
	static string sdec_be;
	sdec_be.clear();
	for (int i = 0; i < len; i++) {
		sdec_be += (char)((d % 10) + '0');
		d /= 10;
	}
	return sdec_be;
}

string rigbase::to_decimal(long d, int len)
{
	static string sdec;
	sdec.clear();
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

void rigbase::showresp(int level, int how, string s, string tx, string rx) 
{
	time_t now;
	time(&now);
	struct tm *local = localtime(&now);
	char sztm[20];
	strftime(sztm, sizeof(sztm), "%H:%M:%S", local);

	string s1 = how == HEX ? str2hex(tx.c_str(), tx.length()) : tx;
	string s2 = how == HEX ? str2hex(rx.c_str(), rx.length()) : rx;
	if (how == ASC) {
		size_t p;
		while((p = s1.find('\r')) != string::npos)
			s1.replace(p, 1, "<cr>");
		while((p = s1.find('\n')) != string::npos)
			s1.replace(p, 1, "<lf>");
		while((p = s2.find('\r')) != string::npos)
			s2.replace(p, 1, "<cr>");
		while((p = s2.find('\n')) != string::npos)
			s2.replace(p, 1, "<lf>");
	}

	switch (level) {
	case ERR:
		SLOG_ERROR("%s: %10s, cmd %s, ans %s", sztm, s.c_str(), s1.c_str(), s2.c_str());
		break;
	case WARN:
		SLOG_WARN("%s: %10s, cmd %s, ans %s", sztm, s.c_str(), s1.c_str(), s2.c_str());
		break;
	case INFO:
	default:
		SLOG_INFO("%s: %10s, cmd %s, ans %s", sztm, s.c_str(), s1.c_str(), s2.c_str());
	}
}

int rigbase::waitN(size_t n, int timeout, const char *sz, int pr)
{
	char sztemp[50];
	string returned = "";
	string tosend = cmd;
	int cnt = 0, repeat = 0;
	int waited = 0;
	size_t num = n + cmd.length();
	int delay =  num * 11000.0 / RigSerial.Baud();

	replystr.clear();

	if (!RigSerial.IsOpen()) {
		snprintf(sztemp, sizeof(sztemp), "TEST %s", sz);
		showresp(WARN, pr, sztemp, tosend, replystr);
		return 0;
	}

	sendCommand(tosend, 0);
	MilliSleep(delay);
	returned = "";
	for ( cnt = 0; cnt < timeout / 10; cnt++) {
		readResponse();
		returned.append(replystr);
		if (returned.length() >= n) {
			replystr = returned;
			waited = cnt * 10 * repeat + delay;
			snprintf(sztemp, sizeof(sztemp), "%s OK %d ms", sz, waited);
			showresp(WARN, pr, sztemp, cmd, returned);
			return replystr.length();
		}
		MilliSleep(10);
		Fl::awake();
	}

	replystr = returned;
	waited = cnt * 10 * repeat + delay;
	snprintf(sztemp, sizeof(sztemp), "%s failed %d ms", sz, waited);
	showresp(WARN, pr, sztemp, cmd, returned);
	return 0;
}


