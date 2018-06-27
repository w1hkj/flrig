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

#ifndef _PCR1000_H
#define _PCR1000_H

#include "rigbase.h"

class RIG_PCR1000 : public rigbase {

public:

private:

	static const char 		get_smeter_command[] ;
	static char 			volume_command[] ;
	static char 			squelch_command[] ;
	static char 			if_shift_command[] ;
	static char				check_power_command[] ;
	static char				power_on_command[] ;
	static char				power_off_command[] ;
	static const char		att_off_command[] ;
	static const char		att_on_command[] ;
	static const char		noise_off_command[] ;
	static const char		noise_on_command[] ;

	static const char name[] ;

	static const char *modes[] ;
	static const char mode_chr[] ;
	static const char mode_type[] ;
	static const char *band_widths[] ;

	int current_volume ;

	struct freq_cmd {
		char command[2] ;		// The command will always be 'K0'
		char frequency[10] ;	// 10 digit frequency in Hertz
		char mode[2] ;			// Mode code, 00=LSB, 01=USB, 02=AM, 03=CW, 05=NFM, 06=WFM, 04 is NOT USED!
		char band_width[2] ;	// Band width setting 00=2.8kHz, 01=6kHz, 02=15kHz, 03=50kHz, 04=230kHz
		char pad[5] ;			// Always '00\0'
	} ;

	static freq_cmd freq_cmds[] ;
	static const char hex_chars[] ;

	bool 	notch_on;
	int  	att_level;
	int  	nb_level;
	int  	active_bandwidth;
	int  	active_mode;
	int  	sql ;
	int		if_shift ;
	int		attenuator ;
	int		noise ;

	bool att_on;
	bool is_PCR1000;
	
	XCVR_STATE&	current_vfo ;

	// Internal methods
	/*
	 * Since the same command is used for frequency, mode, and bandwidth we'll
	 * use a single private method that is called whenever any of the three is
	 * changed.
	 */

	void setFreqModeBW(XCVR_STATE &freqMode) ; 	// Set the frequency in the current mode
	int hexTo(const char c) const ; 			// Returns int value of c, i.e '2' returns 2, 'A' returns 10, etc.
	void set2Hex(int ival, char *cptr) ;		// Set the hex value of ival into the first 2 chars of cptr

public:
	RIG_PCR1000();
	~RIG_PCR1000(){};
	
	void initialize();
	void shutdown() ;

	void set_volume_control(int val);
	int  get_volume_control();

	bool check();

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB(long);

	void selectA() ;
	void selectB() ;
	void A2B() ;
	void swapAB() ;

	int  get_modetype(int n);
	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();

	void set_bwA(int val) ;
	int  get_bwA() ;
	void set_bwB(int val) ;
	int  get_bwB() ;

	int  get_smeter();

	void set_squelch(int val) ;
	int  get_squelch() ;
	void get_squelch_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 255; step = 1; }

	void set_if_shift(int val) ;
	bool get_if_shift(int &val) ;
	void get_if_min_max_step(int &min, int &max, int &step) {
		if_shift_min = min = -800; if_shift_max = max = 800; if_shift_step = step = 10; }
	void get_if_mid() {if_shift_mid = 0; }
	

	void set_attenuator(int val) ;
	int  get_attenuator() ;

	void set_noise(bool on) ;
	int  get_noise() ;

};


#endif
