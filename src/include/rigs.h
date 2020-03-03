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

#ifndef RIGS_H
#define RIGS_H

#include "rigbase.h"
#include "AOR5K.h"
#include "FT100D.h"
#include "FT450.h"
#include "FT450D.h"
#include "FT747.h"
#include "FT767.h"
#include "FT817.h"
#include "FT847.h"
#include "FT857D.h"
#include "FT890.h"
#include "FT891.h"
#include "FT900.h"
#include "FT920.h"
#include "FT950.h"
#include "FT990.h"
#include "FT990a.h"
#include "FT991.h"
#include "FT991A.h"
#include "FT1000.h"
#include "FT1000MP.h"
#include "FT2000.h"
#include "FTdx101D.h"
#include "FTdx1200.h"
#include "FTdx3000.h"
#include "FTdx9000.h"
#include "FT5000.h"
#include "ICbase.h"
#include "IC703.h"
#include "IC706MKIIG.h"
#include "IC718.h"
#include "IC728.h"
#include "IC735.h"
#include "IC746.h"
#include "IC756.h"
#include "IC756PRO2.h"
#include "IC756PRO3.h"
#include "IC910.h"
#include "IC7000.h"
#include "IC7100.h"
#include "IC7410.h"
#include "IC7200.h"
#include "IC7300.h"
#include "IC7600.h"
#include "IC7610.h"
#include "IC7700.h"
#include "IC7800.h"
#include "IC7851.h"
#include "IC9100.h"
#include "IC9700.h"
#include "ICF8101.h"
#include "K2.h"
#include "K3.h"
#include "KX3.h"
#include "PCR1000.h"
#include "RAY152.h"
#include "TS140.h"
#include "TS450S.h"
#include "TS480HX.h"
#include "TS480SAT.h"
#include "TS570.h"
#include "TS590S.h"
#include "TS590SG.h"
#include "TS790.h"
#include "TS850.h"
#include "TS870S.h"		// wbx
#include "TS890S.h"
#include "TS940S.h"
#include "TS950.h"
#include "TS990.h"		// G0HIX
#include "TS2000.h"
#include "TT516.h"
#include "DELTA-II.h"
#include "TT538.h"
#include "TT550.h"
#include "TT563.h"
#include "TT566.h"
#include "TT588.h"
#include "TT599.h"

extern rigbase			rig_null;		// 0
extern RIG_FT100D		rig_FT100D;		// 1
extern RIG_FT450		rig_FT450;		// 2
extern RIG_FT450D		rig_FT450D;		// 3
extern RIG_FT747		rig_FT747;		// 4
extern RIG_FT767		rig_FT767;		// 5
extern RIG_FT817		rig_FT817;		// 6
extern RIG_FT847		rig_FT847;		// 7
extern RIG_FT857D		rig_FT857D;		// 8
extern RIG_FT890		rig_FT890;		// 9
extern RIG_FT891		rig_FT891;		// 10
extern RIG_FT897D		rig_FT897D;		// 11
extern RIG_FT900		rig_FT900;		// 12
extern RIG_FT920		rig_FT920;		// 13
extern RIG_FT950		rig_FT950;		// 14
extern RIG_FT990		rig_FT990;		// 15
extern RIG_FT990A		rig_FT990A;		// 16
extern RIG_FT991		rig_FT991;		// 17
extern RIG_FT991A		rig_FT991A;		// 18
extern RIG_FT1000		rig_FT1000;		// 19
extern RIG_FT1000MP     rig_FT1000MP;	// 20
extern RIG_FT2000		rig_FT2000;		// 21
extern RIG_FTdx101D		rig_FTdx101D;	// 22
extern RIG_FTdx101MP	rig_FTdx101MP;	// 23
extern RIG_FTdx1200		rig_FTdx1200;	// 24
extern RIG_FTdx3000		rig_FTdx3000;	// 25
extern RIG_FT5000		rig_FT5000;		// 26
extern RIG_FTdx9000		rig_FTdx9000;	// 27
extern RIG_IC703		rig_IC703;		// 28
extern RIG_IC706MKIIG	rig_IC706MKIIG;	// 29
extern RIG_IC718		rig_IC718;		// 30
extern RIG_IC728		rig_IC728;		// 31
extern RIG_IC735		rig_IC735;		// 32
extern RIG_IC746		rig_IC746;		// 33
extern RIG_IC746PRO     rig_IC746PRO;	// 34
extern RIG_IC756		rig_IC756;		// 35
extern RIG_IC756PRO     rig_IC756PRO;	// 36
extern RIG_IC756PRO2	rig_IC756PRO2;	// 37
extern RIG_IC756PRO3	rig_IC756PRO3;	// 38
extern RIG_IC7000		rig_IC7000;		// 39
extern RIG_IC7100		rig_IC7100;		// 40
extern RIG_IC7200		rig_IC7200;		// 41
extern RIG_IC7300		rig_IC7300;		// 42
extern RIG_IC7410		rig_IC7410;		// 43
extern RIG_IC7600		rig_IC7600;		// 44
extern RIG_IC7610		rig_IC7610;		// 45
extern RIG_IC7700		rig_IC7700;		// 46
extern RIG_IC7800		rig_IC7800;		// 47
extern RIG_IC7851		rig_IC7851;		// 48
extern RIG_IC9100		rig_IC9100;		// 49
extern RIG_IC9700		rig_IC9700;		// 50
extern RIG_IC910H		rig_IC910H;		// 51
extern RIG_ICF8101		rig_ICF8101;	// 52
extern RIG_K2			rig_K2;			// 53
extern RIG_K3			rig_K3;			// 54
extern RIG_KX3			rig_KX3;		// 55
extern RIG_PCR1000		rig_PCR1000;	// 56
extern RIG_RAY152		rig_RAY152;		// 57
extern RIG_TS140		rig_TS140;		// 58
extern RIG_TS450S		rig_TS450S;		// 59
extern RIG_TS480HX		rig_TS480HX;	// 60
extern RIG_TS480SAT	    rig_TS480SAT;	// 61
extern RIG_TS570		rig_TS570;		// 62
extern RIG_TS590S		rig_TS590S;		// 63
extern RIG_TS590SG		rig_TS590SG;	// 64
extern RIG_TS790		rig_TS790;		// 65
extern RIG_TS850		rig_TS850;		// 66
extern RIG_TS870S		rig_TS870S;		// 67 wbx
extern RIG_TS890S		rig_TS890S;		// 68
extern RIG_TS940S		rig_TS940S;		// 69
extern RIG_TS950		rig_TS950;		// 70
extern RIG_TS990		rig_TS990;		// 71 G0HIX
extern RIG_TS2000		rig_TS2000;		// 72
extern RIG_TT516		rig_TT516;		// 73
extern RIG_TT535		rig_TT535;		// 74
extern RIG_TT538		rig_TT538;		// 75
extern RIG_TT550		rig_TT550;		// 76
extern RIG_TT563		rig_TT563;		// 77
extern RIG_TT566		rig_TT566;		// 78
extern RIG_TT588		rig_TT588;		// 79
extern RIG_TT599		rig_TT599;		// 80
extern RIG_AOR5K		rig_AOR5K;		// 81

#endif
