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
// This interface is based on the cmedia file and uses hidapi to
// communicate with the Elard TMATE 2 device
//
// adapt frequency step size to your transceiver in the setting menu
//
// ----------------------------------------------------------------------------

#ifndef TMATE2_H
#define TMATE2_H

//TMate 2 USB device parameters
#define VID              0x1721
#define PID              0x0614
#define LCD_STRING_SIZE  44
//definitions to be compatible to the TMATE 2 SDK
#define TM_ON  	0x01
#define TM_OFF 	0x00

//used for main segments
#define SEG0L            0xD
#define SEG0H            0x7
#define SEG1L            0x0
#define SEG1H            0x6
#define SEG2L            0xE
#define SEG2H            0x3
#define SEG3L            0xA
#define SEG3H            0x7
#define SEG4L            0x3
#define SEG4H            0x6
#define SEG5L            0xB
#define SEG5H            0x5
#define SEG6L            0xF
#define SEG6H            0x5
#define SEG7L            0x0
#define SEG7H            0x7
#define SEG8L            0xF
#define SEG8H            0x7
#define SEG9L            0xB
#define SEG9H            0x7
#define SEGAL            0x7
#define SEGAH            0x7
#define SEGBL            0xF
#define SEGBH            0x4
#define SEGCL            0xD
#define SEGCH            0x1
#define SEGDL            0xE
#define SEGDH            0x6
#define SEGEL            0xF
#define SEGEH            0x1
#define SEGFL            0x7
#define SEGFH            0x1

//used for smeter segments
#define SEGSWR0L         0xF0
#define SEGSWR0H         0xA0
#define SEGSWR1L         0x60
#define SEGSWR1H         0x00
#define SEGSWR2L         0xD0
#define SEGSWR2H         0x60
#define SEGSWR3L         0xF0
#define SEGSWR3H         0x40
#define SEGSWR4L         0x60
#define SEGSWR4H         0xD0
#define SEGSWR5L         0xB0
#define SEGSWR5H         0xC0
#define SEGSWR6L         0xB0
#define SEGSWR6H         0xE0
#define SEGSWR7L         0xE0
#define SEGSWR7H         0x00
#define SEGSWR8L         0xF0
#define SEGSWR8H         0xE0
#define SEGSWR9L         0xE0
#define SEGSWR9H         0xC0
#define SEGSWRAL         0xE0
#define SEGSWRAH         0xE0
#define SEGSWRBL         0x30
#define SEGSWRBH         0xE0
#define SEGSWRCL         0x90
#define SEGSWRCH         0xA0
#define SEGSWRDL         0x70
#define SEGSWRDH         0x60
#define SEGSWREL         0x90
#define SEGSWREH         0xE0
#define SEGSWRFL         0x80
#define SEGSWRFH         0xE0

#define ENCODER1_KEY_MASK		0x0100
#define ENCODER2_KEY_MASK		0x0080
#define MAIN_ENCODER_KEY_MASK   0x0040

#define F1_KEY_PRESSED			0x0001
#define F2_KEY_PRESSED			0x0002
#define F3_KEY_PRESSED			0x0004
#define F4_KEY_PRESSED			0x0008
#define F5_KEY_PRESSED			0x0010
#define F6_KEY_PRESSED			0x0020

#define SEG_SMETER_LINE			0
#define SEG_SMETER_DB_MINUS		1
#define SEG_CW_PLUS				2
#define SEG_CW_MINUS         	3
#define SEG_DIG_PLUS	      	4
#define SEG_DIG_MINUS	      	5
#define SEG_UNDERLINE_9      	6
#define	SEG_UNDERLINE_8	   		7
#define	SEG_UNDERLINE_7	   		8
#define	SEG_UNDERLINE_6	   		9
#define	SEG_UNDERLINE_5	  		10
#define	SEG_UNDERLINE_4	  		11
#define	SEG_UNDERLINE_3	  		12
#define	SEG_UNDERLINE_2	  		13
#define	SEG_UNDERLINE_1	  		14
#define	SEG_DOT1			15
#define	SEG_DOT2			16
#define	SEG_HZ              17
#define	SEG_E1			    18
#define	SEG_ERR			    19
#define	SEG_E2			    20
#define	SEG_LP			    21
#define	SEG_ATT			    22
#define	SEG_S			    23
#define	SEG_VFO			    24
#define	SEG_NR			    25
#define	SEG_NB			    26
#define	SEG_SMETER_1	    27
#define	SEG_A				28
#define	SEG_B			    29
#define	SEG_VOL			    30
#define	SEG_SMETER_9        31
#define	SEG_SMETER_7		32
#define	SEG_SMETER_5		33
#define	SEG_SMETER_3		34
#define	SEG_AN			    35
#define	SEG_RFG			    36
#define	SEG_SQL			    37
#define	SEG_DRV			    38
#define	SEG_SHIFT		    39
#define	SEG_LOW			    40
#define	SEG_HIGH		    41
#define	SEG_DSB			    42
#define	SEG_FM			    43
#define	SEG_USB			    44
#define	SEG_SAM			    45
#define	SEG_DRM			    46
#define	SEG_DIG			    47
#define	SEG_STEREO		    48
#define	SEG_DBM			    49
#define	SEG_CW			    50
#define	SEG_LSB			    51
#define	SEG_AM			    52
#define	SEG_SMETER_PLUS20	53
#define	SEG_SMETER_PLUS40	54
#define	SEG_SMETER_PLUS60	55
#define	SEG_SMETER_10		56
#define	SEG_SMETER_20		57
#define	SEG_SMETER_40		58
#define	SEG_SMETER_60		59
#define	SEG_RX			    60
#define	SEG_TX			    61
#define	SEG_ATT_1		    62
#define	SEG_ATT_2		    63
#define	SEG_PRE			    64
#define	SEG_PRE_1		    65
#define	SEG_PRE_2		    66
#define	SEG_mW_W		    67
#define	SEG_mW_m		    68
#define	SEG_W			    69
#define	SEG_K			    70
#define	SEG_RIT			    71
#define	SEG_XIT			    72
#define	SEG_W_FM		    73
#define	SEG_NR2			    74
#define	SEG_NB2			    75
#define	SEG_AN2			    76
#define	SMETER_BAR1	        77
#define	SMETER_BAR2	        78
#define	SMETER_BAR3	        79
#define	SMETER_BAR4	        80
#define	SMETER_BAR5	        81
#define	SMETER_BAR6	        82
#define	SMETER_BAR7	        83
#define	SMETER_BAR8	        84
#define	SMETER_BAR9	        85
#define	SMETER_BAR10	    86
#define	SMETER_BAR11	    87
#define	SMETER_BAR12	    88
#define	SMETER_BAR13	    89
#define	SMETER_BAR14	    90
#define	SMETER_BAR15	    91
#define SEG_MAIN_9A         92
#define SEG_MAIN_9B         93
#define SEG_MAIN_9C         94
#define SEG_MAIN_9D         95
#define SEG_MAIN_9E         96
#define SEG_MAIN_9F         97
#define SEG_MAIN_9G         98

#define SEG_MAIN_8A          99
#define SEG_MAIN_8B          100
#define SEG_MAIN_8C          101
#define SEG_MAIN_8D          102
#define SEG_MAIN_8E          103
#define SEG_MAIN_8F          104
#define SEG_MAIN_8G          105

#define SEG_MAIN_7A          106
#define SEG_MAIN_7B          107
#define SEG_MAIN_7C          108
#define SEG_MAIN_7D          109
#define SEG_MAIN_7E          110
#define SEG_MAIN_7F          111
#define SEG_MAIN_7G          112

#define SEG_MAIN_6A          113
#define SEG_MAIN_6B          114
#define SEG_MAIN_6C          115
#define SEG_MAIN_6D          116
#define SEG_MAIN_6E          117
#define SEG_MAIN_6F          118
#define SEG_MAIN_6G          119

#define SEG_MAIN_5A          120
#define SEG_MAIN_5B          121
#define SEG_MAIN_5C          122
#define SEG_MAIN_5D          123
#define SEG_MAIN_5E          124
#define SEG_MAIN_5F          125
#define SEG_MAIN_5G          126

#define SEG_MAIN_4A          127
#define SEG_MAIN_4B          128
#define SEG_MAIN_4C          129
#define SEG_MAIN_4D          130
#define SEG_MAIN_4E          131
#define SEG_MAIN_4F          132
#define SEG_MAIN_4G          133

#define SEG_MAIN_3A          134
#define SEG_MAIN_3B          135
#define SEG_MAIN_3C          136
#define SEG_MAIN_3D          137
#define SEG_MAIN_3E          138
#define SEG_MAIN_3F          139
#define SEG_MAIN_3G          140

#define SEG_MAIN_2A          141
#define SEG_MAIN_2B          142
#define SEG_MAIN_2C          143
#define SEG_MAIN_2D          144
#define SEG_MAIN_2E          145
#define SEG_MAIN_2F          146
#define SEG_MAIN_2G          147

#define SEG_MAIN_1A          148
#define SEG_MAIN_1B          149
#define SEG_MAIN_1C          150
#define SEG_MAIN_1D          151
#define SEG_MAIN_1E          152
#define SEG_MAIN_1F          153
#define SEG_MAIN_1G          154

#define SEG_SMETER_3A        155
#define SEG_SMETER_3B        156
#define SEG_SMETER_3C        157
#define SEG_SMETER_3D        158
#define SEG_SMETER_3E        159
#define SEG_SMETER_3F        160
#define SEG_SMETER_3G        161

#define SEG_SMETER_2A        162
#define SEG_SMETER_2B        163
#define SEG_SMETER_2C        164
#define SEG_SMETER_2D        165
#define SEG_SMETER_2E        166
#define SEG_SMETER_2F        167
#define SEG_SMETER_2G        168
#define SEG_SMETER_1A        169
#define SEG_SMETER_1B        170
#define SEG_SMETER_1C        171
#define SEG_SMETER_1D        172
#define SEG_SMETER_1E        173
#define SEG_SMETER_1F        174
#define SEG_SMETER_1G        175

#define LED_STATUS               32
#define BACKLIGTH_R              33
#define BACKLIGTH_G              34
#define BACKLIGTH_B              35
#define CONTRAST                 36
#define LCD_REFRESH_TIME         37
#define INCR_STEP_SPEED1         38
#define INCR_STEP_SPEED2         39
#define INCR_STEP_SPEED3         40
#define THR_SPEED_1_2            41
#define THR_SPEED_2_3            42
#define CHANGE_SPEED_EVAL_TIME   43

extern void tmate2_init_hids();
extern int  tmate2_open();
extern void * tmate2_thread_loop (void *d);
extern void tmate2_write_smeter_display (unsigned char *lcdstring, int SmeterValue);
extern void tmate2_set_segment(unsigned char *lcdstring, int lcd_mask, unsigned char status);
extern void tmate2_write_main_display (unsigned char *lcdstring, int Value);
extern int  tmate2_set_lcd (unsigned char *bufout);
extern int  tmate2_read_value (unsigned short *enc1, unsigned short *enc2, unsigned short *enc3, unsigned short *keys);
extern void tmate2_close();
extern int  tmate2_close_connection(void);

#endif
