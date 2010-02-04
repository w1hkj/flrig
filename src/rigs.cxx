//=============================================================================
// Reference Transceiver unique drivers


#include "rigbase.h"
#include "util.h"
#include "debug.h"
#include "rig_io.h"

#include "FT100D.h"
#include "FT450.h"
#include "FT767.h"
#include "FT817.h"
#include "FT857D.h"
#include "FT950.h"
#include "FT2000.h"
#include "IC706MKIIG.h"
#include "IC728.h"
#include "IC735.h"
#include "ICgeneric.h"
#include "K2.h"
#include "K3.h"
#include "TS140.h"
#include "TS570.h"
#include "TS2000.h"
#include "TT516.h"
#include "TT538.h"
#include "TT550.h"
#include "TT563.h"

rigbase			rig_null;
RIG_FT100D		rig_FT100D;
RIG_FT450		rig_FT450;
RIG_FT767		rig_FT767;
RIG_FT817		rig_FT817;
RIG_FT857D		rig_FT857D;
RIG_FT897D		rig_FT897D;
RIG_FT950		rig_FT950;
RIG_FT2000		rig_FT2000;
RIG_IC706MKIIG	rig_IC706MKIIG;
RIG_IC728		rig_IC728;
RIG_IC735		rig_IC735;
RIG_IC746		rig_IC746;
RIG_IC746PRO	rig_IC746PRO;
RIG_IC756PRO2	rig_IC756PRO2;
RIG_IC756PRO3	rig_IC756PRO3;
RIG_IC7000		rig_IC7000;
RIG_IC7200		rig_IC7200;
RIG_IC7700		rig_IC7700;
RIG_IC910H		rig_IC910H;
RIG_K2			rig_K2;
RIG_K3			rig_K3;
RIG_TS140		rig_TS140;
RIG_TS570		rig_TS570;
RIG_TS2000		rig_TS2000;
RIG_TT516		rig_TT516;
RIG_TT538		rig_TT538;
RIG_TT550		rig_TT550;
RIG_TT563		rig_TT563;

rigbase *rigs[] = {
	&rig_null,
	&rig_FT100D,
	&rig_FT450,
	&rig_FT767,
	&rig_FT817,
	&rig_FT857D,
	&rig_FT897D,
	&rig_FT950,
	&rig_FT2000,
	&rig_IC706MKIIG,
	&rig_IC728,
	&rig_IC735,
	&rig_IC746,
	&rig_IC746PRO,
	&rig_IC756PRO2,
	&rig_IC756PRO3,
	&rig_IC7000,
	&rig_IC7200,
	&rig_IC7700,
	&rig_IC910H,
	&rig_K2,
	&rig_K3,
	&rig_TS140,
	&rig_TS570,
	&rig_TS2000,
	&rig_TT516,
	&rig_TT538,
	&rig_TT550,
	&rig_TT563,
	NULL
};

//=============================================================================
