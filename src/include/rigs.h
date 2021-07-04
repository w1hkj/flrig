// ----------------------------------------------------------------------------
// Copyright (C) 2014, 2020
//              David Freese, W1HKJ
//              Michael Black W9MDB
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
#include "other/AOR5K.h"
#include "icom/ICbase.h"
#include "icom/IC703.h"
#include "icom/IC705.h"
#include "icom/IC706MKIIG.h"
#include "icom/IC718.h"
#include "icom/IC728.h"
#include "icom/IC735.h"
#include "icom/IC746.h"
#include "icom/IC756.h"
#include "icom/IC756PRO2.h"
#include "icom/IC756PRO3.h"
#include "icom/IC910.h"
#include "icom/IC7000.h"
#include "icom/IC7100.h"
#include "icom/IC7410.h"
#include "icom/IC7200.h"
#include "icom/IC7300.h"
#include "icom/IC7600.h"
#include "icom/IC7610.h"
#include "icom/IC7700.h"
#include "icom/IC7800.h"
#include "icom/IC7851.h"
#include "icom/IC9100.h"
#include "icom/IC9700.h"
#include "icom/ICF8101.h"
#include "elecraft/K2.h"
#include "elecraft/K3.h"
#include "elecraft/KX3.h"
#include "kenwood/TS140.h"
#include "kenwood/TS440.h"
#include "kenwood/TS450S.h"
#include "kenwood/TS480HX.h"
#include "kenwood/TS480SAT.h"
#include "kenwood/TS570.h"
#include "kenwood/TS590S.h"
#include "kenwood/TS590SG.h"
#include "kenwood/TS790.h"
#include "kenwood/TS850.h"
#include "kenwood/TS870S.h"
#include "kenwood/TS890S.h"
#include "kenwood/TS940S.h"
#include "kenwood/TS950.h"
#include "kenwood/TS990.h"
#include "kenwood/TS2000.h"
#include "other/AOR5K.h"
#include "other/PCR1000.h"
#include "other/RAY152.h"
#include "other/TMD710.h"
#include "other/FLEX1500.h"
#include "other/PowerSDR.h"
#include "tentec/TT516.h"
#include "tentec/DELTA-II.h"
#include "tentec/TT538.h"
#include "tentec/TT550.h"
#include "tentec/TT563.h"
#include "tentec/TT566.h"
#include "tentec/TT588.h"
#include "tentec/TT599.h"
#include "xiegu/Xiegu-5105.h"
#include "xiegu/Xiegu-G90.h"
#include "yaesu/FT100D.h"
#include "yaesu/FT450.h"
#include "yaesu/FT450D.h"
#include "yaesu/FT736R.h"
#include "yaesu/FT747.h"
#include "yaesu/FT767.h"
#include "yaesu/FT817.h"
#include "yaesu/FT817BB.h"
#include "yaesu/FT818.h"
#include "yaesu/FT847.h"
#include "yaesu/FT757GX2.h"
#include "yaesu/FT857D.h"
#include "yaesu/FT890.h"
#include "yaesu/FT891.h"
#include "yaesu/FT900.h"
#include "yaesu/FT920.h"
#include "yaesu/FT950.h"
#include "yaesu/FT990.h"
#include "yaesu/FT990a.h"
#include "yaesu/FT991.h"
#include "yaesu/FT991A.h"
#include "yaesu/FT1000.h"
#include "yaesu/FT1000MP.h"
#include "yaesu/FT2000.h"
#include "yaesu/FTdx101D.h"
#include "yaesu/FTdx1200.h"
#include "yaesu/FTdx3000.h"
#include "yaesu/FTdx9000.h"
#include "yaesu/FT5000.h"

extern rigbase			rig_null;		// 0
extern RIG_FT100D		rig_FT100D;		// 1
extern RIG_FT450		rig_FT450;		// 2
extern RIG_FT450D		rig_FT450D;		// 3
extern RIG_FT747		rig_FT747;		// 4
extern RIG_FT736R		rig_FT736R;		// 5
extern RIG_FT757GX2		rig_FT757GX2;	// 6
extern RIG_FT767		rig_FT767;		// 7
extern RIG_FT817		rig_FT817;		// 8
extern RIG_FT817BB		rig_FT817BB;	// 9
extern RIG_FT818ND		rig_FT818ND;	// 10
extern RIG_FT847		rig_FT847;		// 11
extern RIG_FT857D		rig_FT857D;		// 12
extern RIG_FT890		rig_FT890;		// 13
extern RIG_FT891		rig_FT891;		// 14
extern RIG_FT897D		rig_FT897D;		// 15
extern RIG_FT900		rig_FT900;		// 16
extern RIG_FT920		rig_FT920;		// 17
extern RIG_FT950		rig_FT950;		// 18
extern RIG_FT990		rig_FT990;		// 19
extern RIG_FT990A		rig_FT990A;		// 20
extern RIG_FT991		rig_FT991;		// 21
extern RIG_FT991A		rig_FT991A;		// 22
extern RIG_FT1000		rig_FT1000;		// 23
extern RIG_FT1000MP     rig_FT1000MP;	// 24
extern RIG_FT2000		rig_FT2000;		// 25
extern RIG_FTdx101D		rig_FTdx101D;	// 26
extern RIG_FTdx101MP	rig_FTdx101MP;	// 27
extern RIG_FTdx1200		rig_FTdx1200;	// 28
extern RIG_FTdx3000		rig_FTdx3000;	// 29
extern RIG_FT5000		rig_FT5000;		// 30
extern RIG_FTdx9000		rig_FTdx9000;	// 31
extern RIG_IC703		rig_IC703;		// 32
extern RIG_IC705		rig_IC705;		// 33
extern RIG_IC706MKIIG	rig_IC706MKIIG;	// 34
extern RIG_IC718		rig_IC718;		// 35
extern RIG_IC728		rig_IC728;		// 36
extern RIG_IC735		rig_IC735;		// 37
extern RIG_IC746		rig_IC746;		// 38
extern RIG_IC746PRO     rig_IC746PRO;	// 39
extern RIG_IC756		rig_IC756;		// 40
extern RIG_IC756PRO     rig_IC756PRO;	// 41
extern RIG_IC756PRO2	rig_IC756PRO2;	// 42
extern RIG_IC756PRO3	rig_IC756PRO3;	// 43
extern RIG_IC7000		rig_IC7000;		// 44
extern RIG_IC7100		rig_IC7100;		// 45
extern RIG_IC7200		rig_IC7200;		// 46
extern RIG_IC7300		rig_IC7300;		// 47
extern RIG_IC7410		rig_IC7410;		// 48
extern RIG_IC7600		rig_IC7600;		// 49
extern RIG_IC7610		rig_IC7610;		// 50
extern RIG_IC7700		rig_IC7700;		// 51
extern RIG_IC7800		rig_IC7800;		// 52
extern RIG_IC7851		rig_IC7851;		// 53
extern RIG_IC9100		rig_IC9100;		// 54
extern RIG_IC9700		rig_IC9700;		// 55
extern RIG_IC910H		rig_IC910H;		// 56
extern RIG_ICF8101		rig_ICF8101;	// 57
extern RIG_K2			rig_K2;			// 58
extern RIG_K3			rig_K3;			// 59
extern RIG_KX3			rig_KX3;		// 60
extern RIG_PCR1000		rig_PCR1000;	// 61
extern RIG_RAY152		rig_RAY152;		// 62
extern RIG_TMD710		rig_TMD710;		// 63
extern RIG_TS440		rig_TS440;		// 64
extern RIG_TS140		rig_TS140;		// 65
extern RIG_TS450S		rig_TS450S;		// 66
extern RIG_TS480HX		rig_TS480HX;	// 67
extern RIG_TS480SAT	    rig_TS480SAT;	// 68
extern RIG_TS570		rig_TS570;		// 69
extern RIG_TS590S		rig_TS590S;		// 70
extern RIG_TS590SG		rig_TS590SG;	// 71
extern RIG_TS790		rig_TS790;		// 72
extern RIG_TS850		rig_TS850;		// 73
extern RIG_TS870S		rig_TS870S;		// 74
extern RIG_TS890S		rig_TS890S;		// 75
extern RIG_TS940S		rig_TS940S;		// 76
extern RIG_TS950		rig_TS950;		// 77
extern RIG_TS990		rig_TS990;		// 78
extern RIG_TS2000		rig_TS2000;		// 79
extern RIG_TT516		rig_TT516;		// 80
extern RIG_TT535		rig_TT535;		// 81
extern RIG_TT538		rig_TT538;		// 82
extern RIG_TT550		rig_TT550;		// 83
extern RIG_TT563		rig_TT563;		// 84
extern RIG_TT566		rig_TT566;		// 85
extern RIG_TT588		rig_TT588;		// 86
extern RIG_TT599		rig_TT599;		// 87
extern RIG_AOR5K		rig_AOR5K;		// 88
extern RIG_XI5105		rig_XI5105;		// 89
extern RIG_Xiegu_G90	rig_XIG90;		// 90
extern RIG_PowerSDR		rig_PowerSDR;	// 91
extern RIG_FLEX1500		rig_FLEX1500;	// 92

#endif
