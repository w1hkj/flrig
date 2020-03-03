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

//=============================================================================
// Reference Transceiver unique drivers

#include "rigs.h"
#include "util.h"
#include "debug.h"
#include "rig_io.h"

rigbase			rig_null;

RIG_AOR5K		rig_AOR5K;
RIG_FT100D		rig_FT100D;
RIG_FT450		rig_FT450;
RIG_FT450D		rig_FT450D;
RIG_FT747		rig_FT747;
RIG_FT767		rig_FT767;
RIG_FT817		rig_FT817;
RIG_FT847		rig_FT847;
RIG_FT857D		rig_FT857D;
RIG_FT890		rig_FT890;
RIG_FT897D		rig_FT897D;
RIG_FT891		rig_FT891;
RIG_FT900		rig_FT900;
RIG_FT920		rig_FT920;
RIG_FT950		rig_FT950;
RIG_FT990		rig_FT990;
RIG_FT990A		rig_FT990A;
RIG_FT991		rig_FT991;
RIG_FT991A		rig_FT991A;
RIG_FT1000		rig_FT1000;
RIG_FT1000MP	rig_FT1000MP;
RIG_FT2000		rig_FT2000;
RIG_FTdx101D	rig_FTdx101D;
RIG_FTdx101MP	rig_FTdx101MP;
RIG_FTdx1200	rig_FTdx1200;
RIG_FTdx3000	rig_FTdx3000;
RIG_FT5000		rig_FT5000;
RIG_FTdx9000	rig_FTdx9000;

RIG_IC703		rig_IC703;
RIG_IC706MKIIG	rig_IC706MKIIG;
RIG_IC718		rig_IC718;
RIG_IC728		rig_IC728;
RIG_IC735		rig_IC735;
RIG_IC746		rig_IC746;
RIG_IC746PRO	rig_IC746PRO;
RIG_IC756		rig_IC756;
RIG_IC756PRO	rig_IC756PRO;
RIG_IC756PRO2	rig_IC756PRO2;
RIG_IC756PRO3	rig_IC756PRO3;
RIG_IC7000		rig_IC7000;
RIG_IC7100		rig_IC7100;
RIG_IC7410		rig_IC7410;
RIG_IC7200		rig_IC7200;
RIG_IC7300		rig_IC7300;
RIG_IC7600		rig_IC7600;
RIG_IC7610		rig_IC7610;
RIG_IC7700		rig_IC7700;
RIG_IC7800		rig_IC7800;
RIG_IC7851		rig_IC7851;
RIG_IC9100		rig_IC9100;
RIG_IC9700		rig_IC9700;
RIG_IC910H		rig_IC910H;
RIG_ICF8101		rig_ICF8101;

RIG_K2			rig_K2;
RIG_K3			rig_K3;
RIG_KX3			rig_KX3;

RIG_PCR1000		rig_PCR1000;
RIG_RAY152		rig_RAY152;

RIG_TS140		rig_TS140;
RIG_TS450S		rig_TS450S;
RIG_TS480HX		rig_TS480HX;
RIG_TS480SAT	rig_TS480SAT;
RIG_TS570		rig_TS570;
RIG_TS590S		rig_TS590S;
RIG_TS590SG		rig_TS590SG;
RIG_TS790		rig_TS790;
RIG_TS850		rig_TS850;
RIG_TS890S		rig_TS890S;
RIG_TS950		rig_TS950;
RIG_TS870S		rig_TS870S;
RIG_TS940S		rig_TS940S;
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
	&rig_FT450D,	// 3
	&rig_FT747,		// 4
	&rig_FT767,		// 5
	&rig_FT817,		// 6
	&rig_FT847,		// 7
	&rig_FT857D,	// 8
	&rig_FT890,		// 9
	&rig_FT897D,	// 10
	&rig_FT891,		// 11
	&rig_FT900,		// 12
	&rig_FT920,		// 13
	&rig_FT950,		// 14
	&rig_FT990,		// 15
	&rig_FT990A,	// 16
	&rig_FT991,		// 17
	&rig_FT991A,	// 18
	&rig_FT1000,	// 19
	&rig_FT1000MP,	// 20
	&rig_FT2000,	// 21
	&rig_FTdx101D,	// 22
	&rig_FTdx101MP,	// 23
	&rig_FTdx1200,	// 24
	&rig_FTdx3000,	// 25
	&rig_FT5000,	// 26
	&rig_FTdx9000,	// 27
	&rig_IC703,		// 28
	&rig_IC706MKIIG,// 29
	&rig_IC718,		// 30
	&rig_IC728,		// 31
	&rig_IC735,		// 32
	&rig_IC746,		// 33
	&rig_IC746PRO,	// 34
	&rig_IC756,		// 35
	&rig_IC756PRO,	// 36
	&rig_IC756PRO2,	// 37
	&rig_IC756PRO3,	// 38
	&rig_IC7000,	// 39
	&rig_IC7100,	// 40
	&rig_IC7200,	// 41
	&rig_IC7300,	// 42
	&rig_IC7410,	// 43
	&rig_IC7600,	// 44
	&rig_IC7610,	// 45
	&rig_IC7700,	// 46
	&rig_IC7800,	// 47
	&rig_IC7851,	// 48
	&rig_IC9100,	// 49
	&rig_IC9700,	// 50
	&rig_IC910H,	// 51
	&rig_ICF8101,	// 52
	&rig_K2,		// 53
	&rig_K3,		// 54
	&rig_KX3,		// 55
	&rig_PCR1000,	// 56
	&rig_RAY152,	// 57
	&rig_TS140,		// 58
	&rig_TS450S,	// 59
	&rig_TS480HX,	// 60
	&rig_TS480SAT,	// 61
	&rig_TS570,		// 62
	&rig_TS590S,	// 63
	&rig_TS590SG,	// 64
	&rig_TS790,		// 65
	&rig_TS850,		// 66
	&rig_TS870S,	// 67 wbx
	&rig_TS890S,	// 68
	&rig_TS940S,	// 69
	&rig_TS950,		// 70
	&rig_TS990,		// 71
	&rig_TS2000,	// 72
	&rig_TT516,		// 73
	&rig_TT535,		// 74
	&rig_TT538,		// 75
	&rig_TT550,		// 76
	&rig_TT563,		// 77
	&rig_TT566,		// 78
	&rig_TT588,		// 79
	&rig_TT599,		// 80
	&rig_AOR5K,		// 81
	NULL
};

//=============================================================================
