#include <string>

#include "support.h"

struct CPT {
	int nbr;
	std::string szPort;
};

extern Fl_Double_Window *dlgDisplayConfig;
extern Fl_Double_Window *dlgXcvrConfig;
extern Fl_Double_Window *dlgMemoryDialog;

extern CPT commPortTable[];
extern int iNbrCommPorts;
