#include "dialogs.h"
#include "util.h"
#include "debug.h"
#include "serial.h"
#include "support.h"
#include "rigpanel.h"
#include "xml_io.h"
#include "rigbase.h"
#include "font_browser.h"

#include <string>

#include <iostream>

using namespace std;

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef __WIN32__
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>
#endif

#ifdef __APPLE__
#  include <glob.h>
#endif

Fl_Double_Window *dlgDisplayConfig = NULL;
Fl_Double_Window *dlgXcvrConfig = NULL;
Fl_Double_Window *dlgMemoryDialog = NULL;
Fl_Double_Window *dlgControls = NULL;
Fl_Double_Window *tt550_controls = NULL;
Font_Browser     *fntbrowser = NULL;


//======================================================================
// test comm ports
//======================================================================

void clear_combos()
{
	selectCommPort->clear();
	selectAuxPort->clear();
	selectSepPTTPort->clear();
	selectCommPort->add("NONE");
	selectAuxPort->add("NONE");
	selectSepPTTPort->add("NONE");
}

void add_combos(char *port)
{
	selectCommPort->add(port);
	selectAuxPort->add(port);
	selectSepPTTPort->add(port);
}

//======================================================================
// WIN32 init_port_combos
//======================================================================

#ifdef __WIN32__
static bool open_serial(const char* dev)
{
	bool ret = false;
	HANDLE fd = CreateFile(dev, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if (fd != INVALID_HANDLE_VALUE) {
		CloseHandle(fd);
		ret = true;
	}
	return ret;
}

#  define TTY_MAX 255
void init_port_combos()
{
	int retval;
	clear_combos();

	char ttyname[21];
	const char tty_fmt[] = "//./COM%u";

	for (unsigned j = 0; j < TTY_MAX; j++) {
		snprintf(ttyname, sizeof(ttyname), tty_fmt, j);
		if (!open_serial(ttyname))
			continue;
		snprintf(ttyname, sizeof(ttyname), "COM%u", j);
		LOG_INFO("Found serial port %s", ttyname);
		add_combos(ttyname);
	}
}
#endif //__WIN32__

//======================================================================
// Linux init_port_combos
//======================================================================

#ifdef __linux__
#ifndef PATH_MAX
#  define PATH_MAX 1024
#endif
#  define TTY_MAX 8

void init_port_combos()
{
	int retval;


	struct stat st;
	char ttyname[PATH_MAX + 1];
	bool ret = false;

	DIR* sys = NULL;
	char cwd[PATH_MAX] = { '.', '\0' };

	clear_combos();

	if (getcwd(cwd, sizeof(cwd)) == NULL || chdir("/sys/class/tty") == -1 ||
	    (sys = opendir(".")) == NULL)
		goto out;

	ssize_t len;
	struct dirent* dp;
	while ((dp = readdir(sys))) {
#  ifdef _DIRENT_HAVE_D_TYPE
		if (dp->d_type != DT_LNK)
			continue;
#  endif
		if ((len = readlink(dp->d_name, ttyname, sizeof(ttyname)-1)) == -1)
			continue;
		ttyname[len] = '\0';
		if (!strstr(ttyname, "/devices/virtual/")) {
			snprintf(ttyname, sizeof(ttyname), "/dev/%s", dp->d_name);
			if (stat(ttyname, &st) == -1 || !S_ISCHR(st.st_mode))
				continue;
			LOG_INFO("Found serial port %s", ttyname);
			add_combos(ttyname);
		}
	}
	ret = true;

out:
	if (sys)
		closedir(sys);
	retval = chdir(cwd);
	if (ret) // do we need to fall back to the probe code below?
		return;

	const char* tty_fmt[] = {
		"/dev/ttyS%u",
		"/dev/ttyUSB%u",
		"/dev/usb/ttyUSB%u"
	};

	for (size_t i = 0; i < sizeof(tty_fmt)/sizeof(*tty_fmt); i++) {
		for (unsigned j = 0; j < TTY_MAX; j++) {
			snprintf(ttyname, sizeof(ttyname), tty_fmt[i], j);
			if ( !(stat(ttyname, &st) == 0 && S_ISCHR(st.st_mode)) )
				continue;

			LOG_INFO("Found serial port %s", ttyname);
			add_combos(ttyname);
		}
	}

}
#endif // __linux__

//======================================================================
// APPLE init_port_combos
//======================================================================

#ifdef __APPLE__
#ifndef PATH_MAX
#  define PATH_MAX 1024
#endif

void init_port_combos()
{
	int retval;

	clear_combos();

	struct stat st;

	const char* tty_fmt[] = {
		"/dev/cu.*",
		"/dev/tty.*"
	};

	glob_t gbuf;

	for (size_t i = 0; i < sizeof(tty_fmt)/sizeof(*tty_fmt); i++) {
		glob(tty_fmt[i], 0, NULL, &gbuf);
		for (size_t j = 0; j < gbuf.gl_pathc; j++) {
			if ( !(stat(gbuf.gl_pathv[j], &st) == 0 && S_ISCHR(st.st_mode)) ||
			     strstr(gbuf.gl_pathv[j], "modem") )
				continue;
			LOG_INFO("Found serial port %s", gbuf.gl_pathv[j]);
			add_combos(gbuf.gl_pathv[j]);
		}
		globfree(&gbuf);
	}
}
#endif //__APPLE__

//======================================================================
// FreeBSD init_port_combos
//======================================================================

#ifdef __FreeBSD__
#ifndef PATH_MAX
#  define PATH_MAX 1024
#endif
#  define TTY_MAX 8

void init_port_combos()
{
	int retval;
	struct stat st;
	char ttyname[PATH_MAX + 1];
	const char* tty_fmt[] = {
		"/dev/ttyd%u"
	};

	clear_combos();

	for (size_t i = 0; i < sizeof(tty_fmt)/sizeof(*tty_fmt); i++) {
		for (unsigned j = 0; j < TTY_MAX; j++) {
			snprintf(ttyname, sizeof(ttyname), tty_fmt[i], j);
			if ( !(stat(ttyname, &st) == 0 && S_ISCHR(st.st_mode)) )
				continue;
			LOG_INFO("Found serial port %s", ttyname);
			add_combos(ttyname);
		}
	}
}
#endif //__FreeBSD__

void cbCancelXcvrDialog()
{
	bypass_serial_thread_loop = false;

	btnOkXcvrDialog->labelcolor(FL_BLACK);
	dlgXcvrConfig->hide();
}

void cbOkXcvrDialog()
{
	// close the current rig control
	closeRig();

	pthread_mutex_lock(&mutex_serial);
		RigSerial.ClosePort();
		AuxSerial.ClosePort();
		SepSerial.ClosePort();
		bypass_serial_thread_loop = true;
	pthread_mutex_unlock(&mutex_serial);


	string p1 = selectCommPort->value();
	string p2 = selectAuxPort->value();
	string p3 = selectSepPTTPort->value();

	if (p1.compare("NONE") != 0)
		if (p1 == p2 || p1 == p3) {
			fl_message("Select separate ports");
			return;
		}

	if (p2.compare("NONE") != 0)
		if (p2 == p3) {
			fl_message("Select separate ports");
			return;
		}

	if (rig_nbr != selectRig->index()) {
		clearList();
		saveFreqList();
		rig_nbr = selectRig->index();
		selrig = rigs[rig_nbr];
	}

	progStatus.rig_nbr = rig_nbr;

	progStatus.xcvr_serial_port = selectCommPort->value();
	progStatus.aux_serial_port = selectAuxPort->value();
	progStatus.sep_serial_port = selectSepPTTPort->value();

	progStatus.comm_baudrate = mnuBaudrate->index();
	progStatus.stopbits = btnOneStopBit->value() ? 1 : 2;
	progStatus.comm_retries = (int)cntRigCatRetries->value();
	progStatus.comm_timeout = (int)cntRigCatTimeout->value();
	progStatus.comm_wait = (int)cntRigCatWait->value();
	progStatus.comm_echo = btnRigCatEcho->value();
	progStatus.comm_rtsptt = btnrtsptt->value();
	progStatus.comm_catptt = btncatptt->value();
	progStatus.comm_dtrptt = btndtrptt->value();
	progStatus.comm_rtscts = chkrtscts->value();
	progStatus.comm_rtsplus = btnrtsplus->value();
	progStatus.comm_dtrplus = btndtrplus->value();

	progStatus.sep_dtrplus = btnSepDTRplus->value();
	progStatus.sep_dtrptt = btnSepDTRptt->value();
	progStatus.sep_rtsplus = btnSepRTSplus->value();
	progStatus.sep_rtsptt = btnSepRTSptt->value();

	progStatus.opMODE = selrig->def_mode;
	progStatus.opBW   = selrig->defbw_;
	progStatus.freq   = selrig->deffreq_;

	init_title();

	if (!startXcvrSerial()) {
		if (progStatus.xcvr_serial_port.compare("NONE") == 0) {
			LOG_WARN("No comm port ... test mode");
		} else {
			progStatus.xcvr_serial_port = "NONE";
			selectCommPort->value(progStatus.xcvr_serial_port.c_str());
		}
	}
	if (!startAuxSerial()) {
		if (progStatus.aux_serial_port.compare("NONE") != 0) {
			progStatus.aux_serial_port = "NONE";
			selectAuxPort->value(progStatus.aux_serial_port.c_str());
		}
	}
	if (!startSepSerial()) {
		if (progStatus.sep_serial_port.compare("NONE") != 0) {
			progStatus.sep_serial_port = "NONE";
			selectSepPTTPort->value(progStatus.sep_serial_port.c_str());
		}
	}

	initRig();

	cbCancelXcvrDialog();
}

void configXcvr()
{
//	pthread_mutex_lock(&mutex_serial);
//		bypass_serial_thread_loop = true;
//	pthread_mutex_unlock(&mutex_serial);

	selectCommPort->value(progStatus.xcvr_serial_port.c_str());
	selectAuxPort->value(progStatus.aux_serial_port.c_str());
	selectSepPTTPort->value(progStatus.sep_serial_port.c_str());

	dlgXcvrConfig->show();
}

void createXcvrDialog()
{
	dlgXcvrConfig = XcvrDialog();
	dlgControls = make_XcvrXtra();
	tt550_controls = make_TT550();

	init_port_combos();

	mnuBaudrate->clear();
	for (int i = 0; szBaudRates[i] != NULL; i++)
		mnuBaudrate->add(szBaudRates[i]);

	cbo_agc_level->clear();
	cbo_agc_level->add("slow");
	cbo_agc_level->add("med");
	cbo_agc_level->add("fast");
	cbo_agc_level->index(progStatus.agc_level);

	cbo_tt550_agc_level->add("slow");
	cbo_tt550_agc_level->add("med");
	cbo_tt550_agc_level->add("fast");
	cbo_tt550_agc_level->index(progStatus.tt550_agc_level);

	cbo_tt550_nb_level->add("NONE");
	cbo_tt550_nb_level->add("1");
	cbo_tt550_nb_level->add("2");
	cbo_tt550_nb_level->add("3");
	cbo_tt550_nb_level->add("4");
	cbo_tt550_nb_level->add("5");
	cbo_tt550_nb_level->add("6");
	cbo_tt550_nb_level->add("7");
	cbo_tt550_nb_level->index(progStatus.tt550_nb_level);

	initRigCombo();
}

// Frequency display font / colors

void cbFreqControlFontBrowser(Fl_Widget*, void*) {
	Fl_Font fnt = fntbrowser->fontNumber();
	progStatus.fontnbr = fnt;
	lblTest->labelfont(fnt);
	dlgDisplayConfig->redraw();
	FreqDisp->font(fnt);
	fntbrowser->hide();
}

void cbPrefFont()
{
	fntbrowser->fontNumber(progStatus.fontnbr);
	fntbrowser->fontFilter(Font_Browser::FIXED_WIDTH);
	fntbrowser->fontFilter(Font_Browser::ALL_TYPES);
	fntbrowser->callback(cbFreqControlFontBrowser);
	fntbrowser->show();
}

uchar fg_red, fg_green, fg_blue;
uchar bg_red, bg_green, bg_blue;
uchar sl_red, sl_green, sl_blue;

void cbPrefBackground()
{
	const char *title = "Background color";
	Fl::get_color(lblTest->color(), bg_red, bg_green, bg_blue);
	if (fl_color_chooser(title, bg_red, bg_green, bg_blue))
		lblTest->color(fl_rgb_color (bg_red, bg_green, bg_blue));
	dlgDisplayConfig->redraw();
}

void cbPrefForeground()
{
	const char *title = "Foreground color";
	Fl::get_color(lblTest->labelcolor(), bg_red, bg_green, bg_blue);
	if (fl_color_chooser(title, fg_red, fg_green, fg_blue))
		lblTest->labelcolor(fl_rgb_color (fg_red, fg_green, fg_blue));
	dlgDisplayConfig->redraw();
}

void cbBacklightColor()
{
	uchar red, green, blue;
	Fl::get_color(scaleSmeterColor->color(), red, green, blue);
	if (fl_color_chooser("Backlight color", red, green, blue)) {
		scaleSmeterColor->color(fl_rgb_color (red, green, blue));
		grpMeterColor->color(fl_rgb_color (red, green, blue));
	}
	grpMeterColor->redraw();
}

void cbSMeterColor()
{
	uchar red, green, blue;
	Fl::get_color(sldrRcvSignalColor->color(), red, green, blue);
	if (fl_color_chooser("S Meter color", red, green, blue))
		sldrRcvSignalColor->color(fl_rgb_color (red, green, blue));
	grpMeterColor->redraw();
}

uchar pRed, pGreen, pBlue;

void cbPwrMeterColor()
{
	pRed = progStatus.pwrRed;
	pGreen = progStatus.pwrGreen;
	pBlue = progStatus.pwrBlue;
	fl_color_chooser("Power color", pRed, pGreen, pBlue);
}

uchar sRed, sGreen, sBlue;

void cbSWRMeterColor()
{
	sRed = progStatus.swrRed;
	sGreen = progStatus.swrGreen;
	sBlue = progStatus.swrBlue;
	fl_color_chooser("Power color", sRed, sGreen, sBlue);
}

void cbOkDisplayDialog()
{
	FreqDisp->SetONOFFCOLOR(lblTest->labelcolor(), lblTest->color());
	
	txtInactive->color(lblTest->color());
	txtInactive->labelcolor(lblTest->labelcolor());
	txtInactive->redraw();

	scaleSmeter->color(scaleSmeterColor->color());
	scalePower->color(scaleSmeterColor->color());
	btnALC_SWR->color(scaleSmeterColor->color());
	grpMeters->color(scaleSmeterColor->color());
	
	sldrRcvSignal->color(sldrRcvSignalColor->color(), scaleSmeter->color());
	sldrFwdPwr->color(fl_rgb_color(pRed, pGreen, pBlue), scaleSmeter->color());
	sldrALC_SWR->color(fl_rgb_color(sRed, sGreen, sBlue), scaleSmeter->color());
	progStatus.swrRed = sRed;
	progStatus.swrGreen = sGreen;
	progStatus.swrBlue = sBlue;
	progStatus.pwrRed = pRed;
	progStatus.pwrGreen = pGreen;
	progStatus.pwrBlue = pBlue;

	grpMeters->redraw();

	dlgDisplayConfig->hide();
}

void cbCancelDisplayDialog()
{
	dlgDisplayConfig->hide();
}

void setDisplayColors()
{
	unsigned char red, green, blue;

	if (dlgDisplayConfig == NULL)
		return;

	FreqDisp->GetONCOLOR(red,green,blue);
	lblTest->labelcolor(fl_rgb_color(red,green,blue));
	FreqDisp->GetOFFCOLOR(red,green,blue);
	lblTest->color(fl_rgb_color(red,green,blue));
	scaleSmeterColor->color(scaleSmeter->color());
	grpMeterColor->color(scaleSmeterColor->color());
	sldrRcvSignalColor->color(sldrRcvSignal->color(), scaleSmeter->color());
	sldrRcvSignalColor->minimum(0);
	sldrRcvSignalColor->maximum(100);
	sldrRcvSignalColor->value(25);
	dlgDisplayConfig->show();
}

void cbCloseMemory()
{
	dlgMemoryDialog->hide();
}

void openMemoryDialog()
{
	if (dlgMemoryDialog == NULL)
		return;
	dlgMemoryDialog->show();
}

void show_controls()
{
	if (rig_nbr == TT550)
		tt550_controls->show();
	else
		dlgControls->show();
}
