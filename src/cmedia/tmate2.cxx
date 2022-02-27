// ----------------------------------------------------------------------------
// Copyright (C) 2021
//              David Freese, W1HKJ
//              Dominik, DB1RUF
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
//
// This interface uses hidapi to communicate with the Elard TMATE 2 device
//
// adapt frequency step size to your transceiver in the setting menu
//
// ----------------------------------------------------------------------------

#include <iostream>
#include <fstream>

#include <string>
#include <map>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include <FL/Fl.H>

#ifndef __WIN32__
#include <termios.h>
#include <glob.h>
#  ifndef __APPLE__
#    include <error.h>
#  endif
#endif

#include "config.h"
#include "threads.h"
#include "debug.h"
#include "util.h"
#include "support.h"
#include "status.h"

#include "hidapi.h"
#include "tmate2.h"

// enable TMATE2_DEBUG for debugging output

//#define TMATE2_DEBUG 1

int ChangeSpeedEvalTime_ms = 500;

static std::map<std::string, std::string> paths;
static hid_device *tm2_dev = (hid_device *)0;

//this is used for writing to USB it contains settings, LED and LCD symbols
unsigned char lcdstring[LCD_STRING_SIZE];

int LcdRefreshTime_ms=100;

pthread_t *tmate_thread = 0;
std::string tm2_dev_path;

int tmate2_close_connection(void)  //currently no function executed in hidapi
{
	// Finalize the hidapi library
	// Close the device
	return hid_exit();
}

void tmate2_close()
{
	delete tm2_dev;
	tm2_dev = (hid_device *)0;
}

//reads the knob values from the device
int tmate2_read_value (unsigned short *enc1, unsigned short *enc2, unsigned short *enc3, unsigned short *keys)
{
	// This function gives the values of the buttons and knob's back
	// enc1 = big knob
	// enc2 = knob E2
	// enc2 = knob E3
	// keys = buttons F1 to F6 and pressed E1 E2 big knob
	int res = 0;
	unsigned char buf[66];
	tm2_dev = hid_open_path(tm2_dev_path);  //is needed Warum ???
	if (!tm2_dev) {
		LOG_ERROR( "unable to open TMATE2 device for read");
		return false;
	}

	res = tm2_dev->hid_read(buf, 64);
	if (res <= 0) {
		LOG_ERROR( "TMATE 2 read return bytes are %i",res);
		return false;
	}

	*enc1 = buf[2] << 8 | buf[1];
	*enc2 = buf[4] << 8 | buf[3];
	*enc3 = buf[6] << 8 | buf[5];
	*keys = buf[8] << 8 | buf[7];

#ifdef TMATE2_DEBUG
	LOG_DEBUG("enc1: %i   enc2: %i  enc3: %i  keys: %i",*enc1, *enc2, *enc3, *keys);
	if ((*keys & ENCODER1_KEY_MASK) == 0x00) {
		LOG_DEBUG("ENCODER1 KEY PRESSED\n");
	}
	if ((*keys & ENCODER2_KEY_MASK) == 0x00) {
		LOG_DEBUG("ENCODER2 KEY PRESSED\n");
	}
	if ((*keys & MAIN_ENCODER_KEY_MASK) == 0x00) {
		LOG_DEBUG("MAIN ENCODER KEY PRESSED\n");
	}
	if ((*keys & F1_KEY_PRESSED) == 0x00) {
		LOG_DEBUG("F1 KEY PRESSED\n");
	}
	if ((*keys & F2_KEY_PRESSED) == 0x00) {
		LOG_DEBUG("F2 KEY PRESSED\n");
	}
	if ((*keys & F3_KEY_PRESSED) == 0x00) {
		LOG_DEBUG("F3 KEY PRESSED\n");
	}
	if ((*keys & F4_KEY_PRESSED) == 0x00) {
		LOG_DEBUG("F4 KEY PRESSED\n");
	}
	if ((*keys & F5_KEY_PRESSED) == 0x00) {
		LOG_DEBUG("F5 KEY PRESSED\n");
	}
	if ((*keys & F6_KEY_PRESSED) == 0x00) {
		LOG_DEBUG("F6 KEY PRESSED\n");
	}
#endif

	return true;
}

//sets values to the display and turns on the LED must be sent out to the USB bus minimum once in 400ms otherwise the LED and LCD turns off
int tmate2_set_lcd (unsigned char *bufout)
{
	// just write the first 44 BYTE and fill up with 0x0
	// lcdstring[0-31] LCD Segment status.
	// Use the function tmate2_set_segment to turn on the desired segment
	// lcdstring [32] Led status.
	//    Bit 0: red TUNE LOCK led (0:OFF, 1:ON)
	//    Bit 1: green USB led (0:OFF, 1: ON)
	//  lcdstring [33] “Red” backlight component [0:255];
	//  lcdstring [34] “Green” backlight component [0:255];
	//  lcdstring [35] “Blue” backlight component [0:255];
	//  lcdstring [36] LCD contrast. Set 0 for maximum contrast, 255 for minimum contrast
	//  lcdstring [37] Display refresh time in ms/10;
	//    For a refresh time of 100ms, set LCD[37]=10
	//  lcdstring [38] Main encoder increment count for each encoder click in speed 1
	//  lcdstring [39] Main encoder increment count for each encoder click in speed 2
	//  lcdstring [40] Main encoder increment count for each encoder click in speed 3
	//  lcdstring [41] Increment count to a speed 1 -> speed 2 transition
	//  lcdstring [42] Increment count to a speed 2 -> speed 3 transition
	//  lcdstring [43] Evaluation time in ms/10 for speed transition

	int res = 0;
	int i;
	unsigned char buf[65];
	buf[64]= '\0';
	buf[0] = 0x0; // report number
	for (i = 1; i <= 45; i++) {
		buf[i] = bufout[i-1];
	}
	for (i = 46; i <= 64; i++) {
		buf[i] = 0xFF;
	}

	buf[64]= '\0';
	if (tm2_dev != 0x0) {
		while (res == 0) {
			res = tm2_dev->hid_write(buf, 65);
			if (res == 0) {
				LOG_ERROR("HID WRITE return1: %i\n",res);
			}
			if (res < 0) {
				LOG_ERROR("HID WRITE return2: %i\n",res);
				tm2_dev->hid_close();
				return false;
			}
			if (res > 1) {
				tm2_dev->hid_close();
#ifdef TMATE2_DEBUG
				LOG_DEBUG("Bytes successfully written to TMATE 2: %i\n",res);
#endif
				return true;
			}
		}
		return true;
	}
	return false;
}

//sets the bits in the lcdstring to the value and did not touch the other values
void tmate2_write_main_display (unsigned char *lcdstring, int Value)
{
	//The left site of the seven segment frequency display is on lcdstring[3] and lcdstring[4]
	if (Value < 0) return;
	int number;
	int i;
	for (i = 3; i < 19; i++) { //Ensures that the used bytes are in a clear state
		lcdstring[i]=0x00;
	}
	for (i = 19; i >= 3; i--) {
		number = Value % 10 ;
		Value = Value / 10 ;
		switch (number) {
			case 0:
				lcdstring[i] |= SEG0L;
				lcdstring[i+1] |= SEG0H;
				if (Value == 0) {
					lcdstring[i] = 0x0;
					lcdstring[i+1] = 0x0;
				}
				break;
			case 1:
				lcdstring[i] |= SEG1L;
				lcdstring[i+1] |= SEG1H;
				break;
			case 2:
				lcdstring[i] |= SEG2L;
				lcdstring[i+1] |= SEG2H;
				break;
			case 3:
				lcdstring[i] |= SEG3L;
				lcdstring[i+1] |= SEG3H;
				break;
			case 4:
				lcdstring[i] |= SEG4L;
				lcdstring[i+1] |= SEG4H;
				break;
			case 5:
				lcdstring[i] |= SEG5L;
				lcdstring[i+1] |= SEG5H;
				break;
			case 6:
				lcdstring[i] |= SEG6L;
				lcdstring[i+1] |= SEG6H;
				break;
			case 7:
				lcdstring[i] |= SEG7L;
				lcdstring[i+1] |= SEG7H;
				break;
			case 8:
				lcdstring[i] |= SEG8L;
				lcdstring[i+1] |= SEG8H;
				break;
			case 9:
				lcdstring[i] |= SEG9L;
				lcdstring[i+1] |= SEG9H;
				break;
			}
		i--;
		}
}

//sets the bits in the lcdstring to the value and did not touch the other values

void tmate2_set_segment(unsigned char *lcdstring, int lcd_mask, unsigned char status)
{
	//This function set one bit of the lcdstring according to the status and returns the new value. This bit corresponds on the LCD to on segment and displays it or not.
	unsigned char mask = 0;
	int vector = 0;
	switch(lcd_mask) {
		case  0 :  //  SEG_SMETER_LINE
			vector = 2; mask = 0x0;
			break;
		case  1 :  //  SEG_SMETER_DB_MINUS
			vector = 28; mask = 0x10;
			break;
		case  2 :  //  SEG_CW_PLUS
			vector = 21;  mask = 0x01;
			break;
		case  3 :  //  SEG_CW_MINUS
			vector = 21;  mask = 0x02;
			break;
		case  4 :  //  SEG_DIG_PLUS
			vector = 21;  mask = 0x04;
			break;
		case  5 :  //  SEG_DIG_MINUS
			vector = 21;  mask = 0x08;
			break;
		case  6 :  //  SEG_UNDERLINE_9
			vector = 4;  mask = 0x08;
		break;
		case  7 :  //  SEG_UNDERLINE_8
			vector = 6;  mask = 0x08;
			break;
		case  8 :  //  SEG_UNDERLINE_7
				vector = 8;  mask = 0x08;
				break;
			case  9 :  //  SEG_UNDERLINE_6
				vector = 10;  mask = 0x08;
				break;
			case  10 :  //  SEG_UNDERLINE_5
				vector = 12;  mask = 0x08;
				break;
			case  11 :  //  SEG_UNDERLINE_4
				vector = 14;  mask = 0x08;
				break;
			case  12 :  //  SEG_UNDERLINE_3
				vector = 16;  mask = 0x08;
				break;
			case  13 :  //  SEG_UNDERLINE_2
				vector = 18;  mask = 0x08;
				break;
			case  14 :  //  SEG_UNDERLINE_1
				vector = 20;  mask = 0x08;
				break;
			case  15 :  //  SEG_DOT1
				vector = 9;  mask = 0x10;
				break;
			case  16 :  //  SEG_DOT2
				vector = 15;  mask = 0x10;
				break;
			case  17 :  //  SEG_HZ
				vector = 23;  mask = 0x01;
				break;
			case  18 :  //  SEG_E1
				vector = 0;  mask = 0x80;
				break;
			case  19 :  //  SEG_ERR
				vector = 19;  mask = 0x10;
				break;
			case  20 :  //  SEG_E2
				vector = 8;  mask = 0x10;
				break;
			case  21 :  //  SEG_LP
				vector = 0; mask = 0x02;
				break;
			case  22 :  //  SEG_ATT
				vector = 1;  mask = 0x01;
				break;
			case  23 :  //  SEG_S
				vector = 0;  mask = 0x10;
				break;
			case  24 :  //  SEG_VFO
				vector = 0;  mask = 0x20;
				break;
			case  25 :  //  SEG_NR
				vector = 0;  mask = 0x40;
				break;
			case  26 :  //  SEG_NB
				vector = 1;  mask = 0x40;
				break;
			case  27 :  //  SEG_SMETER_1
				vector = 1;  mask = 0x10;
				break;
			case  28 :  //  SEG_A
				vector = 1;  mask = 0x20;
				break;
			case  29 :  //  SEG_B
				vector = 2;  mask = 0x20;
				break;
			case  30 :  //  SEG_VOL
				vector = 1;  mask = 0x80;
				break;
			case  31 :  //  SEG_SMETER_9
				vector = 2;  mask = 0x2;
				break;
			case  32 :  //  SEG_SMETER_7
				vector = 2;  mask = 0x04;
				break;
			case  33 :  //  SEG_SMETER_5
				vector = 2;  mask = 0x08;
				break;
			case  34 :  //  SEG_SMETER_3
				vector = 2;  mask = 0x10;
				break;
			case  35 :  //  SEG_AN
				vector = 2;  mask = 0x40;
				break;
			case  36 :  //  SEG_RFG
				vector = 2;  mask = 0x80;
				break;
			case  37 :  //  SEG_SQL
				vector = 3;  mask = 0x10;
				break;
			case  38 :  //  SEG_DRV
				vector = 4;  mask = 0x10;
				break;
			case  39 :  //  SEG_SHIFT
				vector = 12;  mask = 0x10;
				break;
			case  40 :  //  SEG_LOW
				vector = 11;  mask = 0x10;
				break;
			case  41 :  //  SEG_HIGH
				vector = 10;  mask = 0x10;
				break;
			case  42 :  //  SEG_DSB
				vector = 21;  mask = 0x10;
				break;
			case  43 :  //  SEG_FM
				vector = 21;  mask = 0x20;
				break;
			case  44 :  //  SEG_USB
				vector = 21;  mask = 0x40;
				break;
			case  45 :  //  SEG_SAM
				vector = 21;  mask = 0x80;
				break;
			case  46 :  //  SEG_DRM
				vector = 22;  mask = 0x01;
				break;
			case  47 :  //  SEG_DIG
				vector = 22;  mask = 0x02;
				break;
			case  48 :  //  SEG_STEREO
				vector = 22;  mask = 0x04;
				break;
			case  49 :  //  SEG_DBM
				vector = 22;  mask = 0x10;
				break;
			case  50 :  //  SEG_CW
				vector = 22;  mask = 0x20;
				break;
			case  51 :  //  SEG_LSB
				vector = 22;  mask = 0x40;
				break;
			case  52 :  //  SEG_AM
				vector = 22;  mask = 0x80;
				break;
			case  53 :  //  SEG_SMETER_PLUS20
				vector = 9;  mask = 0x20;
				break;
			case  54 :  //  SEG_SMETER_PLUS40
				vector = 15;  mask = 0x20;
				break;
			case  55 :  //  SEG_SMETER_PLUS60
				vector = 18;  mask = 0x20;
				break;
			case  56 :  //  SEG_SMETER_10
				vector = 8;  mask = 0x20;
				break;
			case  57 :  //  SEG_SMETER_20
				vector = 10;  mask = 0x20;
				break;
			case  58 :  //  SEG_SMETER_40
				vector = 16;  mask = 0x20;
				break;
			case  59 :  //  SEG_SMETER_60
				vector = 19;  mask = 0x20;
				break;
			case  60 :  //  SEG_RX
				vector = 0;  mask = 0x04;
				break;
			case  61 :  //  SEG_TX
				vector = 0;  mask = 0x08;
				break;
			case  62 :  //  SEG_ATT_1
				vector = 31;  mask = 0x04;
				break;
			case  63 :  //  SEG_ATT_2
				vector = 31;  mask = 0x01;
				break;
			case  64 :  //  SEG_PRE
				vector = 31;  mask = 0x02;
				break;
			case  65 :  //  SEG_PRE_1
				vector = 30;  mask = 0x01;
				break;
			case  66 :  //  SEG_PRE_2
				vector = 30;  mask = 0x02;
				break;
			case  67 :  //  SEG_mW_W
				vector = 27;  mask = 0x01;
				break;
			case  68 :  //  SEG_mW_m
				vector = 28;  mask = 0x01;
				break;
			case  69 :  //  SEG_W
				vector = 20;  mask = 0x20;
				break;
			case  70 :  //  SEG_K
				vector = 25;  mask = 0x01;
				break;
			case  71 :  //  SEG_RIT
				vector = 13;  mask = 0x10;
				break;
			case  72 :  //  SEG_XIT
				vector = 14;  mask = 0x10;
				break;
			case  73 :  //  SEG_W_FM
				vector = 20;  mask = 0x10;
				break;
			case  74 :  //  SEG_NR2
				vector = 5;  mask = 0x10;
				break;
			case  75 :  //  SEG_NB2
				vector = 6;  mask = 0x10;
				break;
			case  76 :  //  SEG_AN2
				vector = 7;  mask = 0x10;
				break;
			case  77 :  //  SMETER_BAR1
				vector = 1;  mask = 0x08;
				break;
			case  78 :  //  SMETER_BAR2
				vector = 1;  mask = 0x04;
				break;
			case  79 :  //  SMETER_BAR3
				vector = 1;  mask = 0x02;
				break;
			case  80 :  //  SMETER_BAR4
				vector = 31;  mask = 0x80;
				break;
			case  81 :  //  SMETER_BAR5
				vector = 31;  mask = 0x40;
				break;
			case  82 :  //  SMETER_BAR6
				vector = 31;  mask = 0x20;
				break;
			case  83 :  //  SMETER_BAR7
				vector = 31;  mask = 0x10;
				break;
			case  84 :  //  SMETER_BAR8
				vector = 30;  mask = 0x10;
				break;
			case  85 :  //  SMETER_BAR9
				vector = 30;  mask = 0x20;
				break;
			case  86 :  //  SMETER_BAR10
				vector = 30;  mask = 0x40;
				break;
			case  87 :  //  SMETER_BAR11
				vector = 30;  mask = 0x80;
				break;
			case  88 :  //  SMETER_BAR12
				vector = 29;  mask = 0x80;
				break;
			case  89 :  //  SMETER_BAR13
				vector = 29;  mask = 0x40;
				break;
			case  90 :  //  SMETER_BAR14
				vector = 29;  mask = 0x20;
				break;
			case  91 :  //  SMETER_BAR15
				vector = 29;  mask = 0x10;
				break;
			case  92 :  //  SEG_MAIN_9A
				vector = 4;  mask = 0x01;
				break;
			case  93 :  //  SEG_MAIN_9B
				vector = 4;  mask = 0x02;
				break;
			case  94 :  //  SEG_MAIN_9C
				vector = 4;  mask = 0x04;
				break;
			case  95 :  //  SEG_MAIN_9D
				vector = 3;  mask = 0x08;
				break;
			case  96 :  //  SEG_MAIN_9E
				vector = 3;  mask = 0x04;
				break;
			case  97 :  //  SEG_MAIN_9F
				vector = 3;  mask = 0x01;
				break;
			case  98 :  //  SEG_MAIN_9G
				vector = 3;  mask = 0x02;
				break;
			case  99 :  //  SEG_MAIN_8A
				vector = 6;  mask = 0x01;
				break;
			case  100 :  //  SEG_MAIN_8B
				vector = 6;  mask = 0x02;
				break;
			case  101 :  //  SEG_MAIN_8C
				vector = 6;  mask = 0x04;
				break;
			case  102 :  //  SEG_MAIN_8D
				vector = 5;  mask = 0x08;
				break;
			case  103 :  //  SEG_MAIN_8E
				vector = 5;  mask = 0x04;
				break;
			case  104 :  //  SEG_MAIN_8F
				vector = 5;  mask = 0x01;
				break;
			case  105 :  //  SEG_MAIN_8G
				vector = 5;  mask = 0x02;
				break;
			case  106 :  //  SEG_MAIN_7A
				vector = 8;  mask = 0x01;
				break;
			case  107 :  //  SEG_MAIN_7B
				vector = 8;  mask = 0x02;
				break;
			case  108 :  //  SEG_MAIN_7C
				vector = 8;  mask = 0x04;
				break;
			case  109 :  //  SEG_MAIN_7D
				vector = 7;  mask = 0x08;
				break;
			case  110 :  //  SEG_MAIN_7E
				vector = 7;  mask = 0x04;
				break;
			case  111 :  //  SEG_MAIN_7F
				vector = 7;  mask = 0x01;
				break;
			case  112 :  //  SEG_MAIN_7G
				vector = 7;  mask = 0x02;
				break;
			case  113 :  //  SEG_MAIN_6A
				vector = 10;  mask = 0x01;
				break;
			case  114 :  //  SEG_MAIN_6B
				vector = 10;  mask = 0x02;
				break;
			case  115 :  //  SEG_MAIN_6C
				vector = 10;  mask = 0x04;
				break;
			case  116 :  //  SEG_MAIN_6D
				vector = 9;  mask = 0x08;
				break;
			case  117 :  //  SEG_MAIN_6E
				vector = 9;  mask = 0x04;
				break;
			case  118 :  //  SEG_MAIN_6F
				vector = 9;  mask = 0x01;
				break;
			case  119 :  //  SEG_MAIN_6G
				vector = 9;  mask = 0x02;
				break;
			case  120 :  //  SEG_MAIN_5A
				vector = 12;  mask = 0x01;
				break;
			case  121 :  //  SEG_MAIN_5B
				vector = 12;  mask = 0x02;
				break;
			case  122 :  //  SEG_MAIN_5C
				vector = 12;  mask = 0x04;
				break;
			case  123 :  //  SEG_MAIN_5D
				vector = 11;  mask = 0x08;
				break;
			case  124 :  //  SEG_MAIN_5E
				vector = 11;  mask = 0x04;
				break;
			case  125 :  //  SEG_MAIN_5F
				vector = 11;  mask = 0x01;
				break;
			case  126 :  //  SEG_MAIN_5G
				vector = 11;  mask = 0x02;
				break;
			case  127 :  //  SEG_MAIN_4A
				vector = 14;  mask = 0x01;
				break;
			case  128 :  //  SEG_MAIN_4B
				vector = 14;  mask = 0x02;
				break;
			case  129 :  //  SEG_MAIN_4C
				vector = 14;  mask = 0x04;
				break;
			case  130 :  //  SEG_MAIN_4D
				vector = 13;  mask = 0x08;
				break;
			case  131 :  //  SEG_MAIN_4E
				vector = 13;  mask = 0x04;
				break;
			case  132 :  //  SEG_MAIN_4F
				vector = 13;  mask = 0x01;
				break;
			case  133 :  //  SEG_MAIN_4G
				vector = 13;  mask = 0x02;
				break;
			case  134 :  //  SEG_MAIN_3A
				vector = 16;  mask = 0x01;
				break;
			case  135 :  //  SEG_MAIN_3B
				vector = 16;  mask = 0x02;
				break;
			case  136 :  //  SEG_MAIN_3C
				vector = 16;  mask = 0x04;
				break;
			case  137 :  //  SEG_MAIN_3D
				vector = 15;  mask = 0x08;
				break;
			case  138 :  //  SEG_MAIN_3E
				vector = 15;  mask = 0x04;
				break;
			case  139 :  //  SEG_MAIN_3F
				vector = 15;  mask = 0x01;
				break;
			case  140 :  //  SEG_MAIN_3G
				vector = 15;  mask = 0x02;
				break;
			case  141 :  //  SEG_MAIN_2A
				vector = 18;  mask = 0x01;
				break;
			case  142 :  //  SEG_MAIN_2B
				vector = 18;  mask = 0x02;
				break;
			case  143 :  //  SEG_MAIN_2C
				vector = 18;  mask = 0x04;
				break;
			case  144 :  //  SEG_MAIN_2D
				vector = 17;  mask = 0x08;
				break;
			case  145 :  //  SEG_MAIN_2E
				vector = 17;  mask = 0x04;
				break;
			case  146 :  //  SEG_MAIN_2F
				vector = 17;  mask = 0x01;
				break;
			case  147 :  //  SEG_MAIN_2G
				vector = 17;  mask = 0x02;
				break;
			case  148 :  //  SEG_MAIN_1A
				vector = 20;  mask = 0x01;
				break;
			case  149 :  //  SEG_MAIN_1B
				vector = 20;  mask = 0x02;
				break;
			case  150 :  //  SEG_MAIN_1C
				vector = 20;  mask = 0x04;
				break;
			case  151 :  //  SEG_MAIN_1D
				vector = 19;  mask = 0x08;
				break;
			case  152 :  //  SEG_MAIN_1E
				vector = 19;  mask = 0x04;
				break;
			case  153 :  //  SEG_MAIN_1F
				vector = 19;  mask = 0x01;
				break;
			case  154 :  //  SEG_MAIN_1G
				vector = 19;  mask = 0x02;
				break;
			case  155 :  //  SEG_SMETER_3A
				vector = 27;  mask = 0x80;
				break;
			case  156 :  //  SEG_SMETER_3B
				vector = 27;  mask = 0x40;
				break;
			case  157 :  //  SEG_SMETER_3C
				vector = 27;  mask = 0x20;
				break;
			case  158 :  //  SEG_SMETER_3D
				vector = 27;  mask = 0x10;
				break;
			case  159 :  //  SEG_SMETER_3E
				vector = 28;  mask = 0x20;
				break;
			case  160 :  //  SEG_SMETER_3F
				vector = 28;  mask = 0x80;
				break;
			case  161 :  //  SEG_SMETER_3G
				vector = 28;  mask = 0x40;
				break;
			case  162 :  //  SEG_SMETER_2A
				vector = 25;  mask = 0x80;
				break;
			case  163 :  //  SEG_SMETER_2B
				vector = 25;  mask = 0x40;
				break;
			case  164 :  //  SEG_SMETER_2C
				vector = 25;  mask = 0x20;
				break;
			case  165 :  //  SEG_SMETER_2D
				vector = 25;  mask = 0x10;
				break;
			case  166 :  //  SEG_SMETER_2E
				vector = 26;  mask = 0x20;
				break;
			case  167 :  //  SEG_SMETER_2F
				vector = 26;  mask = 0x80;
				break;
			case  168 :  //  SEG_SMETER_2G
				vector = 26;  mask = 0x40;
				break;
			case  169 :  //  SEG_SMETER_1A
				vector = 23;  mask = 0x80;
				break;
			case  170 :  //  SEG_SMETER_1B
				vector = 23;  mask = 0x40;
				break;
			case  171 :  //  SEG_SMETER_1C
				vector = 23;  mask = 0x20;
				break;
			case  172 :  //  SEG_SMETER_1D
				vector = 23;  mask = 0x10;
				break;
			case  173 :  //  SEG_SMETER_1E
				vector = 24;  mask = 0x20;
				break;
			case  174 :  //  SEG_SMETER_1F
				vector = 24;  mask = 0x80;
				break;
			case  175 :  //  SEG_SMETER_1G
				vector = 24;  mask = 0x40;
				break;
	}
	if (status == 0x0)
		lcdstring[vector] ^= mask;
	else
		lcdstring[vector] |= mask;
}

//sets the bits in the lcdstring to the value and did not touch the other values

void tmate2_write_smeter_display (unsigned char *lcdstring, int SmeterValue)
{
	//  lcdstring[23] = 0xB0; right segment low byte
	//  lcdstring[24] = 0xC0; right segment high byte
	//  lcdstring[25] = 0xB0;
	//  lcdstring[26] = 0xE0;
	//  lcdstring[27] = 0xE0;
	//  lcdstring[28] = 0x00; left segment high byte

	if (SmeterValue < 0) return;
	int number;
	int i;
	for (i = 23; i <= 28; i++) {
		number = SmeterValue % 10 ;
		SmeterValue = SmeterValue / 10 ;
		switch (number) {
			case 0:
				lcdstring[i] |= SEGSWR0L;
				lcdstring[i+1] |= SEGSWR0H;
				if (SmeterValue == 0) {
					lcdstring[i] = 0x00;
					lcdstring[i+1] = 0x00;
				}
				break;
			case 1:
				lcdstring[i] |= SEGSWR1L;
				lcdstring[i+1] |= SEGSWR1H;
				break;
			case 2:
				lcdstring[i] |= SEGSWR2L;
				lcdstring[i+1] |= SEGSWR2H;
				break;
			case 3:
				lcdstring[i] |= SEGSWR3L;
				lcdstring[i+1] |= SEGSWR3H;
				break;
			case 4:
				lcdstring[i] |= SEGSWR4L;
				lcdstring[i+1] |= SEGSWR4H;
				break;
			case 5:
				lcdstring[i] |= SEGSWR5L;
				lcdstring[i+1] |= SEGSWR5H;
				break;
			case 6:
				lcdstring[i] |= SEGSWR6L;
				lcdstring[i+1] |= SEGSWR6H;
				break;
			case 7:
				lcdstring[i] |= SEGSWR7L;
				lcdstring[i+1] |= SEGSWR7H;
				break;
			case 8:
				lcdstring[i] |= SEGSWR8L;
				lcdstring[i+1] |= SEGSWR8H;
				break;
			case 9:
				lcdstring[i] |= SEGSWR9L;
				lcdstring[i+1] |= SEGSWR9H;
				break;
			}
		i++;
	}
}

// all updates to the UI must be from main thread
// otherwise random segmentation faults may occur

void tmate2_update (void *) 
{
	FreqDispA->value( vfoA.freq );
	selrig->set_vfoA(vfoA.freq);
	FreqDispA->redraw();
}

//Continuously read and write to TMATE2

void * tmate2_thread_loop (void *d)
{
	unsigned short enc1 = 0;
	unsigned short enc2 = 0;
	unsigned short enc3 = 0;
	unsigned short keys = 0;
	unsigned short enc1old = 0;
//	unsigned short enc2old = 0;
//	unsigned short enc3old = 0;
//	unsigned short keysold = 0;
	if (!tmate2_read_value (&enc1, &enc2, &enc3, &keys)) {
		LOG_ERROR( "TMATE 2 error1");
	}
	//this loop is feeding the LED and LCD and continuously read the value from the buttons
	for (;;) {
		tmate2_write_main_display (lcdstring, vfoA.freq);
		if (!tmate2_set_lcd (lcdstring)) {
			LOG_ERROR( "TMATE 2 error2");
		}
		MilliSleep(150);
		enc1old = enc1;
//		enc2old = enc2;
//		enc3old = enc3;
//		keysold = keys;
		if (!tmate2_read_value (&enc1, &enc2, &enc3, &keys)) {
			LOG_ERROR( "TMATE 2 error3");
		}
		//Main knob
		if ( (enc1old - enc1 != 0) && (enc1old - enc1 <10000) )  {
			//there is an overflow 0 one step backwards is 65535
			//step size is transceiver dependent
			vfoA.freq = vfoA.freq + (enc1old - enc1) * atoi(progStatus.tmate2_freq_step.c_str()) ;
			Fl::awake (tmate2_update);
		}
	}
	return NULL;
}

int tmate2_open()
{
	std::string str_device = progStatus.tmate2_device;

	memset (lcdstring,0x00,LCD_STRING_SIZE); //clear
	lcdstring[LED_STATUS] |= 0x01;  //LED USB on
	lcdstring[BACKLIGTH_R] = 0;
	lcdstring[BACKLIGTH_G] = 255;
	lcdstring[BACKLIGTH_B] = 0;
	lcdstring[CONTRAST] = 0;  //0 MAX  255 MIN
	lcdstring[LCD_REFRESH_TIME] = (char)(LcdRefreshTime_ms / 10);
	lcdstring[INCR_STEP_SPEED1] = (char)1;
	lcdstring[INCR_STEP_SPEED2] = (char)2;
	lcdstring[INCR_STEP_SPEED3] = (char)5;
	lcdstring[THR_SPEED_1_2] = 45;
	lcdstring[THR_SPEED_2_3] = 50;
	lcdstring[CHANGE_SPEED_EVAL_TIME] = (char)(ChangeSpeedEvalTime_ms / 10);
	tmate2_set_segment(lcdstring, SEG_HZ, TM_ON);
	tmate2_set_segment(lcdstring, SEG_VFO, TM_ON);
	tmate2_set_segment(lcdstring, SEG_A, TM_ON);

	if (str_device == "NONE") {
		progStatus.tmate2_connected = false;
		return -1;
	}

	tm2_dev_path = paths[str_device];

	LOG_DEBUG("TMATE2 Device path: %s", tm2_dev_path.c_str());

	tm2_dev = hid_open_path(tm2_dev_path);

	if (!tm2_dev) {
		LOG_ERROR( "unable to open TMATE2 device");
		progStatus.tmate2_connected = false;
		return -1;
	}

	//start feeding the USB
	tmate_thread = new pthread_t;
	if (pthread_create(tmate_thread, NULL, tmate2_thread_loop, NULL)) {
		perror("pthread_create");
		progStatus.tmate2_connected = false;
		exit(EXIT_FAILURE);
	}

	LOG_INFO("TMATE2 device %s opened", str_device.c_str());
	progStatus.tmate2_connected = true;

	return 0;
}

void tmate2_init_hids()
{
	std::string hidstr = "NONE";
	hid_device_info *devs = 0;
	cbo_tmate2_dev->clear();       //Fl_ComboBox in xvcr_setup and as external in rigpanel.h
	paths.clear();

	if (hid_init()) {
		cbo_tmate2_dev->add(hidstr.c_str());
		return;
	}

	devs = hid_enumerate(VID, PID);  // find all Elad TM2 devices currently only one supported but lets see

	std::string dev_name = "TMATE2-A";
	while (devs) {
		LOG_INFO("\n\
			HID           : %s\n\
			vendor id     : %04hx\n\
			product id    : %04hx\n\
			Manufacturer  : %s\n\
			Product       : %s\n\
			Serial Number : %s\n\
			Release       : %hx",
			dev_name.c_str(),
			devs->vendor_id,
			devs->product_id,
			devs->str_manufacturer_string.c_str(),
			devs->str_product_string.c_str(),
			devs->str_serial_number.c_str(),
			devs->release_number);

		hidstr.append("|").append(dev_name);
		paths[dev_name] = devs->path;
		++dev_name[7]; // increment A->B->C...
		devs = devs->next;
	}
	cbo_tmate2_dev->add(hidstr.c_str());
	cbo_tmate2_dev->value(progStatus.tmate2_device.c_str());

	cbo_tmate2_freq_step->value(progStatus.tmate2_freq_step.c_str());
}





