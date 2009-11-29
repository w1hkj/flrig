#include "config.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
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

#ifdef WIN32
#  include "flrigrc.h"
#  include "compat.h"
#  define dirent fl_dirent_no_thanks
#endif

#include <FL/filename.H>
#ifdef __MINGW32__
#  undef dirent
#endif

//#include "fileselect.h"
#include <dirent.h>

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
#include "sockxml_io.h"

#include "flrig_icon.cxx"

Fl_Double_Window *mainwindow;
string RigHomeDir;
string TempDir;
string defFileName;
string title;

pthread_t *serial_thread = 0;
pthread_t *digi_thread = 0;

pthread_mutex_t mutex_serial = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_xmlrpc = PTHREAD_MUTEX_INITIALIZER;

//----------------------------------------------------------------------

extern void saveFreqList();

void * flrig_terminate(void) {
	std::cerr << "terminating" << std::endl;
	
	fl_message("Closing flrig");

	progStatus.rig_nbr = rig_nbr;
	progStatus.opMODE = opMODE->index();
	progStatus.opBW = opBW->index();
	progStatus.freq = FreqDisp->value();
	progStatus.imode_B = vfoB.imode;
	progStatus.iBW_B = vfoB.iBW;
	progStatus.freq_B = vfoB.freq;

	progStatus.mute = btnVol->value();
	progStatus.volume = sldrVOLUME->value();
	progStatus.power_level = sldrPOWER->value();
	progStatus.mic_gain = sldrMICGAIN->value();
	progStatus.notch = btnNotch->value();
	progStatus.notch_val = sldrNOTCH->value();
	progStatus.shift = btnIFsh->value();
	progStatus.shift_val = sldrIFSHIFT->value();
	progStatus.noise = btnNOISE->value();
	progStatus.attenuator = btnAttenuator->value();
	progStatus.preamp = btnPreamp->value();

	progStatus.saveLastState();

	saveFreqList();

	exit(1);
}

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
		string& dir;
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
			cerr << _("Could not make directory") << ' ' << dirs[i].dir
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
}

extern void open_rig_socket();
extern bool run_digi_loop;

void startup(void*)
{
	btnInitializing->show();
	mainwindow->redraw();
	initStatusConfigDialog();

}

int main (int argc, char *argv[])
{
	std::terminate_handler(flrig_terminate);

	if (argc > 1) {
		if (strcasecmp("--help", argv[1]) == 0) {
			printf("\
  --help this help text\n\
  --version\n\
\n\
  Usage: flrig\n");
			return 0;
		} 
		if (strcasecmp("--version", argv[1]) == 0) {
			printf("Version: "VERSION"\n");
			return 0;
		}
	}

	mainwindow = Rig_window();
	mainwindow->callback(exit_main);

	dlgMemoryDialog = Memory_Dialog();
	dlgDisplayConfig = DisplayDialog();

	char dirbuf[FL_PATH_MAX + 1];
#ifdef __WIN32__
	fl_filename_expand(dirbuf, sizeof(dirbuf) - 1, "$USERPROFILE/flrig.files/");
#else
	fl_filename_expand(dirbuf, sizeof(dirbuf) - 1, "$HOME/.flrig/");
#endif
	RigHomeDir = dirbuf;
	checkdirectories();

	try {
		debug::start(string(RigHomeDir).append("status_log.txt").c_str());
		time_t t = time(NULL);
		LOG(debug::WARN_LEVEL, debug::LOG_OTHER, _("%s log started on %s"), PACKAGE_STRING, ctime(&t));
	}
	catch (const char* error) {
		cerr << error << '\n';
		debug::stop();
		exit(1);
	}

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

    open_rig_socket();

	wait_query = true;
	digi_thread = new pthread_t;      
	if (pthread_create(digi_thread, NULL, digi_loop, NULL)) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

	progStatus.loadLastState();
	createXcvrDialog();

	btnALC_SWR->image(image_swr);
	sldrRcvSignal->value(0);
	sldrFwdPwr->value(0);
	sldrALC_SWR->value(0);

	mainwindow->resize( progStatus.mainX, progStatus.mainY, mainwindow->w(), btnInitializing->h() + 24 );

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
	mainwindow->show();
#endif

	Fl::add_timeout(0.250, startup);

	return Fl::run();

}

