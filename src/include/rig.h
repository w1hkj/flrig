#ifndef _Rig_H
#define _Rig_H

#include <string>
#include <sys/types.h>
#include <pthread.h>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Enumerations.H>

#ifndef WIN32
#include <unistd.h>
#include <pwd.h>
#endif

#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/fl_draw.H>

#include "config.h"

#include "FreqControl.h"
#include "rigpanel.h"
#include "rig_io.h"
#include "serial.h"
#include "support.h"
#include "xml_io.h"

extern Fl_Double_Window *mainwindow;
extern string HomeDir;
extern string RigHomeDir;
extern string TempDir;
extern string rigfile;
extern string pttfile;

extern string defFileName;
extern string title;

extern pthread_t *serial_thread;
extern pthread_t *digi_thread;
extern pthread_mutex_t mutex_serial;
extern pthread_mutex_t mutex_xmlrpc;
extern pthread_mutex_t mutex_queX;
extern pthread_mutex_t mutex_queA;
extern pthread_mutex_t mutex_queB;
extern pthread_mutex_t mutex_ptt;

extern bool RIG_DEBUG;
extern bool XML_DEBUG;

#endif
