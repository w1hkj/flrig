//=============================================================================
// Reference Transceiver unique drivers

#include "rigs.h"
#include "util.h"
#include "debug.h"
#include "rig_io.h"

rigbase			rig_null;

RIG_FT100D		rig_FT100D;
RIG_FT450		rig_FT450;
RIG_FT747		rig_FT747;
RIG_FT767		rig_FT767;
RIG_FT817		rig_FT817;
RIG_FT847		rig_FT847;
RIG_FT857D		rig_FT857D;
RIG_FT890		rig_FT890;
RIG_FT897D		rig_FT897D;
RIG_FT920		rig_FT920;
RIG_FT950		rig_FT950;
RIG_FT990		rig_FT990;
RIG_FT1000MP	rig_FT1000MP;
RIG_FT2000		rig_FT2000;
RIG_FTdx3000	rig_FTdx3000;
RIG_FT5000		rig_FT5000;

RIG_IC703		rig_IC703;
RIG_IC706MKIIG	rig_IC706MKIIG;
RIG_IC718		rig_IC718;
RIG_IC728		rig_IC728;
RIG_IC735		rig_IC735;
RIG_IC746		rig_IC746;
RIG_IC746PRO	rig_IC746PRO;
RIG_IC756PRO	rig_IC756PRO;
RIG_IC756PRO2	rig_IC756PRO2;
RIG_IC756PRO3	rig_IC756PRO3;
RIG_IC7000		rig_IC7000;
RIG_IC7100		rig_IC7100;
RIG_IC7200		rig_IC7200;
RIG_IC7600		rig_IC7600;
RIG_IC7700		rig_IC7700;
RIG_IC9100		rig_IC9100;
RIG_IC910H		rig_IC910H;

RIG_K2			rig_K2;
RIG_K3			rig_K3;

RIG_PCR1000		rig_PCR1000;
RIG_RAY152		rig_RAY152;

RIG_TS140		rig_TS140;
RIG_TS450S		rig_TS450S;
RIG_TS480HX		rig_TS480HX;
RIG_TS480SAT	rig_TS480SAT;
RIG_TS570		rig_TS570;
RIG_TS590S		rig_TS590S;
RIG_TS870S		rig_TS870S;
RIG_TS990		rig_TS990;
RIG_TS2000		rig_TS2000;

RIG_TT516		rig_TT516;
RIG_TT535		rig_TT535;
RIG_TT538		rig_TT538;
RIG_TT550		rig_TT550;
RIG_TT563		rig_TT563;
RIG_TT566		rig_TT566;
RIG_TT588		rig_TT588;
RIG_TT599		rig_TT599;

rigbase *rigs[] = {
	&rig_null,		// 0
	&rig_FT100D,	// 1
	&rig_FT450,		// 2
	&rig_FT747,		// 3
	&rig_FT767,		// 4
	&rig_FT817,		// 5
	&rig_FT847,		// 6
	&rig_FT857D,	// 7
	&rig_FT890,		// 8
	&rig_FT897D,	// 9
	&rig_FT920,		// 10
	&rig_FT950,		// 11
	&rig_FT990,		// 12
	&rig_FT1000MP,	// 13
	&rig_FT2000,	// 14
	&rig_FTdx3000,	// 15
	&rig_FT5000,	// 16
	&rig_IC703,		// 17
	&rig_IC706MKIIG,// 18
	&rig_IC718,		// 19
	&rig_IC728,		// 20
	&rig_IC735,		// 21
	&rig_IC746,		// 22
	&rig_IC746PRO,	// 23
	&rig_IC756PRO,	// 24
	&rig_IC756PRO2,	// 25
	&rig_IC756PRO3,	// 26
	&rig_IC7000,	// 27
	&rig_IC7100,	// 28
	&rig_IC7200,	// 29
	&rig_IC7600,	// 30
	&rig_IC7700,	// 31
	&rig_IC9100,	// 32
	&rig_IC910H,	// 33
	&rig_K2,		// 34
	&rig_K3,		// 35
	&rig_PCR1000,	// 36
	&rig_RAY152,	// 37
	&rig_TS140,		// 38
	&rig_TS450S,	// 39
	&rig_TS480HX,	// 40
	&rig_TS480SAT,	// 41
	&rig_TS570,		// 42
	&rig_TS590S,	// 43
	&rig_TS870S,	// 44 wbx
	&rig_TS990,		// 45
	&rig_TS2000,	// 46
	&rig_TT516,		// 47
	&rig_TT535,		// 48
	&rig_TT538,		// 49
	&rig_TT550,		// 50
	&rig_TT563,		// 51
	&rig_TT566,		// 52
	&rig_TT588,		// 53
	&rig_TT599,		// 54
	NULL
};

//=============================================================================
