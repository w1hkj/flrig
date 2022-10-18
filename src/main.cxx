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

#include "config.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <ctime>
#include <exception>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include <FL/Fl.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Sys_Menu_Bar.H>
#include <FL/x.H>
#include <FL/Fl_Help_Dialog.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

#ifdef WIN32
#  include "flrigrc.h"
#  include "compat.h"
#  define dirent fl_dirent_no_thanks
#endif

#include <FL/filename.H>

#ifdef __MINGW32__
#	if FLRIG_FLTK_API_MAJOR == 1 && FLRIG_FLTK_API_MINOR < 3
#		undef dirent
#		include <dirent.h>
#	endif
#else
#	include <dirent.h>
#endif

#include <FL/x.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Image.H>

//#include "images/flrig.xpm"
#include "support.h"
#include "dialogs.h"
#include "rig.h"
#include "status.h"
#include "debug.h"
#include "util.h"
#include "gettext.h"
#include "xml_server.h"
//#include "xml_io.h"
#include "serial.h"
#include "ui.h"
#include "icons.h"
#include "cwio.h"
#include "cwioUI.h"
#include "fsk.h"
#include "fskioUI.h"
#include "serial.h"

#include "flrig_icon.cxx"

int parse_args(int argc, char **argv, int& idx);

Fl_Double_Window *mainwindow = (Fl_Double_Window *)0;
Fl_Double_Window *tabs_dialog = (Fl_Double_Window *)0;
Fl_Double_Window *cwio_keyer_dialog = (Fl_Double_Window *)0;
Fl_Double_Window *cwio_editor = (Fl_Double_Window *)0;
Fl_Double_Window *cwio_configure = (Fl_Double_Window *)0;
Fl_Double_Window *FSK_keyer_dialog = (Fl_Double_Window *)0;
Fl_Double_Window *FSK_editor = (Fl_Double_Window *)0;
Fl_Double_Window *FSK_configure = (Fl_Double_Window *)0;

Fl_Double_Window *meter_scale_dialog = (Fl_Double_Window *)0;
Fl_Double_Window *meter_filters = (Fl_Double_Window *)0;
Fl_Double_Window *meters_dialog = (Fl_Double_Window *)0;

std::string HomeDir;
std::string RigHomeDir;
std::string TempDir;
std::string defFileName;
std::string title;

pthread_t *serial_thread = 0;
pthread_t *digi_thread = 0;

pthread_mutex_t mutex_serial = PTHREAD_MUTEX_INITIALIZER;

//pthread_mutex_t mutex_xmlrpc = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_vfoque = PTHREAD_MUTEX_INITIALIZER;

//pthread_mutex_t mutex_vfoque = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t mutex_vfoque = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_ptt = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_replystr = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_srvc_reqs = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_trace = PTHREAD_MUTEX_INITIALIZER;

int use_trace = 0;

bool EXPAND_CONTROLS = false;

int xmlport = 12345;

bool testmode = false;

//----------------------------------------------------------------------
void about()
{
	std::string msg = "\
%s\n\
Version %s\n\
copyright W1HKJ  <w1hkj@@w1hkj.com>\n\
Developer:  Dave,  W1HKJ";
	fl_message(msg.c_str(), PACKAGE_TARNAME, PACKAGE_VERSION);
}

void visit_URL(void* arg)
{
	const char* url = reinterpret_cast<const char *>(arg);
#ifndef __WOE32__
	const char* browsers[] = {
#  ifdef __APPLE__
		getenv("FLDIGI_BROWSER"), // valid for any OS - set by user
		"open"                    // OS X
#  else
		"fl-xdg-open",            // Puppy Linux
		"xdg-open",               // other Unix-Linux distros
		getenv("FLDIGI_BROWSER"), // force use of spec'd browser
		getenv("BROWSER"),        // most Linux distributions
		"sensible-browser",
		"firefox",
		"mozilla"                 // must be something out there!
#  endif
	};
	switch (fork()) {
	case 0:
#  ifndef NDEBUG
		unsetenv("MALLOC_CHECK_");
		unsetenv("MALLOC_PERTURB_");
#  endif
		for (size_t i = 0; i < sizeof(browsers)/sizeof(browsers[0]); i++)
			if (browsers[i])
				execlp(browsers[i], browsers[i], url, (char*)0);
		exit(EXIT_FAILURE);
	case -1:
		fl_alert(_("Could not run a web browser:\n%s\n\n"
			 "Open this URL manually:\n%s"),
			 strerror(errno), url);
	}
#else
	// gurgle... gurgle... HOWL
	// "The return value is cast as an HINSTANCE for backward
	// compatibility with 16-bit Windows applications. It is
	// not a true HINSTANCE, however. The only thing that can
	// be done with the returned HINSTANCE is to cast it to an
	// int and compare it with the value 32 or one of the error
	// codes below." (Error codes omitted to preserve sanity).
	if ((size_t)ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL) <= 32)
		fl_alert(_("Could not open url:\n%s\n"), url);
#endif
}

//----------------------------------------------------------------------

extern void saveFreqList();

void showEvents(void *)
{
	debug::show();
}

#if defined(__WIN32__) && defined(PTW32_STATIC_LIB)
static void ptw32_cleanup(void)
{
	(void)pthread_win32_process_detach_np();
}

void ptw32_init(void)
{
	(void)pthread_win32_process_attach_np();
	atexit(ptw32_cleanup);
}
#endif // __WIN32__

#define KNAME "Flrig"
#if !defined(__WIN32__) && !defined(__APPLE__)
Pixmap  Rig_icon_pixmap;

void make_pixmap(Pixmap *xpm, const char **data)
{
	Fl_Window w(0,0, KNAME);
	w.xclass(KNAME);
	w.show();
	w.make_current();
	Fl_Pixmap icon(data);
	int maxd = (icon.w() > icon.h()) ? icon.w() : icon.h();
	*xpm = fl_create_offscreen(maxd, maxd);
	fl_begin_offscreen(*xpm);
	fl_color(FL_BACKGROUND_COLOR);
	fl_rectf(0, 0, maxd, maxd);
	icon.draw(maxd - icon.w(), maxd - icon.h());
	fl_end_offscreen();
}

#endif

static void checkdirectories(void)
{
	struct {
		std::string& dir;
		const char* suffix;
		void (*new_dir_func)(void);
	} dirs[] = {
		{ RigHomeDir, 0, 0 }
	};

	int r;
	for (size_t i = 0; i < sizeof(dirs)/sizeof(*dirs); i++) {
		if (dirs[i].suffix)
			dirs[i].dir.assign(RigHomeDir).append(dirs[i].suffix).append("/");

		if ((r = mkdir(dirs[i].dir.c_str(), 0777)) == -1 && errno != EEXIST) {
			std::cerr << _("Could not make directory") << ' ' << dirs[i].dir
				 << ": " << strerror(errno) << '\n';
			exit(EXIT_FAILURE);
		}
		else if (r == 0 && dirs[i].new_dir_func)
			dirs[i].new_dir_func();
	}
}

void exit_main(Fl_Widget *w)
{
	if (Fl::event_key() == FL_Escape)
		return;
	cbExit();
#if SERIAL_DEBUG
	fclose(serlog);
#endif

}

void expand_controls(void*)
{
	show_controls();
}

void close_controls(void*)
{
	switch (progStatus.UIsize) {
		case wide_ui :
			if (EXPAND_CONTROLS && selrig->has_extras) return;
			btn_show_controls->label("@-22->");
			btn_show_controls->redraw_label();
			mainwindow->size(progStatus.mainW, 150);
			mainwindow->size_range(735, 150, 0, 150);
			if (tabs_dialog && tabs_dialog->visible()) tabs_dialog->hide();
			break;
		case small_ui :
			if (tabs_dialog && tabs_dialog->visible()) tabs_dialog->hide();
			break;
		case touch_ui :
		default :
			break;
	}
}

void startup(void*)
{
	initStatusConfigDialog();

	switch (progStatus.UIsize) {
		case touch_ui :
			mainwindow->size(progStatus.mainW, TOUCH_MAINH);
			mainwindow->size_range(TOUCH_MAINW, TOUCH_MAINH, 0, TOUCH_MAINH);
			mainwindow->redraw();
			break;
		case wide_ui :
		{
			if (EXPAND_CONTROLS && selrig->has_extras) return;
			btn_show_controls->label("@-22->");
			btn_show_controls->redraw_label();
			mainwindow->redraw();
		}
		case small_ui :
		default :
			break;
	}
	start_server(xmlport);
}

void rotate_log(std::string filename)
{
	std::string f1, f2, f3, f4, f5;

	f1.assign(filename).append(".1");
	f2.assign(filename).append(".2");
	f3.assign(filename).append(".3");
	f4.assign(filename).append(".4");
	f5.assign(filename).append(".5");

	rename(f4.c_str(), f5.c_str());
	rename(f3.c_str(), f4.c_str());
	rename(f2.c_str(), f3.c_str());
	rename(f1.c_str(), f2.c_str());
	rename(filename.c_str(), f1.c_str());

	std::ofstream tfile(filename.c_str());
	tfile << "flrig " << VERSION << std::endl << std::endl;
	tfile.close();

}

void flrig_terminate() {
	std::cerr << "terminating" << std::endl;
	fl_message("Closing flrig");
	cbExit();
}

int main (int argc, char *argv[])
{
	std::set_terminate(flrig_terminate);

	int arg_idx;
	HomeDir.clear();
	RigHomeDir.clear();

	Fl::args(argc, argv, arg_idx, parse_args);
	Fl::set_fonts(0);

	char dirbuf[FL_PATH_MAX + 1];
	std::string appdir = argv[0];
	size_t p;
#ifdef __WIN32__
	p = appdir.rfind("flrig.exe");
	if (p != std::string::npos) appdir.erase(p);
	p = appdir.find("FL_APPS\\");
	if (p != std::string::npos) {
		HomeDir.assign(appdir.substr(0, p + 8));
		RigHomeDir.assign(HomeDir).append("flrig.files\\");
	} else if (RigHomeDir.empty()) {
		fl_filename_expand(dirbuf, sizeof(dirbuf) -1, "$USERPROFILE/");
		HomeDir = dirbuf;
		RigHomeDir.assign(HomeDir).append("flrig.files\\");
	}
#else
		p = appdir.rfind("flrig");
		if (p != std::string::npos)
			appdir.erase(p);
		p = appdir.find("FL_APPS/");
		if (p != std::string::npos)
			RigHomeDir = appdir.substr(0, p + 8);
		if (RigHomeDir.empty()) {
			fl_filename_expand(dirbuf, FL_PATH_MAX, "$HOME/");
			HomeDir = dirbuf;
		}

		DIR *isdir = 0;
		std::string test_dir;
		test_dir.assign(HomeDir).append("flrig.files/");
		isdir = opendir(test_dir.c_str());
		if (isdir) {
			RigHomeDir = test_dir;
			closedir(isdir);
		} else if (RigHomeDir.empty()) {
			RigHomeDir.assign(HomeDir).append(".flrig/");
		}

#endif
	checkdirectories();

#if SERIAL_DEBUG
	std::string serlogname = RigHomeDir;
	serlogname.append("serlog.txt");
	serlog = fopen(serlogname.c_str(), "w");
#endif

	RigSerial	= new Cserial;
	SepSerial	= new Cserial;
	AuxSerial	= new Cserial;
	cwio_serial	= new Cserial();
	morse		= new Cmorse();
	FSK_serial	= new Cserial();

	try {
		std::string fname = RigHomeDir;
		fname.append("debug_log.txt");
		rotate_log(fname);
		debug::start(fname.c_str());
		time_t t = time(NULL);
		LOG(debug::INFO_LEVEL, debug::LOG_OTHER, _("%s log started on %s"), PACKAGE_STRING, ctime(&t));
		std::string trace_fname = RigHomeDir;
		trace_fname.append("trace.txt");
		rotate_log(trace_fname);

	}
	catch (const char* error) {
		std::cerr << error << '\n';
		debug::stop();
		exit(1);
	}

	progStatus.loadLastState();

	if (use_trace) progStatus.trace = true;

	switch (progStatus.UIsize) {
		case touch_ui :
			mainwindow = touch_rig_window();
			break;
		case small_ui :
			mainwindow = Small_rig_window();
			tabs_dialog = tabs_window();
			tabs_dialog->hide();
			break;
		case wide_ui :
		default :
			mainwindow = Wide_rig_window();
			tabs_dialog = tabs_window();
			tabs_dialog->hide();
	}
	mainwindow->callback(exit_main);

	meters_dialog = win_meters();

	progStatus.UI_laststate();

	cwio_keyer_dialog = new_cwio_dialog();
	cwio_editor = new_message_editor();
	cwio_configure = new_cwio_config_dialog();
	morse->init();

	FSK_keyer_dialog = fskio_window();
	FSK_editor = FSK_make_message_editor();
	FSK_configure = fskio_config_dialog();

	fntbrowser = new Font_Browser;
	dlgMemoryDialog = Memory_Dialog();
	dlgDisplayConfig = DisplayDialog();

	Fl::lock();

#if defined(__WIN32__) && defined(PTW32_STATIC_LIB)
	ptw32_init();
#endif

	bypass_serial_thread_loop = true;
	serial_thread = new pthread_t;
	if (pthread_create(serial_thread, NULL, serial_thread_loop, NULL)) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

//	start_server(xmlport);
	if (start_cwio_thread() != 0)
		return 1;
	if (FSK_start_thread() != 0)
		return 1;

	if (progStatus.cwioCONNECTED) {
		if (!open_cwkey()) {
			btn_cwioCONNECT->value(0);
			btn_cwioCAT->activate();
			btn_cwioAUX->activate();
			btn_cwioSEP->activate();
			progStatus.cwioCONNECTED = 0;
		} else {
			btn_cwioCONNECT->value(1);
			btn_cwioCAT->value(0); btn_cwioCAT->deactivate();
			btn_cwioAUX->value(0); btn_cwioAUX->deactivate();
			btn_cwioSEP->value(0); btn_cwioSEP->deactivate();
			progStatus.cwioCONNECTED = 1;
		}
	}

	if (progStatus.FSK_CONNECTED) {
		if (!FSK_open_port()) {
			btn_fskioCONNECT->value(0);
			btn_fskioCAT->activate();
			btn_fskioAUX->activate();
			btn_fskioSEP->activate();
			btn_fskioSHARED->activate();
			progStatus.FSK_CONNECTED = 0;
		} else {
			btn_fskioCONNECT->value(1);
			btn_fskioCAT->deactivate();
			btn_fskioAUX->deactivate();
			btn_fskioSEP->deactivate();
			btn_fskioSHARED->deactivate();
			progStatus.FSK_CONNECTED = 1;
		}
	}

	createXcvrDialog();

	btnALC_IDD_SWR->image(image_swr);
	meter_image = SWR_IMAGE;
	sldrRcvSignal->clear();
	sldrFwdPwr->clear();
	sldrALC->clear();
	sldrIDD->clear();
	sldrSWR->clear();

	switch (progStatus.UIsize) {
		case small_ui :
			mainwindow->resize(
				progStatus.mainX, progStatus.mainY,
				mainwindow->w(), 150);//mainwindow->h());
				grpInitializing->size(mainwindow->w(), mainwindow->h() - grpInitializing->y());
				grpInitializing->redraw();
				progress->position(progress->x(), grpInitializing->y() + grpInitializing->h()/2);
				progress->redraw();
			break;
		case wide_ui :
			mainwindow->resize(
				progStatus.mainX, progStatus.mainY,
				progStatus.mainW, btnVol->y() + btnVol->h() + 2);
			break;
		case touch_ui :
			mainwindow->resize(
				progStatus.mainX, progStatus.mainY,
				progStatus.mainW, TOUCH_MAINH);
		default :
			break;
	}
	mainwindow->xclass(KNAME);

#if defined(__WOE32__)
#  ifndef IDI_ICON
#    define IDI_ICON 101
#  endif
	mainwindow->icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(IDI_ICON)));
	mainwindow->show (argc, argv);
#elif !defined(__APPLE__)
	make_pixmap(&Rig_icon_pixmap, flrig_icon);
	mainwindow->icon((char *)Rig_icon_pixmap);
	mainwindow->show(argc, argv);
#else
	mainwindow->show(argc, argv);
#endif

	if (progStatus.UIsize != touch_ui) {
		btn_show_controls->label("@-28->");
		btn_show_controls->redraw_label();
	}

	Fl::add_timeout(0.1, startup);

	return Fl::run();

}

void cl_print(std::string cl)
{
	Fl_Double_Window clwin(50,50,600,500, "Command line text");
		Fl_Text_Display cldisplay(2,2,596,496);
		Fl_Text_Buffer  clbuff;

		cldisplay.buffer(&clbuff);
		cldisplay.textfont(FL_COURIER);

		cldisplay.insert(cl.c_str());

	clwin.end();
	clwin.resizable(cldisplay);

	clwin.show();

	while (clwin.visible()) {
		Fl::wait();
		MilliSleep(50);
	}
}

void cb_xml_help(Fl_Menu_*, void*)
{
	cl_print(print_xmlhelp());
}

int parse_args(int argc, char **argv, int& idx)
{
	std::string helpstr =
"Usage: \n\
  --help this help text\n\
  --version\n\
  --config-dir [fully qualified pathname to <DIR>]\n\
  --debug-level N (0..4)\n\
  --serial-debug \n\
  --trace\n\
  --xml-help\n\
  --xml-trace\n\
  --exp (expand menu tab controls)\n\
  --test\n";

	if (strcasecmp("--help", argv[idx]) == 0) {
#ifdef __WIN32__
		fl_alert2("%s", helpstr.c_str());
#else
		std::cout << helpstr << std::endl;
#endif
		exit(0);
	}
	if (strcasecmp("--version", argv[idx]) == 0) {
		std::string ver = "Version: ";
		ver.append(VERSION).append("\n");
		std::cout << ver;
		exit (0);
	}
	if (strcasecmp("--trace", argv[idx]) == 0) {
		use_trace = true;
		idx++;
		return 1;
	}
	if (strcasecmp("--xml-help", argv[idx]) == 0) {
		std::string help = print_xmlhelp();
		std::cout << help;
		exit(0);
	}
	if (strcasecmp("--test", argv[idx]) == 0) {
		testmode = true;
		idx++;
		return 1;
	}
	if (strcasecmp("--debug-level", argv[idx]) == 0) {
		std::string level = argv[idx + 1];
		switch (level[0]) {
			case '0': debug::level = debug::QUIET_LEVEL; break;
			case '1': debug::level = debug::ERROR_LEVEL; break;
			case '2': debug::level = debug::WARN_LEVEL; break;
			case '3': debug::level = debug::INFO_LEVEL; break;
			case '4': debug::level = debug::DEBUG_LEVEL; break;
			default : debug::level = debug::WARN_LEVEL;
		}
		idx += 2;
		return 1;
	}
	if (strcasecmp("--serial-debug", argv[idx]) == 0) {
		SERIALDEBUG = 1;
		idx++;
		return 1;
	}
	if (strcasecmp("--config-dir", argv[idx]) == 0) {
		RigHomeDir = argv[idx + 1];
		if (RigHomeDir[RigHomeDir.length()-1] != '/')
			RigHomeDir += '/';
		idx += 2;
		return 1;
	}
	if (strcasecmp("--exp", argv[idx]) == 0) {
		EXPAND_CONTROLS = true;
		idx++;
		return 1;
	}
#  ifdef __APPLE__
	if (strncasecmp("-psn", argv[idx], 4) == 0) {
		idx++;
		return 1;
	}
#endif
	fl_alert2("Unknown command line parameter: \"%s\"\n\n%s", argv[idx], helpstr.c_str());
	exit(0);

	return 0;
}
