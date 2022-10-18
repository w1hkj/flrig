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

#include <FL/Fl.H>
#include <FL/x.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Native_File_Chooser.H>

#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef __WIN32__
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>
#include <glob.h>
#endif

#include "dialogs.h"
#include "rigs.h"
#include "util.h"
#include "debug.h"
#include "serial.h"
#include "support.h"
#include "rigpanel.h"
#include "rigbase.h"
#include "font_browser.h"
#include "ui.h"
#include "status.h"
#include "rig.h"
#include "socket_io.h"
#include "rigpanel.h"
#include "gettext.h"
#include "cwioUI.h"
#include "fskioUI.h"

Fl_Double_Window *dlgDisplayConfig = NULL;
Fl_Double_Window *dlgXcvrConfig = NULL;
Fl_Double_Window *dlgMemoryDialog = NULL;
Fl_Double_Window *dlgControls = NULL;

Font_Browser     *fntbrowser = NULL;

Fl_Color flrig_def_color(int);

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
	select_cwioPORT->add("NONE");
	select_fskioPORT->add("NONE");
}

void add_combos(char *port)
{
	if (progStatus.serialtrace) {
		static char sztr[100];
		snprintf(sztr, sizeof(sztr), "Discovered %s", port);
		ser_trace(1, sztr);
	}
	selectCommPort->add(port);
	selectAuxPort->add(port);
	selectSepPTTPort->add(port);
	select_cwioPORT->add(port);
	select_fskioPORT->add(port);
}

void set_combo_value()
{
	selectCommPort->value(progStatus.xcvr_serial_port.c_str());
	selectAuxPort->value(progStatus.aux_serial_port.c_str());
	selectSepPTTPort->value(progStatus.sep_serial_port.c_str());
	select_cwioPORT->value(progStatus.cwioPORT.c_str());
	select_fskioPORT->value(progStatus.FSK_PORT.c_str());
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
	clear_combos();

	char ttyname[21];
	const char tty_fmt[] = "//./COM%u";

	for (unsigned j = 0; j < TTY_MAX; j++) {
		snprintf(ttyname, sizeof(ttyname), tty_fmt, j);
		if (!open_serial(ttyname))
			continue;
		snprintf(ttyname, sizeof(ttyname), "COM%u", j);
		LOG_WARN("Found serial port %s", ttyname);
		add_combos(ttyname);
	}
	set_combo_value();
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
	struct stat st;
	char ttyname[PATH_MAX + 1];
	bool ret = false;

	DIR* sys = NULL;
	char cwd[PATH_MAX] = { '.', '\0' };

	clear_combos();

	LOG_QUIET("%s","Search for serial ports");

	glob_t gbuf;

	glob("/dev/pts/*", 0, NULL, &gbuf);
	for (size_t j = 0; j < gbuf.gl_pathc; j++) {
		if ( !(stat(gbuf.gl_pathv[j], &st) == 0 && S_ISCHR(st.st_mode)) ||
		     strstr(gbuf.gl_pathv[j], "modem") )
			continue;
		LOG_QUIET("Found virtual serial port %s", gbuf.gl_pathv[j]);
			add_combos(gbuf.gl_pathv[j]);
	}
	globfree(&gbuf);

	glob("/dev/serial/by-id/*", 0, NULL, &gbuf);
	for (size_t j = 0; j < gbuf.gl_pathc; j++) {
		if ( !(stat(gbuf.gl_pathv[j], &st) == 0 && S_ISCHR(st.st_mode)) ||
		     strstr(gbuf.gl_pathv[j], "modem") )
			continue;
		LOG_QUIET("Found serial port %s", gbuf.gl_pathv[j]);
			add_combos(gbuf.gl_pathv[j]);
	}
	globfree(&gbuf);

	glob("/dev/tty*", 0, NULL, &gbuf);
	for (size_t j = 0; j < gbuf.gl_pathc; j++) {
		if ( !(stat(gbuf.gl_pathv[j], &st) == 0 && S_ISCHR(st.st_mode)) ||
		     strstr(gbuf.gl_pathv[j], "modem") )
			continue;
		LOG_QUIET("Found serial port %s", gbuf.gl_pathv[j]);
		add_combos(gbuf.gl_pathv[j]);
	}
	globfree(&gbuf);

	glob("/dev/tnt*", 0, NULL, &gbuf);
	for (size_t j = 0; j < gbuf.gl_pathc; j++) {
		if ( !(stat(gbuf.gl_pathv[j], &st) == 0 && S_ISCHR(st.st_mode)) ||
		     strstr(gbuf.gl_pathv[j], "modem") )
			continue;
		LOG_QUIET("Found serial port %s", gbuf.gl_pathv[j]);
		add_combos(gbuf.gl_pathv[j]);
	}
	globfree(&gbuf);

	if (getcwd(cwd, sizeof(cwd)) == NULL) goto out;

	if (chdir("/sys/class/tty") == -1) goto check_cuse;
	if ((sys = opendir(".")) == NULL) goto check_cuse;

	ssize_t len;
	struct dirent* dp;

	LOG_QUIET("%s", "Searching /sys/class/tty/");

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
			LOG_QUIET("Found serial port %s", ttyname);
			add_combos(ttyname);
			ret = true;
		}
	}

check_cuse:
	if (sys) {
		closedir(sys);
		sys = NULL;
	}
	if (chdir("/sys/class/cuse") == -1) goto out;
	if ((sys = opendir(".")) == NULL) goto out;

	LOG_QUIET("%s", "Searching /sys/class/cuse/");

	while ((dp = readdir(sys))) {
#  ifdef _DIRENT_HAVE_D_TYPE
		if (dp->d_type != DT_LNK)
			continue;
#  endif
		if ((len = readlink(dp->d_name, ttyname, sizeof(ttyname)-1)) == -1)
			continue;
		ttyname[len] = '\0';
		if (strstr(ttyname, "/devices/virtual/") && !strncmp(dp->d_name, "mhuxd", 5)) {
			char *name = strdup(dp->d_name);
			if(!name)
				continue;
			char *p = strchr(name, '!');
			if(p)
				*p = '/';
			snprintf(ttyname, sizeof(ttyname), "/dev/%s", name);
			free(name);
			if (stat(ttyname, &st) == -1 || !S_ISCHR(st.st_mode))
				continue;
			LOG_QUIET("Found serial port %s", ttyname);
			add_combos(ttyname);
			ret = true;
		}
	}

out:
	std::string tty_virtual = HomeDir;
	tty_virtual.append("vdev");

	LOG_QUIET("Searching %s", tty_virtual.c_str());

	tty_virtual.append("/ttyS%u");
	for (unsigned j = 0; j < TTY_MAX; j++) {
		snprintf(ttyname, sizeof(ttyname), tty_virtual.c_str(), j);
		if ( !(stat(ttyname, &st) == 0 && S_ISCHR(st.st_mode)) )
			continue;
		LOG_QUIET("Found serial port %s", ttyname);
		add_combos(ttyname);
	}

	if (sys) closedir(sys);
	if (chdir(cwd) == -1) return;
	if (ret) { // do we need to fall back to the probe code below?
		set_combo_value();
		return;
	}

	const char* tty_fmt[] = {
		"/dev/ttyS%u",
		"/dev/ttyUSB%u",
		"/dev/usb/ttyUSB%u"
	};
	LOG_QUIET("%s", "Serial port discovery via 'stat'");
	for (size_t i = 0; i < sizeof(tty_fmt)/sizeof(*tty_fmt); i++) {
		for (unsigned j = 0; j < TTY_MAX; j++) {
			snprintf(ttyname, sizeof(ttyname), tty_fmt[i], j);
			if ( !(stat(ttyname, &st) == 0 && S_ISCHR(st.st_mode)) )
				continue;

			LOG_WARN("Found serial port %s", ttyname);
			add_combos(ttyname);
		}
	}
	set_combo_value();
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
	std::string pname;
	const char* tty_fmt[] = {
		"/dev/cu.*",
		"/dev/tty.*"
	};
	struct stat st;

	clear_combos();
	glob_t gbuf;
	bool is_serial;

	clear_combos();
	for (size_t i = 0; i < sizeof(tty_fmt)/sizeof(*tty_fmt); i++) {
		glob(tty_fmt[i], 0, NULL, &gbuf);
		for (size_t j = 0; j < gbuf.gl_pathc; j++) {
			int ret1 = !stat(gbuf.gl_pathv[j], &st);
			int ret2 = S_ISCHR(st.st_mode);
			if (ret1) {
				LOG_INFO("Serial port %s", gbuf.gl_pathv[j]);
				LOG_INFO("  device mode:     %X", st.st_mode);
				LOG_INFO("  char device?     %s", ret2 ? "Y" : "N");
			} else
				LOG_INFO("%s does not return stat query", gbuf.gl_pathv[j]);
			if ( (ret1 && ret2 ) || strstr(gbuf.gl_pathv[j], "modem") )
				add_combos (gbuf.gl_pathv[j]);
		}
		globfree(&gbuf);
	}

	set_combo_value();
}
#endif //__APPLE__
//======================================================================

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
			LOG_WARN("Found serial port %s", ttyname);
			add_combos(ttyname);
		}
	}
}
#endif //__FreeBSD__
//======================================================================

//======================================================================
// OpenBSD init_port_combos
//======================================================================

#ifdef __OpenBSD__

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
		"/dev/ttyd%u",
		"/dev/ttyU%u",
	}

	clear_combos();

	for (size_t i = 0; i < sizeof(tty_fmt)/sizeof(*tty_fmt); i++) {
		for (unsigned j = 0; j < TTY_MAX; j++) {
			snprintf(ttyname, sizeof(ttyname), tty_fmt[i], j);
			if ( !(stat(ttyname, &st) == 0 && S_ISCHR(st.st_mode)) )
				continue;
			LOG_WARN("Found serial port %s", ttyname);
			add_combos(ttyname);
		}
	}
}

#endif //__OpenBSD__
//======================================================================

void cbCIVdefault()
{
	char hexstr[8];
	int picked = selectRig->index();
	rigbase *srig = rigs[picked];
	snprintf(hexstr, sizeof(hexstr), "0x%02X", srig->defaultCIV);
	txtCIV->value(hexstr);
	progStatus.CIV = srig->defaultCIV;
	srig->adjustCIV(progStatus.CIV);
}

void cbCIV()
{
	int picked = selectRig->index();
	int adr = 0;
	rigbase *srig = rigs[picked];
	sscanf(txtCIV->value(), "0x%2X", &adr);
	progStatus.CIV = adr;
	srig->adjustCIV(progStatus.CIV);
}

void cbUSBaudio()
{
	progStatus.USBaudio = btnUSBaudio->value();
}

void configXcvr()
{

	selectCommPort->value(progStatus.xcvr_serial_port.c_str());
	selectAuxPort->value(progStatus.aux_serial_port.c_str());
	selectSepPTTPort->value(progStatus.sep_serial_port.c_str());

	if (selrig->CIV) {
		char hexstr[8];
		snprintf(hexstr, sizeof(hexstr), "0x%02X", selrig->CIV);
		txtCIV->value(hexstr);
		txtCIV->activate();
		btnCIVdefault->activate();
		if (xcvr_name == rig_IC7200.name_ || 
			xcvr_name == rig_IC7300.name_ ||
			xcvr_name == rig_IC7600.name_ ) {
			btnUSBaudio->value(progStatus.USBaudio);
			btnUSBaudio->activate();
		} else
			btnUSBaudio->deactivate();
	} else {
		txtCIV->value("");
		txtCIV->deactivate();
		btnCIVdefault->deactivate();
		btnUSBaudio->value(0);
		btnUSBaudio->deactivate();
	}

	select_tab(_("Xcvr"));
}

void open_poll_tab()
{
	select_tab(_("Poll"));
}

void open_trace_tab()
{
	select_tab(_("Trace"));
}

void open_commands_tab()
{
	select_tab(_("Commands"));
}

void open_restore_tab()
{
	select_tab(_("Restore"));
}

void open_send_command_tab()
{
	select_tab(_("Send"));
}

void open_tcpip_tab()
{
	select_tab(_("TCPIP & TCI"));
}

void open_cmedia_tab()
{
	select_tab(_("PTT-Cmedia"));
}

void open_tmate2_tab()
{
	select_tab(_("TMATE-2"));
}

void open_ptt_tab()
{
	select_tab(_("PTT-Generic"));
}

void open_gpio_tab()
{
	select_tab(_("PTT-GPIO"));
}

void open_other_tab()
{
	select_tab(_("Other"));
}

void open_server_tab()
{
	select_tab(_("Server"));
}

void createXcvrDialog()
{
	dlgXcvrConfig = XcvrDialog();

	init_port_combos();

	mnuBaudrate->clear();
	for (int i = 0; szBaudRates[i] != NULL; i++)
		mnuBaudrate->add(szBaudRates[i]);

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
Fl_Font selfont;

void cbFreqControlFontBrowser(Fl_Widget*, void*) {
	selfont = fntbrowser->fontNumber();
	lblTest->labelfont(selfont);
	dlgDisplayConfig->redraw();
	fntbrowser->hide();
}

void cbPrefFont()
{
	fntbrowser->fontNumber(progStatus.fontnbr);
//	fntbrowser->fontFilter(Font_Browser::FIXED_WIDTH);
//	fntbrowser->fontFilter(Font_Browser::ALL_TYPES);
	fntbrowser->callback(cbFreqControlFontBrowser);
	fntbrowser->show();
}

uchar fg_red, fg_green, fg_blue;
uchar bg_red, bg_green, bg_blue;
uchar smeterRed, smeterGreen, smeterBlue;
uchar peakRed, peakGreen, peakBlue;
uchar pwrRed, pwrGreen, pwrBlue;
uchar swrRed, swrGreen, swrBlue;
uchar voltRed, voltGreen, voltBlue;

Fl_Color bgclr;
Fl_Color fgclr;

Fl_Color fgsys;
static uchar fg_sys_red, fg_sys_green, fg_sys_blue;

Fl_Color bgsys;
static uchar bg_sys_red, bg_sys_green, bg_sys_blue;

Fl_Color bg2sys;
static uchar bg2_sys_red, bg2_sys_green, bg2_sys_blue;

Fl_Color bg_slider;
static uchar bg_slider_red, bg_slider_green, bg_slider_blue;

Fl_Color btn_slider;
static uchar btn_slider_red, btn_slider_green, btn_slider_blue;

Fl_Color btn_lt_color;
static uchar btn_lt_color_red, btn_lt_color_green, btn_lt_color_blue;

Fl_Color tab_color;
static uchar tab_red, tab_green, tab_blue;

void cb_lighted_button()
{
	uchar r = btn_lt_color_red, g = btn_lt_color_green, b = btn_lt_color_blue;
	if (fl_color_chooser("Foreground color", r, g, b)) {
		btn_lt_color_red = r; btn_lt_color_green = g; btn_lt_color_blue = b;
		btn_lt_color = fl_rgb_color(r, g, b);
		btn_lighted->selection_color(btn_lt_color);
		btn_lighted->value(1);
		btn_lighted->redraw();
	}
}

void cb_lighted_default()
{
	btn_lt_color = flrig_def_color(FL_YELLOW);
	btn_lt_color_red = ((btn_lt_color >> 24) & 0xFF);
	btn_lt_color_green = ((btn_lt_color >> 16) & 0xFF);
	btn_lt_color_blue = ((btn_lt_color >> 8) & 0xFF);
	btn_lighted->selection_color(btn_lt_color);
	btn_lighted->value(1);
	btn_lighted->redraw();
}

void cb_change_hrd_button()
{
	progStatus.hrd_buttons = !progStatus.hrd_buttons;
	FreqDispA->set_hrd(progStatus.hrd_buttons);
	FreqDispB->set_hrd(progStatus.hrd_buttons);
}

void set_sliders_when()
{
	if (sldrSQUELCH)
		sldrSQUELCH->when(progStatus.sliders_button);
	if (sldrMICGAIN)
		sldrMICGAIN->when(progStatus.sliders_button);
	if (sldrIFSHIFT)
		sldrIFSHIFT->when(progStatus.sliders_button);
	if (sldrNR)
		sldrNR->when(progStatus.sliders_button);
	if (sldrNOTCH)
		sldrNOTCH->when(progStatus.sliders_button);
	if (sldrRFGAIN)
		sldrRFGAIN->when(progStatus.sliders_button);
	if (sldrINNER)
		sldrINNER->when(progStatus.sliders_button);
	if (sldrOUTER)
		sldrOUTER->when(progStatus.sliders_button);
	if (sldrPOWER)
		sldrPOWER->when(progStatus.sliders_button);
	if (sldrVOLUME)
		sldrVOLUME->when(progStatus.sliders_button);
	if (ic7610_digi_sel_val)
		ic7610_digi_sel_val->when(progStatus.sliders_button);
	if (sldr_nb_level)
		sldr_nb_level->when(progStatus.sliders_button);
}

void cb_change_sliders_button()
{
	if (progStatus.sliders_button == FL_WHEN_CHANGED)
		progStatus.sliders_button = FL_WHEN_RELEASE;
	else
		progStatus.sliders_button = FL_WHEN_CHANGED;
	set_sliders_when();
}

void cb_slider_defaults()
{
	bg_slider_red = 232;
	bg_slider_green = 255;
	bg_slider_blue = 232;

	btn_slider_red = 0;
	btn_slider_green = 0;
	btn_slider_blue = 128;

	bg_slider = fl_rgb_color( 232, 255, 232);
	btn_slider = fl_rgb_color( 0, 0, 128);

	sldrColors->color(bg_slider);
	sldrColors->selection_color(btn_slider);
	sldrColors->redraw();
}

void cb_slider_background()
{
	uchar r = bg_slider_red, g = bg_slider_green, b = bg_slider_blue;
	if (fl_color_chooser("Foreground color", r, g, b)) {
		bg_slider_red = r; bg_slider_green = g; bg_slider_blue = b;
		bg_slider = fl_rgb_color(r, g, b);
		sldrColors->color(bg_slider);
		sldrColors->selection_color(btn_slider);
		sldrColors->redraw();
	}
}

void cb_slider_select()
{
	uchar r = btn_slider_red, g = btn_slider_green, b = btn_slider_blue;
	if (fl_color_chooser("Foreground color", r, g, b)) {
		btn_slider_red = r; btn_slider_green = g; btn_slider_blue = b;
		btn_slider = fl_rgb_color(r, g, b);
		sldrColors->color(bg_slider);
		sldrColors->selection_color(btn_slider);
		sldrColors->redraw();
	}
}

void cb_sys_defaults()
{
	bgsys = flrig_def_color(FL_BACKGROUND_COLOR);
	bg_sys_red = ((bgsys >> 24) & 0xFF);
	bg_sys_green = ((bgsys >> 16) & 0xFF);
	bg_sys_blue = ((bgsys >> 8) & 0xFF);

	bg2sys = flrig_def_color(FL_BACKGROUND2_COLOR);
	bg2_sys_red = ((bg2sys) >> 24 & 0xFF);
	bg2_sys_green = ((bg2sys) >> 16 & 0xFF);
	bg2_sys_blue = ((bg2sys) >> 8 & 0xFF);

	fgsys = flrig_def_color(FL_FOREGROUND_COLOR);
	fg_sys_red = (fgsys >> 24) & 0xFF;
	fg_sys_green = (fgsys >> 16) & 0xFF;
	fg_sys_blue = (fgsys >> 8) & 0xFF;

	Fl::background(bg_sys_red, bg_sys_green, bg_sys_blue);
	Fl::background2(bg2_sys_red, bg2_sys_green, bg2_sys_blue);
	Fl::foreground(fg_sys_red, fg_sys_green, fg_sys_blue);

	dlgDisplayConfig->redraw();
	mainwindow->redraw();
}

void cb_sys_foreground()
{
	uchar r = fg_sys_red, g = fg_sys_green, b = fg_sys_blue;
	if (fl_color_chooser("Foreground color", r, g, b)) {
		fg_sys_red = r; fg_sys_green = g; fg_sys_blue = b;
		fgsys = fl_rgb_color(r, g, b);
		Fl::foreground(r, g, b);
		dlgDisplayConfig->redraw();
		mainwindow->redraw();
	}
}

void cb_sys_background()
{
	uchar r = bg_sys_red, g = bg_sys_green, b = bg_sys_blue;
	if (fl_color_chooser("Background color", r, g, b)) {
		bg_sys_red = r; bg_sys_green = g; bg_sys_blue = b;
		bgsys = fl_rgb_color(r, g, b);
		Fl::background(r, g, b);
		dlgDisplayConfig->redraw();
		mainwindow->redraw();
	}
}

void cb_sys_background2()
{
	uchar r = bg2_sys_red, g = bg2_sys_green, b = bg2_sys_blue;
	if (fl_color_chooser("Background2 color", r, g, b)) {
		bg2_sys_red = r; bg2_sys_green = g; bg2_sys_blue = b;
		bg2sys = fl_rgb_color(r, g, b);
		Fl::background2(r, g, b);
		dlgDisplayConfig->redraw();
		mainwindow->redraw();
	}
}

void cbBacklightColor()
{
	uchar r = bg_red, g = bg_green, b = bg_blue;
	if (fl_color_chooser("Background color", r, g, b)) {
		bg_red = r; bg_green = g; bg_blue = b;
		bgclr = fl_rgb_color(r, g, b);
		lblTest->color(bgclr);

		sldrRcvSignalColor->color( fl_rgb_color (smeterRed, smeterGreen, smeterBlue), bgclr );
		sldrPWRcolor->color(fl_rgb_color (pwrRed, pwrGreen, pwrBlue), bgclr);
		sldrSWRcolor->color(fl_rgb_color (swrRed, swrGreen, swrBlue), bgclr);
		sldrVoltcolor->color(fl_rgb_color (voltRed, voltGreen, voltBlue), bgclr);

		scaleSmeterColor->color(bgclr);
		scalePWRcolor->color(bgclr);
		scaleSWRcolor->color(bgclr);
		scaleVoltcolor->color(bgclr);
		grpMeterColor->color(bgclr);

		dlgDisplayConfig->redraw();
	}
}

void cbPrefForeground()
{
	uchar r = fg_red, g = fg_green, b = fg_blue;
	if (fl_color_chooser("Foreground color", r, g, b)) {
		fg_red = r; fg_green = g; fg_blue = b;
		fgclr = fl_rgb_color(r, g, b);
		lblTest->labelcolor(fgclr);

		scaleSmeterColor->labelcolor(fgclr);
		scalePWRcolor->labelcolor(fgclr);
		scaleSWRcolor->labelcolor(fgclr);
		scaleVoltcolor->labelcolor(fgclr);

		grpMeterColor->labelcolor(fgclr);
		dlgDisplayConfig->redraw();
	}
}

void default_meters()
{
	Fl_Color c;
	bg_red = 232; bg_green = 255; bg_blue = 232;
	bgclr = fl_rgb_color( bg_red, bg_green, bg_blue);
		lblTest->color(bgclr);
		sldrRcvSignalColor->color( fl_rgb_color (smeterRed, smeterGreen, smeterBlue), bgclr );
		sldrPWRcolor->color(fl_rgb_color (pwrRed, pwrGreen, pwrBlue), bgclr);
		sldrSWRcolor->color(fl_rgb_color (swrRed, swrGreen, swrBlue), bgclr);
		scaleSmeterColor->color(bgclr);
		scalePWRcolor->color(bgclr);
		scaleSWRcolor->color(bgclr);
		scaleVoltcolor->color(bgclr);
		grpMeterColor->color(bgclr);

	fg_red = 0; fg_green = 0; fg_blue = 0;
	fgclr = (Fl_Color)0;
		lblTest->labelcolor(fgclr);
		scaleSmeterColor->labelcolor(fgclr);
		scalePWRcolor->labelcolor(fgclr);
		scaleSWRcolor->labelcolor(fgclr);
		scaleVoltcolor->labelcolor(fgclr);
		grpMeterColor->labelcolor(fgclr);
	smeterRed = 0; smeterGreen = 180; smeterBlue = 0;
		c = fl_rgb_color (smeterRed, smeterGreen, smeterBlue);
		sldrRcvSignalColor->color(c, bgclr );
	peakRed = 255; peakGreen = 0; peakBlue = 0;
		c = fl_rgb_color( peakRed, peakGreen, peakBlue );
		sldrRcvSignalColor->PeakColor(c);
		sldrPWRcolor->PeakColor(c);
		sldrSWRcolor->PeakColor(c);
		sldrVoltcolor->PeakColor(bgclr);
	pwrRed = 180; pwrGreen = 0; pwrBlue = 0;
		c = fl_rgb_color( pwrRed, pwrGreen, pwrBlue );
		sldrPWRcolor->color(c, bgclr);
	swrRed = 148; swrGreen = 0; swrBlue = 148;
		c = fl_rgb_color(swrRed, swrGreen, swrBlue);
		sldrSWRcolor->color(c, bgclr);
	voltRed = 0; voltGreen = 0; voltBlue = 128;
	voltRed = 0; voltGreen = 0; voltBlue = 255;
		c = fl_rgb_color(voltRed, voltGreen, voltBlue);
		sldrVoltcolor->color(c, bgclr);

	dlgDisplayConfig->redraw();
}

void cbSMeterColor()
{
	uchar r = smeterRed, g = smeterGreen, b = smeterBlue;
	if (fl_color_chooser("S Meter color", r, g, b)) {
		smeterRed = r; smeterGreen = g; smeterBlue = b;
		sldrRcvSignalColor->color(
			fl_rgb_color (r, g, b),
			bgclr );
		dlgDisplayConfig->redraw();
	}
}

void cbPeakMeterColor()
{
	uchar r = peakRed, g = peakGreen, b = peakBlue;
	if (fl_color_chooser("Peak value color", r, g, b)) {
		peakRed = r; peakGreen = g; peakBlue = b;
		sldrRcvSignalColor->PeakColor(fl_rgb_color (r, g, b));
		sldrPWRcolor->PeakColor(fl_rgb_color (r, g, b));
		sldrSWRcolor->PeakColor(fl_rgb_color (r, g, b));
		dlgDisplayConfig->redraw();
	}
}

void cbPwrMeterColor()
{
	uchar r = pwrRed, g = pwrGreen, b = pwrBlue;
	if (fl_color_chooser("Power meter color", r, g, b)) {
		pwrRed = r; pwrGreen = g; pwrBlue = b;
		sldrPWRcolor->color(
			fl_rgb_color (r, g, b),
			bgclr );
		dlgDisplayConfig->redraw();
	}
}

void cbSWRMeterColor()
{
	uchar r = swrRed, g = swrGreen, b = swrBlue;
	if (fl_color_chooser("SWR meter color", r, g, b)) {
		swrRed = r; swrGreen = g; swrBlue = b;
		sldrSWRcolor->color(
			fl_rgb_color (swrRed, swrGreen, swrBlue),
			bgclr );
		dlgDisplayConfig->redraw();
	}
}

void cbVoltMeterColor()
{
	uchar r = voltRed, g = voltGreen, b = voltBlue;
	if (fl_color_chooser("Volt meter color", r, g, b)) {
		voltRed = r; voltGreen = g; voltBlue = b;
		sldrVoltcolor->color(
			fl_rgb_color (voltRed, voltGreen, voltBlue),
			bgclr );
		dlgDisplayConfig->redraw();
	}
}

void cb_tab_defaults()
{
	tab_red = 230;
	tab_green = 230;
	tab_blue = 230;

	tab_color = fl_rgb_color( tab_red, tab_green, tab_blue);

	btn_tab_color->color(tab_color);
	btn_tab_color->redraw();

	if (tabsGeneric) {
		tabsGeneric->selection_color(tab_color);
		tabsGeneric->redraw();
	}
	if (tabs550) {
		tabs550->selection_color(tab_color);
		tabs550->redraw();
	}
	if (tabCmds) {
		tabCmds->selection_color(tab_color);
		tabCmds->redraw();
	}

}

void cb_tab_colors()
{
	uchar r = tab_red, g = tab_green, b = tab_blue;
	if (fl_color_chooser("TAB color", r, g, b)) {
		tab_red = r; tab_green = g; tab_blue = b;
		tab_color = fl_rgb_color(r, g, b);
		btn_tab_color->color(tab_color);
		btn_tab_color->redraw();

		if (tabsGeneric) {
			tabsGeneric->selection_color(tab_color);
			tabsGeneric->redraw();
		}
		if (tabs550) {
			tabs550->selection_color(tab_color);
			tabs550->redraw();
		}
		if (tabCmds) {
			tabCmds->selection_color(tab_color);
			tabCmds->redraw();
		}
	}
}

void setColors()
{
	progStatus.swrRed = swrRed;
	progStatus.swrGreen = swrGreen;
	progStatus.swrBlue = swrBlue;

	progStatus.pwrRed = pwrRed;
	progStatus.pwrGreen = pwrGreen;
	progStatus.pwrBlue = pwrBlue;

	progStatus.smeterRed = smeterRed;
	progStatus.smeterGreen = smeterGreen;
	progStatus.smeterBlue = smeterBlue;

	progStatus.peakRed = peakRed;
	progStatus.peakGreen = peakGreen;
	progStatus.peakBlue = peakBlue;

	progStatus.voltRed = voltRed;
	progStatus.voltGreen = voltGreen;
	progStatus.voltBlue = voltBlue;

	progStatus.fg_red = fg_red;
	progStatus.fg_green = fg_green;
	progStatus.fg_blue = fg_blue;

	progStatus.bg_red = bg_red;
	progStatus.bg_green = bg_green;
	progStatus.bg_blue = bg_blue;

	progStatus.fontnbr = selfont;
	FreqDispA->font(selfont);
	FreqDispB->font(selfont);

	progStatus.fg_sys_red = fg_sys_red;
	progStatus.fg_sys_green = fg_sys_green;
	progStatus.fg_sys_blue = fg_sys_blue;

	progStatus.bg_sys_red = bg_sys_red;
	progStatus.bg_sys_green = bg_sys_green;
	progStatus.bg_sys_blue = bg_sys_blue;

	progStatus.bg2_sys_red = bg2_sys_red;
	progStatus.bg2_sys_green = bg2_sys_green;
	progStatus.bg2_sys_blue = bg2_sys_blue;

	progStatus.slider_red = bg_slider_red;
	progStatus.slider_green = bg_slider_green;
	progStatus.slider_blue = bg_slider_blue;

	progStatus.slider_btn_red = btn_slider_red;
	progStatus.slider_btn_green = btn_slider_green;
	progStatus.slider_btn_blue = btn_slider_blue;

	progStatus.lighted_btn_red = btn_lt_color_red;
	progStatus.lighted_btn_green = btn_lt_color_green;
	progStatus.lighted_btn_blue = btn_lt_color_blue;

	progStatus.tab_red = tab_red;
	progStatus.tab_green = tab_green;
	progStatus.tab_blue = tab_blue;

	if (selrig->inuse == onB) {
		FreqDispB->SetCOLORS( fl_rgb_color(fg_red, fg_green, fg_blue), bgclr);
		FreqDispA->SetCOLORS(
			fl_rgb_color(fg_red, fg_green, fg_blue),
			fl_color_average(bgclr, FL_BLACK, 0.87));
	} else {
		FreqDispA->SetCOLORS( fl_rgb_color(fg_red, fg_green, fg_blue), bgclr);
		FreqDispB->SetCOLORS(
			fl_rgb_color(fg_red, fg_green, fg_blue),
			fl_color_average(bgclr, FL_BLACK, 0.87));
	}

	grpMeters->color(bgclr);

	meter_fill_box->color(bgclr);

	scaleSmeter->color(bgclr);
	scaleSmeter->labelcolor(fgclr);
	mtr_SMETER->color(bgclr);
	mtr_SMETER->labelcolor(fgclr);


	scalePower->color(bgclr);
	scalePower->labelcolor(fgclr);
	mtr_PWR->color(bgclr);
	mtr_PWR->labelcolor(fgclr);

	scaleVoltage->color(bgclr);
	scaleVoltage->labelcolor(fgclr);
	scaleVoltage->redraw();
	mtr_VOLTS->color(bgclr);
	mtr_VOLTS->labelcolor(fgclr);

	btnALC_IDD_SWR->color(bgclr);
	btnALC_IDD_SWR->labelcolor(fgclr);
	btnALC_IDD_SWR->redraw();
	mtr_SWR->color(bgclr);
	mtr_SWR->labelcolor(fgclr);

	sldrFwdPwr->color(fl_rgb_color (pwrRed, pwrGreen, pwrBlue), bgclr);
	sldrFwdPwr->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	sldrVoltage->color(fl_rgb_color (voltRed, voltGreen, voltBlue), bgclr);
	sldrVoltage->PeakColor(bgclr);//fl_rgb_color (voltRed, voltGreen, voltBlue));
	sldrVoltage->redraw();

	sldrRcvSignal->color(fl_rgb_color (smeterRed, smeterGreen, smeterBlue), bgclr);
	sldrRcvSignal->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	sldrALC->color(fl_rgb_color (swrRed, swrGreen, swrBlue), bgclr);
	sldrALC->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	sldrIDD->color(fl_rgb_color (swrRed, swrGreen, swrBlue), bgclr);
	sldrIDD->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	mtr_ALC->color(bgclr);
	mtr_ALC->labelcolor(fgclr);

	sldrIDD->color(fl_rgb_color (swrRed, swrGreen, swrBlue), bgclr);
	sldrIDD->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	mtr_IDD->color(bgclr);
	mtr_IDD->labelcolor(fgclr);

	sldrSWR->color(fl_rgb_color (swrRed, swrGreen, swrBlue), bgclr);
	sldrSWR->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	grpMeters->redraw();

	if (btnVol)				btnVol->selection_color(btn_lt_color);
	if (btnNR)				btnNR->selection_color(btn_lt_color);
	if (btnIFsh)			btnIFsh->selection_color(btn_lt_color);
	if (btnNotch)			btnNotch->selection_color(btn_lt_color);
	if (btnA)				btnA->selection_color(btn_lt_color);
	if (btnB)				btnB->selection_color(btn_lt_color);
	if (btnSplit)			btnSplit->selection_color(btn_lt_color);
	if (btnAttenuator)		btnAttenuator->selection_color(btn_lt_color);
	if (btnPreamp)			btnPreamp->selection_color(btn_lt_color);
	if (btnNOISE)			btnNOISE->selection_color(btn_lt_color);
	if (btnAutoNotch)		btnAutoNotch->selection_color(btn_lt_color);
	if (btnTune)			btnTune->selection_color(btn_lt_color);
	if (btn_tune_on_off)	btn_tune_on_off->selection_color(btn_lt_color);
	if (btnPTT)				btnPTT->selection_color(btn_lt_color);
	if (btnLOCK)			btnLOCK->selection_color(btn_lt_color);
	if (btnAuxRTS)			btnAuxRTS->selection_color(btn_lt_color);
	if (btnAuxDTR)			btnAuxDTR->selection_color(btn_lt_color);
	if (btnSpot)			btnSpot->selection_color(btn_lt_color);
	if (btn_vox)			btn_vox->selection_color(btn_lt_color);
	if (btnCompON)			btnCompON->selection_color(btn_lt_color);
	if (btnPOWER)			btnPOWER->selection_color(btn_lt_color);

	if (btn_tt550_vox)		btn_tt550_vox->selection_color(btn_lt_color);
	if (btn_tt550_CompON)	btn_tt550_CompON->selection_color(btn_lt_color);

	if (sldrVOLUME)			sldrVOLUME->color(bg_slider);
	if (sldrVOLUME)			sldrVOLUME->selection_color(btn_slider);
	if (sldrRFGAIN)			sldrRFGAIN->color(bg_slider);
	if (sldrRFGAIN)			sldrRFGAIN->selection_color(btn_slider);
	if (sldrSQUELCH)		sldrSQUELCH->color(bg_slider);
	if (sldrSQUELCH)		sldrSQUELCH->selection_color(btn_slider);
	if (sldrNR)				sldrNR->color(bg_slider);
	if (sldrNR)				sldrNR->selection_color(btn_slider);
	if (sldrIFSHIFT)		sldrIFSHIFT->color(bg_slider);
	if (sldrIFSHIFT)		sldrIFSHIFT->selection_color(btn_slider);
	if (sldrINNER)			sldrINNER->color(bg_slider);
	if (sldrINNER)			sldrINNER->selection_color(btn_slider);
	if (sldrOUTER)			sldrOUTER->color(bg_slider);
	if (sldrOUTER)			sldrOUTER->selection_color(btn_slider);
	if (sldrNOTCH)			sldrNOTCH->color(bg_slider);
	if (sldrNOTCH)			sldrNOTCH->selection_color(btn_slider);
	if (sldrMICGAIN)		sldrMICGAIN->color(bg_slider);
	if (sldrMICGAIN)		sldrMICGAIN->selection_color(btn_slider);
	if (sldrPOWER)			sldrPOWER->color(bg_slider);
	if (sldrPOWER)			sldrPOWER->selection_color(btn_slider);

	if (spnrPOWER)			spnrPOWER->color(bg_slider);
	if (spnrVOLUME)			spnrVOLUME->color(bg_slider);

	mainwindow->redraw();
}

void cb_reset_display_dialog()
{
	cb_sys_defaults();
	cb_lighted_default();
	cb_slider_defaults();
	default_meters();
	setColors();
}

void cbOkDisplayDialog()
{
	setColors();
	dlgDisplayConfig->hide();
}

void cbCancelDisplayDialog()
{
	dlgDisplayConfig->hide();
}

void setDisplayColors()
{
	if (dlgDisplayConfig == NULL)
		return;

	swrRed = progStatus.swrRed;
	swrGreen = progStatus.swrGreen;
	swrBlue = progStatus.swrBlue;

	pwrRed = progStatus.pwrRed;
	pwrGreen = progStatus.pwrGreen;
	pwrBlue = progStatus.pwrBlue;

	smeterRed = progStatus.smeterRed;
	smeterGreen = progStatus.smeterGreen;
	smeterBlue = progStatus.smeterBlue;

	peakRed = progStatus.peakRed;
	peakGreen = progStatus.peakGreen;
	peakBlue = progStatus.peakBlue;

	voltRed = progStatus.voltRed;
	voltGreen = progStatus.voltGreen;
	voltBlue = progStatus.voltBlue;

	fg_red = progStatus.fg_red;
	fg_green = progStatus.fg_green;
	fg_blue = progStatus.fg_blue;

	bg_red = progStatus.bg_red;
	bg_green = progStatus.bg_green;
	bg_blue = progStatus.bg_blue;

	bgclr = fl_rgb_color(bg_red, bg_green, bg_blue);
	fgclr = fl_rgb_color(fg_red, fg_green, fg_blue);

	fg_sys_red = progStatus.fg_sys_red;
	fg_sys_green = progStatus.fg_sys_green;
	fg_sys_blue = progStatus.fg_sys_blue;

	bg_sys_red = progStatus.bg_sys_red;
	bg_sys_green = progStatus.bg_sys_green;
	bg_sys_blue = progStatus.bg_sys_blue;

	bg2_sys_red = progStatus.bg2_sys_red;
	bg2_sys_green = progStatus.bg2_sys_green;
	bg2_sys_blue = progStatus.bg2_sys_blue;

	bg_slider_red = progStatus.slider_red;
	bg_slider_green = progStatus.slider_green;
	bg_slider_blue = progStatus.slider_blue;

	btn_slider_red = progStatus.slider_btn_red;
	btn_slider_green = progStatus.slider_btn_green;
	btn_slider_blue = progStatus.slider_btn_blue;

	tab_red = progStatus.tab_red;
	tab_green = progStatus.tab_green;
	tab_blue = progStatus.tab_blue;

	sldrColors->color(fl_rgb_color(bg_slider_red, bg_slider_green, bg_slider_blue));
	sldrColors->selection_color(fl_rgb_color(btn_slider_red, btn_slider_green, btn_slider_blue));

	btn_lt_color_red = progStatus.lighted_btn_red;
	btn_lt_color_green = progStatus.lighted_btn_green;
	btn_lt_color_blue = progStatus.lighted_btn_blue;

	lblTest->labelcolor(fl_rgb_color(fg_red, fg_green, fg_blue));
	lblTest->color(bgclr);

	scaleSmeterColor->color(bgclr);
	scaleSmeterColor->labelcolor(fgclr);
	scalePWRcolor->color(bgclr);
	scalePWRcolor->labelcolor(fgclr);
	scaleSWRcolor->color(bgclr);
	scaleSWRcolor->labelcolor(fgclr);
	scaleVoltcolor->color(bgclr);
	scaleVoltcolor->labelcolor(fgclr);
	grpMeterColor->color(bgclr);
	grpMeterColor->labelcolor(fgclr);

	sldrRcvSignalColor->color(
		fl_rgb_color (smeterRed, smeterGreen, smeterBlue),
		bgclr );
	sldrPWRcolor->color(
		fl_rgb_color (pwrRed, pwrGreen, pwrBlue),
		bgclr );
	sldrSWRcolor->color(
		fl_rgb_color (swrRed, swrGreen, swrBlue),
		bgclr );
	sldrVoltcolor->color(
		fl_rgb_color (voltRed, voltGreen, voltBlue),
		bgclr );

	sldrRcvSignalColor->minimum(0);
	sldrRcvSignalColor->maximum(100);
	sldrRcvSignalColor->value(45);

	sldrPWRcolor->minimum(0);
	sldrPWRcolor->maximum(100);
	sldrPWRcolor->value(80);

	sldrSWRcolor->minimum(0);
	sldrSWRcolor->maximum(100);
	sldrSWRcolor->value(25);

	sldrVoltcolor->minimum(0);
	sldrVoltcolor->maximum(100);
	sldrVoltcolor->value(55);

	btn_lt_color = fl_rgb_color( btn_lt_color_red, btn_lt_color_green, btn_lt_color_blue);
	btn_slider = fl_rgb_color( btn_slider_red, btn_slider_green, btn_slider_blue);
	bg_slider = fl_rgb_color(bg_slider_red, bg_slider_green, bg_slider_blue);

	btn_lighted->value(1);
	btn_lighted->selection_color(btn_lt_color);

	sldrColors->color(bg_slider);
	sldrColors->selection_color(btn_slider);

	mnuScheme->value(mnuScheme->find_item(progStatus.ui_scheme.c_str()));

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
	Fl_Widget * vtab = (Fl_Widget *)0;
	if (tabsGeneric && !progStatus.visible_tab.empty()) {
		Fl_Widget * const *vtabs = tabsGeneric->array();
		int ntabs = tabsGeneric->children();
		for (int n = 0; n < ntabs; n++) {
			if (progStatus.visible_tab == vtabs[n]->label()) {
				vtab = vtabs[n];
			}
			vtabs[n]->redraw();
		}
	}

	switch (progStatus.UIsize) {
		case touch_ui : {
			if (selrig->name_ == rig_TT550.name_) {
				tabs550->show();
				tabsGeneric->hide();
				tabs550->redraw();
			} else {
				tabs550->hide();
				tabsGeneric->show();
				tabsGeneric->redraw();
			}
			mainwindow->redraw();
			break;
		}
		case wide_ui : {
			if (selrig->name_ == rig_TT550.name_) {
				tabsGeneric->hide();
				tabs550->show();
				tabs550->redraw();
			} else {
				tabs550->hide();
				tabsGeneric->show();
				tabsGeneric->redraw();
			}
			if (progStatus.embed_tabs) { // embedded
				int X = mainwindow->x(),
					Y = mainwindow->y(),
					W = mainwindow->w(),
					H = mainwindow->h();
				if ((progStatus.show_tabs && !progStatus.first_use) ||
					(!progStatus.show_tabs && progStatus.first_use)) {

					H = WIDE_MAINH + WIDE_MENUH;
					mainwindow->resize( X, Y, W, H );
					mainwindow->size_range(WIDE_MAINW, H, 0, H);

					tabs->hide();
					progStatus.show_tabs = false;
					mainwindow->redraw();
				} else { 
					H = WIDE_MENUH + WIDE_MAINH + WIDE_TABSH;

					mainwindow->resize( X, Y, W, H);
					mainwindow->size_range(WIDE_MAINW, H, 0, H);

					tabs->add(grpTABS);
					grpTABS->resize(
						tabs->x(), tabs->y(),
						tabs->w(), tabs->h());
					grpTABS->show();
					tabs->show();
					progStatus.show_tabs = true;

					mainwindow->redraw();
				}
			} else {
				if ((progStatus.show_tabs && !progStatus.first_use) || (!progStatus.show_tabs && progStatus.first_use)) {

					tabs_dialog->hide();

					progStatus.show_tabs = false;

				} else if ((!progStatus.show_tabs && !progStatus.first_use) || (progStatus.show_tabs && progStatus.first_use)) {

					static int X, Y, W, H, dH;
					X = mainwindow->x();
					Y = mainwindow->y();
					W = mainwindow->w();
					H = WIDE_MAINH + WIDE_MENUH;

					dH = mainwindow->decorated_h();
					tabs_dialog->resize( X, Y + dH, W, tabs_dialog->h() );
					grpTABS->resize(0, 0, W, tabs_dialog->h());

					tabs_dialog->add(grpTABS);
					tabs_dialog->show();
					tabs_dialog->redraw();
					progStatus.show_tabs = true;

					mainwindow->resize( X, Y, W, H);
					mainwindow->size_range(WIDE_MAINW, H, 0, H);

					mainwindow->redraw();
				}
			}
			break;
		}
		case small_ui : {
			if (selrig->name_ == rig_TT550.name_) {
				tabsGeneric->hide();
				tabs550->show();
				tabs550->redraw();
				break;
			} else {
				tabs550->hide();
				tabsGeneric->show();
				tabsGeneric->redraw();
			}
			if (progStatus.embed_tabs) { // embedded
				static int X = mainwindow->x(),
						   Y = mainwindow->y(),
						   W = mainwindow->w(),
						   H = mainwindow->h();
				if ((progStatus.show_tabs && !progStatus.first_use) || (!progStatus.show_tabs && progStatus.first_use)) {
					tabs_dialog->add(grpTABS);

					grpTABS->resize(tabs_dialog->x(), tabs_dialog->y(), tabs_dialog->w(), tabs_dialog->h());
					mainwindow->resize(mainwindow->x(), mainwindow->y(), W, H);
					progStatus.show_tabs = false;
				} else if (!progStatus.show_tabs && !progStatus.first_use) {
					X = mainwindow->x();
					Y = mainwindow->y();
					W = mainwindow->w();
					H = mainwindow->h();
					mainwindow->resize(X, Y, W, H + grpTABS->h());
					grpTABS->resize(0, H, W, grpTABS->h());
					mainwindow->add(grpTABS);

					grpTABS->show();
					progStatus.show_tabs = true;
				} else if (progStatus.show_tabs && progStatus.first_use) {

					X = mainwindow->x();
					Y = mainwindow->y();
					W = mainwindow->w();
					H = mainwindow->h();

					mainwindow->resize(X, Y, W, H + grpTABS->h());
					grpTABS->resize(0, H, W, grpTABS->h());
					mainwindow->add(grpTABS);

					grpTABS->show();
					grpTABS->redraw();
					mainwindow->redraw();
					progStatus.show_tabs = true;
				}
			} else {
				if ((progStatus.show_tabs && !progStatus.first_use) || (!progStatus.show_tabs && progStatus.first_use)) {
					tabs_dialog->hide();

					progStatus.show_tabs = false;
				} else if ((!progStatus.show_tabs && !progStatus.first_use) || (progStatus.show_tabs && progStatus.first_use)) {
					static int X, Y, W, dH;
					X = mainwindow->x(); //progStatus.mainX;
					Y = mainwindow->y(); //progStatus.mainY;
					W = mainwindow->w(); //progStatus.mainW;
					dH = mainwindow->decorated_h();

					tabs_dialog->resize( X, Y + dH, W, tabs_dialog->h() );
					grpTABS->resize(0, 0, W, tabs_dialog->h());
					if (vtab != (Fl_Widget *)0) tabsGeneric->value(vtab);
					tabs_dialog->add(grpTABS);
					tabs_dialog->show();
					tabs_dialog->redraw();
					progStatus.show_tabs = true;
				}
			}
			break;
		}
		default :
			break;
	}

	if (tabsGeneric) {
		if (vtab != (Fl_Widget *)0)
			tabsGeneric->value(vtab);
		else
			progStatus.visible_tab = (tabsGeneric->value())->label();
	}

	progStatus.first_use = false;
}

// a replica of the default color map used by Fltk

static unsigned flrig_cmap[256] = {
	0x00000000,
	0xff000000,
	0x00ff0000,
	0xffff0000,
	0x0000ff00,
	0xff00ff00,
	0x00ffff00,
	0xffffff00,
	0x55555500,
	0xc6717100,
	0x71c67100,
	0x8e8e3800,
	0x7171c600,
	0x8e388e00,
	0x388e8e00,
	0x00008000,
	0xa8a89800,
	0xe8e8d800,
	0x68685800,
	0x98a8a800,
	0xd8e8e800,
	0x58686800,
	0x9c9ca800,
	0xdcdce800,
	0x5c5c6800,
	0x9ca89c00,
	0xdce8dc00,
	0x5c685c00,
	0x90909000,
	0xc0c0c000,
	0x50505000,
	0xa0a0a000,
	0x00000000,
	0x0d0d0d00,
	0x1a1a1a00,
	0x26262600,
	0x31313100,
	0x3d3d3d00,
	0x48484800,
	0x55555500,
	0x5f5f5f00,
	0x6a6a6a00,
	0x75757500,
	0x80808000,
	0x8a8a8a00,
	0x95959500,
	0xa0a0a000,
	0xaaaaaa00,
	0xb5b5b500,
	0xc0c0c000,
	0xcbcbcb00,
	0xd5d5d500,
	0xe0e0e000,
	0xeaeaea00,
	0xf5f5f500,
	0xffffff00,
	0x00000000,
	0x00240000,
	0x00480000,
	0x006d0000,
	0x00910000,
	0x00b60000,
	0x00da0000,
	0x00ff0000,
	0x3f000000,
	0x3f240000,
	0x3f480000,
	0x3f6d0000,
	0x3f910000,
	0x3fb60000,
	0x3fda0000,
	0x3fff0000,
	0x7f000000,
	0x7f240000,
	0x7f480000,
	0x7f6d0000,
	0x7f910000,
	0x7fb60000,
	0x7fda0000,
	0x7fff0000,
	0xbf000000,
	0xbf240000,
	0xbf480000,
	0xbf6d0000,
	0xbf910000,
	0xbfb60000,
	0xbfda0000,
	0xbfff0000,
	0xff000000,
	0xff240000,
	0xff480000,
	0xff6d0000,
	0xff910000,
	0xffb60000,
	0xffda0000,
	0xffff0000,
	0x00003f00,
	0x00243f00,
	0x00483f00,
	0x006d3f00,
	0x00913f00,
	0x00b63f00,
	0x00da3f00,
	0x00ff3f00,
	0x3f003f00,
	0x3f243f00,
	0x3f483f00,
	0x3f6d3f00,
	0x3f913f00,
	0x3fb63f00,
	0x3fda3f00,
	0x3fff3f00,
	0x7f003f00,
	0x7f243f00,
	0x7f483f00,
	0x7f6d3f00,
	0x7f913f00,
	0x7fb63f00,
	0x7fda3f00,
	0x7fff3f00,
	0xbf003f00,
	0xbf243f00,
	0xbf483f00,
	0xbf6d3f00,
	0xbf913f00,
	0xbfb63f00,
	0xbfda3f00,
	0xbfff3f00,
	0xff003f00,
	0xff243f00,
	0xff483f00,
	0xff6d3f00,
	0xff913f00,
	0xffb63f00,
	0xffda3f00,
	0xffff3f00,
	0x00007f00,
	0x00247f00,
	0x00487f00,
	0x006d7f00,
	0x00917f00,
	0x00b67f00,
	0x00da7f00,
	0x00ff7f00,
	0x3f007f00,
	0x3f247f00,
	0x3f487f00,
	0x3f6d7f00,
	0x3f917f00,
	0x3fb67f00,
	0x3fda7f00,
	0x3fff7f00,
	0x7f007f00,
	0x7f247f00,
	0x7f487f00,
	0x7f6d7f00,
	0x7f917f00,
	0x7fb67f00,
	0x7fda7f00,
	0x7fff7f00,
	0xbf007f00,
	0xbf247f00,
	0xbf487f00,
	0xbf6d7f00,
	0xbf917f00,
	0xbfb67f00,
	0xbfda7f00,
	0xbfff7f00,
	0xff007f00,
	0xff247f00,
	0xff487f00,
	0xff6d7f00,
	0xff917f00,
	0xffb67f00,
	0xffda7f00,
	0xffff7f00,
	0x0000bf00,
	0x0024bf00,
	0x0048bf00,
	0x006dbf00,
	0x0091bf00,
	0x00b6bf00,
	0x00dabf00,
	0x00ffbf00,
	0x3f00bf00,
	0x3f24bf00,
	0x3f48bf00,
	0x3f6dbf00,
	0x3f91bf00,
	0x3fb6bf00,
	0x3fdabf00,
	0x3fffbf00,
	0x7f00bf00,
	0x7f24bf00,
	0x7f48bf00,
	0x7f6dbf00,
	0x7f91bf00,
	0x7fb6bf00,
	0x7fdabf00,
	0x7fffbf00,
	0xbf00bf00,
	0xbf24bf00,
	0xbf48bf00,
	0xbf6dbf00,
	0xbf91bf00,
	0xbfb6bf00,
	0xbfdabf00,
	0xbfffbf00,
	0xff00bf00,
	0xff24bf00,
	0xff48bf00,
	0xff6dbf00,
	0xff91bf00,
	0xffb6bf00,
	0xffdabf00,
	0xffffbf00,
	0x0000ff00,
	0x0024ff00,
	0x0048ff00,
	0x006dff00,
	0x0091ff00,
	0x00b6ff00,
	0x00daff00,
	0x00ffff00,
	0x3f00ff00,
	0x3f24ff00,
	0x3f48ff00,
	0x3f6dff00,
	0x3f91ff00,
	0x3fb6ff00,
	0x3fdaff00,
	0x3fffff00,
	0x7f00ff00,
	0x7f24ff00,
	0x7f48ff00,
	0x7f6dff00,
	0x7f91ff00,
	0x7fb6ff00,
	0x7fdaff00,
	0x7fffff00,
	0xbf00ff00,
	0xbf24ff00,
	0xbf48ff00,
	0xbf6dff00,
	0xbf91ff00,
	0xbfb6ff00,
	0xbfdaff00,
	0xbfffff00,
	0xff00ff00,
	0xff24ff00,
	0xff48ff00,
	0xff6dff00,
	0xff91ff00,
	0xffb6ff00,
	0xffdaff00,
	0xffffff00
};

Fl_Color flrig_def_color(int n)
{
	if ( n > 255 ) n = 255;
	if (n < 0) n = 0;
	return (Fl_Color)flrig_cmap[n];
}


void cb_send_command(std::string command, Fl_Output *resp)
{
	if (command.empty()) return;
	bool usehex = false;
	if (command.empty()) return;
	std::string cmd = "";
	if (command.find("x") != std::string::npos) { // hex std::strings
		size_t p = 0;
		usehex = true;
		unsigned int val;
		while (( p = command.find("x", p)) != std::string::npos) {
			sscanf(&command[p+1], "%x", &val);
			cmd += (unsigned char) val;
			p += 3;
		}
	} else
		cmd = command;

	if (resp) {
		resp->value("");
		resp->redraw();
	}

// lock out polling loops until done
	guard_lock lock1(&mutex_srvc_reqs);
	guard_lock lock2(&mutex_serial);

	sendCommand(cmd, 0);//cmd.length());
	set_trace(2, "command: ", command.c_str());
	waitResponse(100);

	std::string retstr = usehex ? 
		str2hex(respstr.c_str(), respstr.length()) :
		respstr;
	set_trace(2, "response: ", retstr.c_str());

	if (resp) {
		resp->value(retstr.c_str());
		resp->redraw();
	}
}

// =====================================================================
// logbook support code
// =====================================================================
bool cwlog_editing = false;
int  cwlog_edit_nbr = 0;

bool cwlog_changed = false;
bool cwlog_is_open = false;

void cwlog_set_edit(bool on)
{
	cwlog_editing = on;
	if (on) {
		btn_cwlog_edit_entry->label("Delete");
		btn_cwlog_edit_entry->redraw_label();
		btn_cwlog_clear_qso->label("Cancel");
		btn_cwlog_clear_qso->redraw_label();
	} else {
		btn_cwlog_edit_entry->label("Edit");
		btn_cwlog_edit_entry->redraw_label();
		btn_cwlog_clear_qso->label("Clear");
		btn_cwlog_clear_qso->redraw_label();
	}
}

bool cwlog_compare( int &dir, int fld, std::string &s1, std::string &s2) {
	size_t p1 = 0, p2 = 0;
	for (int n = 0; n < fld; n++) {
		p1 = s1.find('\t', p1 + 1);
		p2 = s2.find('\t', p2 + 1);
	}
	if (dir == 1) return (s2.substr(p2) < s1.substr(p1));
	return (s2.substr(p2) > s1.substr(p1));
}

bool cwlog_freq_compare( int &dir, int fld, std::string &s1, std::string &s2) {
	size_t p1 = 0, p2 = 0;
	for (int n = 0; n < fld; n++) {
		p1 = s1.find('\t', p1 + 1);
		p2 = s2.find('\t', p2 + 1);
	}
	float f1 = atof(s1.substr(p1).c_str());
	float f2 = atof(s2.substr(p2).c_str());

	if (dir == 1) return (f2 < f1);
	return (f2 > f1);
}

static int dtdir = 1;
static int dtpos = 0;
void cwlog_sort_by_datetime() {
	if (cwlog_editing) return;
	size_t nbr = brwsr_cwlog_entries->size();
	if (nbr == 0) return;
	std::string entries[nbr];
	for (size_t n = 0; n < nbr; n++) entries[n] = brwsr_cwlog_entries->text(n+1);
	std::string temp;
	if (nbr > 1) {
		for (size_t n = 0; n < nbr - 1; n++) {
			for (size_t j = n + 1; j < nbr; j++) {
				if (cwlog_compare (dtdir, dtpos, entries[n], entries[j])) {
					temp = entries[j];
					entries[j] = entries[n];
					entries[n] = temp;
				}
			}
		}
	}
	brwsr_cwlog_entries->clear();
	for (size_t i = 0; i < nbr; i++)
		brwsr_cwlog_entries->add(entries[i].c_str());
	brwsr_cwlog_entries->redraw();
	if (dtdir == 1) dtdir = -1;
	else dtdir = 1;
}

static int freqdir = 1;
static int freqpos = 2;
void cwlog_sort_by_freq() {
	if (cwlog_editing) return;
	size_t nbr = brwsr_cwlog_entries->size();
	if (nbr == 0) return;
	std::string entries[nbr];
	for (size_t n = 0; n < nbr; n++) entries[n] = brwsr_cwlog_entries->text(n+1);
	std::string temp;
	if (nbr > 1) {
		for (size_t n = 0; n < nbr - 1; n++) {
			for (size_t j = n + 1; j < nbr; j++) {
				if (cwlog_freq_compare (freqdir, freqpos, entries[n], entries[j])) {
					temp = entries[j];
					entries[j] = entries[n];
					entries[n] = temp;
				}
			}
		}
	}
	brwsr_cwlog_entries->clear();
	for (size_t i = 0; i < nbr; i++)
		brwsr_cwlog_entries->add(entries[i].c_str());
	brwsr_cwlog_entries->redraw();
	if (freqdir == 1) freqdir = -1;
	else freqdir = 1;
}

static int calldir = 1;
static int callpos = 3;
void cwlog_sort_by_call() {
	if (cwlog_editing) return;
	size_t nbr = brwsr_cwlog_entries->size();
	if (nbr == 0) return;
	std::string entries[nbr];
	for (size_t n = 0; n < nbr; n++) entries[n] = brwsr_cwlog_entries->text(n+1);
	std::string temp;
	if (nbr > 1) {
		for (size_t n = 0; n < nbr - 1; n++) {
			for (size_t j = n + 1; j < nbr; j++) {
				if (cwlog_compare (calldir, callpos, entries[n], entries[j])) {
					temp = entries[j];
					entries[j] = entries[n];
					entries[n] = temp;
				}
			}
		}
	}
	brwsr_cwlog_entries->clear();
	for (size_t i = 0; i < nbr; i++)
		brwsr_cwlog_entries->add(entries[i].c_str());
	brwsr_cwlog_entries->redraw();
	if (calldir == 1) calldir = -1;
	else calldir = 1;
}

static int nbrdir = 1;
static int nbrpos = 7;
void cwlog_sort_by_nbr() {
	if (cwlog_editing) return;
	size_t nbr = brwsr_cwlog_entries->size();
	if (nbr == 0) return;
	std::string entries[nbr];
	for (size_t n = 0; n < nbr; n++) entries[n] = brwsr_cwlog_entries->text(n+1);
	std::string temp;
	if (nbr > 1) {
		for (size_t n = 0; n < nbr - 2; n++) {
			for (size_t j = n + 1; j < nbr - 1; j++) {
				if (cwlog_compare (nbrdir, nbrpos, entries[n], entries[j])) {
					temp = entries[j];
					entries[j] = entries[n];
					entries[n] = temp;
				}
			}
		}
	}
	brwsr_cwlog_entries->clear();
	for (size_t i = 0; i < nbr; i++)
		brwsr_cwlog_entries->add(entries[i].c_str());
	brwsr_cwlog_entries->redraw();
	if (nbrdir == 1) nbrdir = -1;
	else nbrdir = 1;
}

void cwlog_clear_qso()
{
	cw_qso_date->value("");
	cw_qso_time->value("");
	cw_freq->value("");
	cw_op_call->value("");
	cw_op_name->value("");
	cw_rst_in->value("");
	cw_rst_out->value("");
	cw_xchg_in->value("");

	if (cwlog_editing)
		cwlog_set_edit(false);

}

void cwlog_save_qso()
{
	char line[256];
	if (cwlog_editing) {
		snprintf(line, sizeof(line),
			"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%05d\t%s",
			cw_qso_date->value(),
			cw_qso_time->value(),
			cw_freq->value(),
			cw_op_call->value(),
			cw_op_name->value(),
			cw_rst_in->value(),
			cw_rst_out->value(),
			(int)cw_log_nbr->value(),
			cw_xchg_in->value());
		brwsr_cwlog_entries->insert(cwlog_edit_nbr, line);
		brwsr_cwlog_entries->remove(cwlog_edit_nbr + 1);
		brwsr_cwlog_entries->select(cwlog_edit_nbr);
		cwlog_set_edit(false);
	} else {
		snprintf(line, sizeof(line),
			"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%05d\t%s",
			cw_qso_date->value(),
			cw_qso_time->value(),
			cw_freq->value(),
			cw_op_call->value(),
			cw_op_name->value(),
			cw_rst_in->value(),
			cw_rst_out->value(),
			(int)cw_log_nbr->value(),
			cw_xchg_in->value());
		brwsr_cwlog_entries->add(line);
	}
	cwlog_changed = true;
}

void cwlog_delete_entry()
{
	brwsr_cwlog_entries->remove(cwlog_edit_nbr);
	brwsr_cwlog_entries->select(cwlog_edit_nbr, false);
	brwsr_cwlog_entries->redraw();
	cwlog_clear_qso();
	cwlog_changed = true;
}

void cwlog_edit_entry()
{
	if (cwlog_editing) {
		cwlog_delete_entry();
		return;
	}

	cwlog_edit_nbr = brwsr_cwlog_entries->value();
	if (!cwlog_edit_nbr) return;

	cwlog_clear_qso();
	size_t ptr = 0;
	std::string entry = brwsr_cwlog_entries->text(cwlog_edit_nbr);

	ptr = entry.find('\t');
	cw_qso_date->value(entry.substr(0, ptr).c_str());
	entry.erase(0, ptr+1);

	ptr = entry.find('\t');
	cw_qso_time->value(entry.substr(0,ptr).c_str());
	entry.erase(0, ptr+1);

	ptr = entry.find('\t');
	cw_freq->value(entry.substr(0,ptr).c_str());
	entry.erase(0, ptr+1);

	ptr = entry.find('\t');
	cw_op_call->value(entry.substr(0,ptr).c_str());
	entry.erase(0, ptr+1);

	ptr = entry.find('\t');
	cw_op_name->value(entry.substr(0,ptr).c_str());
	entry.erase(0, ptr+1);

	ptr = entry.find('\t');
	cw_rst_in->value(entry.substr(0,ptr).c_str());
	entry.erase(0, ptr+1);

	ptr = entry.find('\t');
	cw_rst_out->value(entry.substr(0,ptr).c_str());
	entry.erase(0, ptr+1);

	ptr = entry.find('\t');
	cw_log_nbr->value(atoi(entry.substr(0,ptr).c_str()));
	entry.erase(0, ptr+1);

	ptr = entry.find('\t');
	cw_xchg_in->value(entry.substr(0,ptr).c_str());
	entry.erase(0, ptr+1);

	cwlog_set_edit(true);
}

void cwlog_view()
{
	if (!cwlog_viewer) { 
		cwlog_viewer = new_cwlogbook_dialog();
		if (!progStatus.cw_log_name.empty()) {
			txt_cwlog_file->value(progStatus.cw_log_name.c_str());
			cwlog_load();
		} else
			cwlog_open();
	}
	cwlog_viewer->show();
}

void cwlog_save()
{
	if (progStatus.cw_log_name.empty())
		return;
	std::ofstream oLog(progStatus.cw_log_name.c_str());
	if (!oLog) {
		fl_message ("Could not write to %s", progStatus.cw_log_name.c_str());
		return;
	}
	size_t n = brwsr_cwlog_entries->size();
	std::string oline;
	for (size_t i = 1; i <= n; i++) {
		oline = brwsr_cwlog_entries->text(i);
		if (oline.empty()) continue;
		oLog << oline << std::endl;
	}
	oLog.close();
	cwlog_changed = false;
}

void cwlog_load()
{
	std::ifstream iLog(progStatus.cw_log_name.c_str());
	if (!iLog) return;
	brwsr_cwlog_entries->clear();
	char line[256];
	std::string sline;
	while (!iLog.eof()) {
		memset(line, 0, 256);
		iLog.getline(line, 256);
		sline = line;
		if (!sline.empty())
			brwsr_cwlog_entries->add(sline.c_str());
	}
	iLog.close();
	brwsr_cwlog_entries->redraw();
	cwlog_is_open = true;
}

void cwlog_save_as()
{
// Create and post the local native file chooser
	Fl_Native_File_Chooser fnfc;
	fnfc.title("Save As log file");
	fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	fnfc.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM);
	fnfc.filter("CW Log\t*.txt");
// default directory to use
	fnfc.directory(RigHomeDir.c_str());
	fnfc.preset_file(progStatus.cw_log_name.c_str());
// Show native chooser
	switch ( fnfc.show() ) {
		case -1:
			fl_message ("ERROR: %s", fnfc.errmsg());
			return; // ERROR
		case 1: 
			return; // CANCEL
		default:
			progStatus.cw_log_name = fnfc.filename();
			txt_cwlog_file->value(progStatus.cw_log_name.c_str());
	}
	cwlog_save();
}

void cwlog_open()
{
	if (cwlog_is_open && cwlog_changed)
		cwlog_save();

// Create and post the local native file chooser
	Fl_Native_File_Chooser fnfc;
	fnfc.title("Select log file");
	fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
	fnfc.filter("CW Log\t*.txt");
// default directory to use
	fnfc.directory(RigHomeDir.c_str());
// Show native chooser
	switch ( fnfc.show() ) {
		case -1:
			fl_message ("ERROR: %s", fnfc.errmsg());
			return; // ERROR
		case 1: 
			return; // CANCEL
		default:
			progStatus.cw_log_name = fnfc.filename();
			txt_cwlog_file->value(progStatus.cw_log_name.c_str());
			txt_cwlog_file->redraw();
			cwlog_load();
	}
}

void cwlog_new()
{
	if (cwlog_is_open && cwlog_changed)
		cwlog_save();
	brwsr_cwlog_entries->clear();
	brwsr_cwlog_entries->redraw();
	progStatus.cw_log_name.clear();
	txt_cwlog_file->value(progStatus.cw_log_name.c_str());
	txt_cwlog_file->redraw();

	Fl_Native_File_Chooser fnfc;
	fnfc.title("Create new log file");
	fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	fnfc.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM);
	fnfc.filter("CW Log\t*.txt");
// default directory to use
	fnfc.directory(RigHomeDir.c_str());
	fnfc.preset_file("cwlog.txt");
// Show native chooser
	switch ( fnfc.show() ) {
		case -1:
			fl_message ("ERROR: %s", fnfc.errmsg());
			return; // ERROR
		case 1: 
			return; // CANCEL
		default:
			progStatus.cw_log_name = fnfc.filename();
			txt_cwlog_file->value(progStatus.cw_log_name.c_str());
	}
}

void cwlog_close()
{
	if (cwlog_is_open && cwlog_changed)
		cwlog_save();
}

void cwlog_export_adif()
{
// Create and post the local native file chooser
	Fl_Native_File_Chooser fnfc;
	fnfc.title("Export to ADIF file");
	fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	fnfc.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM);
	fnfc.filter("ADIF Log\t*.{adi,adif}");
// default directory to use
	fnfc.directory(RigHomeDir.c_str());
// Show native chooser
	switch ( fnfc.show() ) {
		case -1:
			fl_message ("ERROR: %s", fnfc.errmsg());
			return; // ERROR
		case 1: 
			return; // CANCEL
		default:
			break;
	}

	std::string export_fname = fnfc.filename();
	std::ofstream oExport(export_fname.c_str());
	if (!oExport) {
		fl_message ("Could not write to %s", export_fname.c_str());
		return;
	}

	std::string logline,
				cw_qso_date, cw_qso_time,
				cw_freq,
				cw_op_call,
				cw_op_name,
				cw_rst_in, cw_rst_out,
				cw_log_nbr,
				qso_notes;

	size_t ptr = std::string::npos;
	size_t n = brwsr_cwlog_entries->size();
	for (size_t i = 1; i <= n; i++) {
		logline = brwsr_cwlog_entries->text(i);
		if (logline.empty()) continue;

		ptr = logline.find('\t');
		cw_qso_date = logline.substr(0, ptr);
		logline.erase(0, ptr+1);

		ptr = logline.find('\t');
		cw_qso_time = logline.substr(0, ptr);
		logline.erase(0, ptr+1);

		ptr = logline.find('\t');
		cw_freq = logline.substr(0, ptr);
		logline.erase(0, ptr+1);

		ptr = logline.find('\t');
		cw_op_call = logline.substr(0, ptr);
		logline.erase(0, ptr+1);

		ptr = logline.find('\t');
		cw_op_name = logline.substr(0, ptr);
		logline.erase(0, ptr+1);

		ptr = logline.find('\t');
		cw_rst_in = logline.substr(0, ptr);
		logline.erase(0, ptr+1);

		ptr = logline.find('\t');
		cw_rst_out = logline.substr(0, ptr);
		logline.erase(0, ptr+1);

		ptr = logline.find('\t');
		cw_log_nbr = logline.substr(0, ptr);
		logline.erase(0, ptr+1);

		qso_notes = logline;

		oExport << "<QSO_DATE:" << cw_qso_date.length() << ">" << cw_qso_date
				<< "<TIME_ON:" << cw_qso_time.length() << ">" << cw_qso_time
				<< "<FREQ:" << cw_freq.length() << ">" << cw_freq
				<< "<MODE:2>CW"
				<< "<CALL:" << cw_op_call.length() << ">" << cw_op_call
				<< "<NAME:" << cw_op_name.length() << ">" << cw_op_name
				<< "<RST_RCVD:" << cw_rst_in.length() << ">" << cw_rst_in
				<< "<RST_SENT:" << cw_rst_out.length() << ">" << cw_rst_out
				<< "<STX:" << cw_log_nbr.length() << ">" << cw_log_nbr
				<< "<NOTES:" << qso_notes.length() << ">" << qso_notes
				<< "<EOR>" << std::endl;
	}
	oExport.close();
}

std::string cwlog_adif_extract( std::string FIELD, std::string line)
{
	size_t p1, p2;
	p1 = line.find(FIELD);
	if (p1 != std::string::npos)  {
		p1 = line.find(">", p1);
		if (p1 != std::string::npos) {
			p1++;
			p2 = line.find("<", p1);
			if (p2 != std::string::npos)
				return line.substr(p1, p2 - p1);
		}
	}
	return "";
}

void cwlog_import_adif()
{
	std::string import_fname;
	Fl_Native_File_Chooser fnfc;
	fnfc.title("Import from ADIF file");
	fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
	fnfc.filter("ADIF Log\t*.{adi,adif}");
// default directory to use
	fnfc.directory(RigHomeDir.c_str());
// Show native chooser
	switch ( fnfc.show() ) {
		case -1:
			fl_message ("ERROR: %s", fnfc.errmsg());
			return; // ERROR
		case 1: 
			return; // CANCEL
		default:
			break;
	}
	import_fname = fnfc.filename();
	std::ifstream iImport(import_fname.c_str());
	if (!iImport) return;

// current log
	size_t n = brwsr_cwlog_entries->size();
	size_t p;
	std::string fulllog;
	std::string teststr;
	for (size_t i = 1; i <= n; i++) {
		fulllog.append(brwsr_cwlog_entries->text(i)).append("\n");
	}

	char buff[512];
	std::string line, ldate, ltime, lfreq, lcall, lname, lrst_in, lrst_out, lnbr, lnotes, lbrwsr;
	while (!iImport.eof()) {
		iImport.getline(buff, 512);
		line = buff;
		if (cwlog_adif_extract("MODE", line) == "CW") {
			ldate = cwlog_adif_extract("QSO_DATE", line);
			ltime = cwlog_adif_extract("TIME_ON", line).substr(0,4);
			lfreq = cwlog_adif_extract("FREQ", line);
			lcall = cwlog_adif_extract("CALL", line);
			lname = cwlog_adif_extract("NAME", line);
			lrst_in = cwlog_adif_extract("RST_RCVD", line);
			lrst_out = cwlog_adif_extract("RST_SENT", line);
			lnbr = cwlog_adif_extract("STX", line);
			lnotes = cwlog_adif_extract("NOTES", line);
			lbrwsr.assign(ldate).append("\t");
			lbrwsr.append(ltime).append("\t");
			lbrwsr.append(lfreq).append("\t");
			lbrwsr.append(lcall).append("\t");
			teststr = lbrwsr;
			lbrwsr.append(lname).append("\t");
			lbrwsr.append(lrst_in).append("\t");
			lbrwsr.append(lrst_out).append("\t");
			lbrwsr.append(lnbr).append("\t");
			lbrwsr.append(lnotes);
			p = lbrwsr.find("\n");
			if (p != std::string::npos)
				lbrwsr.erase(p);
			if (fulllog.find(teststr) == std::string::npos &&
				!ldate.empty() &&
				!ltime.empty()) {
				fulllog.append(lbrwsr).append("\n");
				brwsr_cwlog_entries->add(lbrwsr.c_str());
			}
		}
	}
	cwlog_changed = true;
	iImport.close();
}

