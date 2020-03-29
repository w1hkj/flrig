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
#include <string>
#include <iostream>
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

using namespace std;

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
}

void add_combos(char *port)
{
	selectCommPort->add(port);
	selectAuxPort->add(port);
	selectSepPTTPort->add(port);
}

void set_combo_value()
{
	selectCommPort->value(progStatus.xcvr_serial_port.c_str());
	selectAuxPort->value(progStatus.aux_serial_port.c_str());
	selectSepPTTPort->value(progStatus.sep_serial_port.c_str());
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
std::cout << "Serial port: " << gbuf.gl_pathv[j] << std::endl;
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
			LOG_WARN("Found serial port %s", gbuf.gl_pathv[j]);
			add_combos(gbuf.gl_pathv[j]);
		}
		globfree(&gbuf);
	}
	set_combo_value();
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
			LOG_WARN("Found serial port %s", ttyname);
			add_combos(ttyname);
		}
	}
}
#endif //__FreeBSD__

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
	select_tab(_("Cmds"));
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
	select_tab(_("TCPIP"));
}

void open_ptt_tab()
{
	select_tab(_("PTT"));
}

void open_aux_tab()
{
	select_tab(_("Aux"));
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
	fntbrowser->fontFilter(Font_Browser::FIXED_WIDTH);
	fntbrowser->fontFilter(Font_Browser::ALL_TYPES);
	fntbrowser->callback(cbFreqControlFontBrowser);
	fntbrowser->show();
}

uchar fg_red, fg_green, fg_blue;
uchar bg_red, bg_green, bg_blue;
uchar smeterRed, smeterGreen, smeterBlue;
uchar peakRed, peakGreen, peakBlue;
uchar pwrRed, pwrGreen, pwrBlue;
uchar swrRed, swrGreen, swrBlue;

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
		scaleSmeterColor->color(bgclr);
		scalePWRcolor->color(bgclr);
		scaleSWRcolor->color(bgclr);
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
		grpMeterColor->color(bgclr);
	fg_red = 0; fg_green = 0; fg_blue = 0;
	fgclr = (Fl_Color)0;
		lblTest->labelcolor(fgclr);
		scaleSmeterColor->labelcolor(fgclr);
		scalePWRcolor->labelcolor(fgclr);
		scaleSWRcolor->labelcolor(fgclr);
		grpMeterColor->labelcolor(fgclr);
	smeterRed = 0; smeterGreen = 180; smeterBlue = 0;
		c = fl_rgb_color (smeterRed, smeterGreen, smeterBlue);
		sldrRcvSignalColor->color(c, bgclr );
	peakRed = 255; peakGreen = 0; peakBlue = 0;
		c = fl_rgb_color( peakRed, peakGreen, peakBlue );
		sldrRcvSignalColor->PeakColor(c);
		sldrPWRcolor->PeakColor(c);
		sldrSWRcolor->PeakColor(c);
	pwrRed = 180; pwrGreen = 0; pwrBlue = 0;
		c = fl_rgb_color( pwrRed, pwrGreen, pwrBlue );
		sldrPWRcolor->color(c, bgclr);
	swrRed = 148; swrGreen = 0; swrBlue = 148;
		c = fl_rgb_color(swrRed, swrGreen, swrBlue);
		sldrSWRcolor->color(c, bgclr);
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
	if (fl_color_chooser("Power color", r, g, b)) {
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
	if (fl_color_chooser("Power color", r, g, b)) {
		swrRed = r; swrGreen = g; swrBlue = b;
		sldrSWRcolor->color(
			fl_rgb_color (swrRed, swrGreen, swrBlue),
			bgclr );
		dlgDisplayConfig->redraw();
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

	if (useB) {
		FreqDispB->SetONOFFCOLOR( fl_rgb_color(fg_red, fg_green, fg_blue), bgclr);
		FreqDispA->SetONOFFCOLOR(
			fl_rgb_color(fg_red, fg_green, fg_blue),
			fl_color_average(bgclr, FL_BLACK, 0.87));
	} else {
		FreqDispA->SetONOFFCOLOR( fl_rgb_color(fg_red, fg_green, fg_blue), bgclr);
		FreqDispB->SetONOFFCOLOR(
			fl_rgb_color(fg_red, fg_green, fg_blue),
			fl_color_average(bgclr, FL_BLACK, 0.87));
	}

	grpMeters->color(bgclr);

	meter_fill_box->color(bgclr);

	scaleSmeter->color(bgclr);
	scaleSmeter->labelcolor(fgclr);

	scalePower->color(bgclr);
	scalePower->labelcolor(fgclr);

	btnALC_SWR->color(bgclr);
	btnALC_SWR->labelcolor(fgclr);
	btnALC_SWR->redraw();

	sldrFwdPwr->color(fl_rgb_color (pwrRed, pwrGreen, pwrBlue), bgclr);
	sldrFwdPwr->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	sldrRcvSignal->color(fl_rgb_color (smeterRed, smeterGreen, smeterBlue), bgclr);
	sldrRcvSignal->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	sldrALC->color(fl_rgb_color (swrRed, swrGreen, swrBlue), bgclr);
	sldrALC->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

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
	if (btnPTT)				btnPTT->selection_color(btn_lt_color);
	if (btnLOCK)			btnLOCK->selection_color(btn_lt_color);
	if (btnAuxRTS)			btnAuxRTS->selection_color(btn_lt_color);
	if (btnAuxDTR)			btnAuxDTR->selection_color(btn_lt_color);
	if (btnSpot)			btnSpot->selection_color(btn_lt_color);
	if (btn_vox)			btn_vox->selection_color(btn_lt_color);
	if (btnCompON)			btnCompON->selection_color(btn_lt_color);
//	if (btnSpecial)			btnSpecial->selection_color(btn_lt_color);
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
//	if (spnrPOWER)			spnrPOWER->selection_color(btn_slider);

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

	sldrRcvSignalColor->minimum(0);
	sldrRcvSignalColor->maximum(100);
	sldrRcvSignalColor->value(25);

	sldrPWRcolor->minimum(0);
	sldrPWRcolor->maximum(100);
	sldrPWRcolor->value(25);

	sldrSWRcolor->minimum(0);
	sldrSWRcolor->maximum(100);
	sldrSWRcolor->value(25);

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
	int wh = mainwindow->h();
	int ww = mainwindow->w();
	switch (progStatus.UIsize) {
		case wide_ui :
			if (mainwindow->h() > WIDE_MAINH) {
				btn_show_controls->label("@-22->");
				btn_show_controls->redraw_label();
				grpTABS->hide();
				mainwindow->size(mainwindow->w(), WIDE_MAINH);
				mainwindow->size_range(735, WIDE_MAINH, 0, WIDE_MAINH);
			} else {
				wh += grpTABS->h();
				btn_show_controls->label("@-28->");
				btn_show_controls->redraw_label();
				grpTABS->show();
				grpTABS->resize(0, wh - grpTABS->h(), ww, grpTABS->h());
				mainwindow->size(ww, wh);
				mainwindow->size_range(735, wh, 0, wh);
			}
			mainwindow->redraw();
			break;
		case touch_ui : 
			if (selrig->name_ == rig_TT550.name_) {
				tabs550->show();
				tabsGeneric->hide();
			} else {
				tabs550->hide();
				tabsGeneric->show();
			}
			tabs550->redraw();
			tabsGeneric->redraw();
			mainwindow->redraw();
			break;
		case small_ui :
			if (selrig->name_ == rig_TT550.name_) {
				tabsGeneric->hide();
				tabs550->show();
			} else {
				tabs550->hide();
				tabsGeneric->show();
			}
			if (tabs_dialog->visible())
				tabs_dialog->hide();
			else {
				tabs_dialog->position(mainwindow->x(), mainwindow->y() + mainwindow->h() + 26);
				tabs_dialog->show();
			}
			break;
		default :
			break;
	}
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


void cb_send_command(string command, Fl_Output *resp)
{
	if (command.empty()) return;
	bool usehex = false;
	if (command.empty()) return;
	string cmd = "";
	if (command.find("x") != string::npos) { // hex strings
		size_t p = 0;
		usehex = true;
		unsigned int val;
		while (( p = command.find("x", p)) != string::npos) {
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
