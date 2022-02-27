// ----------------------------------------------------------------------------
// fsk.cxx  --  FSK signal generator
//
// Copyright (C) 2021
//		Dave Freese, W1HKJ
//
// This file is part of flrig.
//
// flrig is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// flrig is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with fldigi.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include <config.h>

#include "fsk.h"
#include "cwio.h"
#include "fskioUI.h"
#include "serial.h"
#include "support.h"
#include "status.h"
#include "util.h"

#ifdef __WIN32__
#include "compat.h"
#endif

//#define FSK_DEBUG

extern int errno;

char FSK::letters[32] = {
	'\0',	'E',	'\n',	'A',	' ',	'S',	'I',	'U',
	'\r',	'D',	'R',	'J',	'N',	'F',	'C',	'K',
	'T',	'Z',	'L',	'W',	'H',	'Y',	'P',	'Q',
	'O',	'B',	'G',	' ',	'M',	'X',	'V',	' '
};

/*
 * U.S. version of the figures case.
 */
char FSK::figures[32] = {
	'\0',	'3',	'\n',	'-',	' ',	'\a',	'8',	'7',
	'\r',	'$',	'4',	'\'',	',',	'!',	':',	'(',
	'5',	'"',	')',	'2',	'#',	'6',	'0',	'1',
	'9',	'?',	'&',	' ',	'.',	'/',	';',	' '
};

/*
static const char * FSK::ascii[256] = {
	"<NUL>", "<SOH>", "<STX>", "<ETX>", "<EOT>", "<ENQ>", "<ACK>", "<BEL>",
	"<BS>",  "<TAB>", "\n",    "<VT>",  "<FF>",  "<CR>",  "<SO>",  "<SI>",
	"<DLE>", "<DC1>", "<DC2>", "<DC3>", "<DC4>", "<NAK>", "<SYN>", "<ETB>",
	"<CAN>", "<EM> ", "<SUB>", "<FIG>", "<FS>",  "<GS>",  "<RS>",  "<LTR>",
	" ", "!", "\"","#", "$", "%", "&", "\'",
	"(", ")", "*", "+", ",", "-", ".", "/",
	"0", "1", "2", "3", "4", "5", "6", "7",
	"8", "9", ":", ";", "<", "=", ">", "?",
	"@", "A", "B", "C", "D", "E", "F", "G",
	"H", "I", "J", "K", "L", "M", "N", "O",
	"P", "Q", "R", "S", "T", "U", "V", "W",
	"X", "Y", "Z", "[", "\\","]", "^", "_",
	"`", "a", "b", "c", "d", "e", "f", "g",
	"h", "i", "j", "k", "l", "m", "n", "o",
	"p", "q", "r", "s", "t", "u", "v", "w",
	"x", "y", "z", "{", "|", "}", "~", "<DEL>",
	"<128>", "<129>", "<130>", "<131>", "<132>", "<133>", "<134>", "<135>",
	"<136>", "<137>", "<138>", "<139>", "<140>", "<141>", "<142>", "<143>",
	"<144>", "<145>", "<146>", "<147>", "<148>", "<149>", "<150>", "<151>",
	"<152>", "<153>", "<154>", "<155>", "<156>", "<157>", "<158>", "<159>",
	"<160>", "<161>", "<162>", "<163>", "<164>", "<165>", "<166>", "<167>",
	"<168>", "<169>", "<170>", "<171>", "<172>", "<173>", "<174>", "<175>",
	"<176>", "<177>", "<178>", "<179>", "<180>", "<181>", "<182>", "<183>",
	"<184>", "<185>", "<186>", "<187>", "<188>", "<189>", "<190>", "<191>",
	"<192>", "<193>", "<194>", "<195>", "<196>", "<197>", "<198>", "<199>",
	"<200>", "<201>", "<202>", "<203>", "<204>", "<205>", "<206>", "<207>",
	"<208>", "<209>", "<210>", "<211>", "<212>", "<213>", "<214>", "<215>",
	"<216>", "<217>", "<218>", "<219>", "<220>", "<221>", "<222>", "<223>",
	"<224>", "<225>", "<226>", "<227>", "<228>", "<229>", "<230>", "<231>",
	"<232>", "<233>", "<234>", "<235>", "<236>", "<237>", "<238>", "<239>",
	"<240>", "<241>", "<242>", "<243>", "<244>", "<245>", "<246>", "<247>",
	"<248>", "<249>", "<250>", "<251>", "<252>", "<253>", "<254>", "<255>"
};
*/

pthread_mutex_t fsk_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fskio_text_mutex = PTHREAD_MUTEX_INITIALIZER;

Cserial *FSK_serial = (Cserial *)0;
FSK *fsk_instance = (FSK *)0;

FSK::FSK()
{
	str_buff.clear();

	start_bits = 0;
	stop_bits  = 0;
	chr_bits   = 0;
	chr_out    = 0;
	shift      = 0;
	shift_state  = FSK_FIGURES;
	_shift_on_space = false;
	idles = 3;

	BITLEN = 0.022;

	init_fsk_thread();

	shared_port = false;
}

FSK::~FSK()
{
	exit_fsk_thread();
	if (shared_port)
		return;
	if (FSK_serial)
		FSK_serial->ClosePort();
}

int FSK_open_port()
{
	if (progStatus.FSK_SHARED != FSK_USES_FSK)
		return 0;
	if (!FSK_serial) return 0;

	FSK_serial->Device(progStatus.FSK_PORT);
	FSK_serial->OpenPort();
	return (FSK_serial->IsOpen());
}

void FSK_close_port()
{
	if (!FSK_serial) return;
	FSK_serial->ClosePort();
}

void FSK_reset_port()
{
}

void FSK::fsk_shares_port(Cserial *shared_device)
{
	shared_port = true;
//	fsk_serial = shared_device;
}

void update_fsk_txt_to_send(void *v)
{
	FSK *fsk = (FSK *)v;
	FSK_txt_to_send->value(fsk->str_buff.c_str());
}

void btn_fskioSEND_ON(void *v)
{
	btn_fskioSEND->value(1);
}

void btn_fskioSEND_OFF(void *v)
{
	btn_fskioSEND->value(0);
}

bool FSK::sending() {
	{
		guard_lock lck(&fskio_text_mutex);
		str_buff = FSK_txt_to_send->value();
	}
	if (str_buff[0] == '[') {
		str_buff.erase(0,1);
		Fl::awake(update_fsk_txt_to_send, this);
		Fl::awake(btn_fskioSEND_ON, this);
		FSK_send_text(1);
		idles = 3;
		return 0;
	} else if (str_buff[0] == ']') {
		str_buff.clear();
		Fl::awake(update_fsk_txt_to_send, this);
		Fl::awake(btn_fskioSEND_OFF, this);
		FSK_send_text(0);
		return 0;
	}
	return btn_fskioSEND->value();
}

void FSK::fsk_out (bool state) {
	Cserial *serial = (Cserial *)0;
	switch (progStatus.FSK_SHARED) {
		default : break;
		case FSK_USES_FSK: serial = FSK_serial; break;
		case FSK_USES_CAT: serial = RigSerial; break;
		case FSK_USES_AUX: serial = AuxSerial; break;
		case FSK_USES_SEP: serial = SepSerial; break;
		case FSK_USES_CWIO: serial = cwio_serial; break;
	}
	if (!serial)
		return;

	if (progStatus.FSK_INVERTED) state = !state;

	switch (progStatus.FSK_KEYLINE) {
		case 1: serial->setRTS(state); break;
		case 2: serial->setDTR(state); break;
		default: break;
	}
}

int FSK::baudot_enc(int data) {
	data &= 0xFF;
	int i;

	if (islower(data))
		data = toupper(data);

	if (data == ' ')  // always force space to be a LETTERS char
		return FSK_LETTERS | 4;

	for (i = 0; i < 32; i++) {
		if (data == letters[i]) {
			return (i | FSK_LETTERS);
		}
		if (data == figures[i]) {
			return (i | FSK_FIGURES);
		}
	}
	return shift_state | 4;
}

// return current tick time in seconds
double FSK::now()
{
	static struct timeval t1;
	gettimeofday(&t1, NULL);
	return t1.tv_sec + t1.tv_usec / 1e6;
}

int FSK::sleep (double sleep_time)
{
	struct timespec tv;
	double start_at = now();
	double end_at = start_at + sleep_time;
	double delay = sleep_time - 0.005;
	tv.tv_sec = (time_t) delay;
	tv.tv_nsec = (long) ((delay - tv.tv_sec) * 1e+9);
	int rval = 0;
#ifdef __WIN32__
	timeBeginPeriod(1);
	end_at -= 0.0005;
#endif
	while (1) {
		rval = nanosleep (&tv, &tv);
		if (rval == 0)
			break;
		else if (errno == EINTR)
			continue;
		else
			return rval;
	}
	while (now() < end_at);
#ifdef __WIN32__
	timeEndPeriod(1);
#endif
	return 0;
}

#ifdef FSK_DEBUG
FILE *ftiming = (FILE *)0;
#endif

void FSK::send_baudot(int ch)
{
	double t1;
	if (ch == LTRS) shift_state = FSK_LETTERS;
	else if (ch == FIGS) shift_state = FSK_FIGURES;

	t1 = now();
	fsk_out(FSK_SPACE);
	sleep(BITLEN - (now() - t1));

#ifdef FSK_DEBUG
	fprintf(ftiming, "%f, %f, %f\n", BITLEN, now() - t1, now() - t1 - BITLEN);
#endif
	t1 = now();
	fsk_out((ch & 0x01) == 0x01 ? FSK_MARK : FSK_SPACE);
	sleep(BITLEN - (now() - t1));

#ifdef FSK_DEBUG
	fprintf(ftiming, "%f, %f, %f\n", BITLEN, now() - t1, now() - t1 - BITLEN);
#endif
	t1 = now();
	fsk_out((ch & 0x02) == 0x02 ? FSK_MARK : FSK_SPACE);
	sleep(BITLEN - (now() - t1));

#ifdef FSK_DEBUG
	fprintf(ftiming, "%f, %f, %f\n", BITLEN, now() - t1, now() - t1 - BITLEN);
#endif
	t1 = now();
	fsk_out((ch & 0x04) == 0x04 ? FSK_MARK : FSK_SPACE);
	sleep(BITLEN - (now() - t1));

#ifdef FSK_DEBUG
	fprintf(ftiming, "%f, %f, %f\n", BITLEN, now() - t1, now() - t1 - BITLEN);
#endif
	t1 = now();
	fsk_out((ch & 0x08) == 0x08 ? FSK_MARK : FSK_SPACE);
	sleep(BITLEN - (now() - t1));

#ifdef FSK_DEBUG
	fprintf(ftiming, "%f, %f, %f\n", BITLEN, now() - t1, now() - t1 - BITLEN);
#endif
	t1 = now();
	fsk_out((ch & 0x10) == 0x10 ? FSK_MARK : FSK_SPACE);
	sleep(BITLEN - (now() - t1));

#ifdef FSK_DEBUG
	fprintf(ftiming, "%f, %f, %f\n", BITLEN, now() - t1, now() - t1 - BITLEN);
#endif
	t1 = now();
	fsk_out(FSK_MARK);
	sleep(BITLEN * (progStatus.FSK_STOPBITS ? 1.5 : 2.0) - (now() - t1) );

#ifdef FSK_DEBUG
	fprintf(ftiming, "%f, %f, %f\n",
		BITLEN * 1.5,
		now() - t1,
		now() - t1 - BITLEN * (progStatus.FSK_STOPBITS ? 1.5 : 2.0));
#endif
}


int FSK::callback_method()
{
#ifdef FSK_DEBUG
if (!ftiming) {
	ftiming = fopen("timing.txt", "w");
}
#endif

	if (sending()) {
		if (str_buff.empty() || idles) {
//std::cout << "LTRS" << std::endl;
			send_baudot(LTRS);
			if (idles) idles--;
		} else {
//std::cout << char(str_buff[0]) << std::endl;
			chr_out = baudot_enc(str_buff[0]);
			if ((chr_out & 0x300) != shift_state) {
				shift_state = chr_out & 0x300;
				if (shift_state == FSK_LETTERS) {
//std::cout << "LTRS" << std::endl;
					send_baudot(LTRS);
				} else {
//std::cout << "FIGS" << std::endl;
					send_baudot(FIGS);
				}
			}
			str_buff.erase(0,1);
			Fl::awake(update_fsk_txt_to_send, this);
			send_baudot(chr_out & 0x1F);
		}
	} else
		MilliSleep(BITLEN*1000L);
//		sleep(BITLEN);
	return 0;
}

void *fsk_loop(void *data)
{
	FSK *fsk = (FSK *)data;
	while (1) {
		fsk->callback_method();
		{
			guard_lock tlock (&fsk_mutex);
			if (fsk->fsk_loop_terminate) goto _exit;
		}
	}
_exit:
	return NULL;
}

int FSK::init_fsk_thread()
{
	fsk_loop_terminate = false;

	if(pthread_mutex_init(&fsk_mutex, NULL)) {
		LOG_ERROR("FSK pthread_mutex_init failed");
		return 0;
	}
	if (pthread_create(&fsk_thread, NULL, fsk_loop, this)) {
		LOG_ERROR("FSK timer thread create failed");
		return 0;
	}
	return 1;
}

void FSK::exit_fsk_thread()
{
	{
		guard_lock tlock (&fsk_mutex);
		fsk_loop_terminate = true;
		MilliSleep(50);
	}
	pthread_join(fsk_thread, NULL);

	fsk_loop_terminate = false;
}

int  FSK_start_thread() {
	if (fsk_instance)
		return 0;
	try {
		fsk_instance = new FSK;
	} catch (const char *error) {
		std::cerr << error << '\n';
		debug::stop();
		return 1;
	}
	return 0;
}

void FSK_stop_thread() {
}

static std::string FSK_new_text;

void FSK_add(std::string txt)
{
	guard_lock lck(&fskio_text_mutex);
	FSK_new_text = FSK_txt_to_send->value();
	FSK_new_text.append(txt);
	FSK_txt_to_send->value(FSK_new_text.c_str());
	FSK_txt_to_send->redraw();
}

int FSK_process = 0; // RX state

void FSK_send_text(bool state) // state == 1 (xmt), 0 (rcv)
{
	if (FSK_process == state) return;
	FSK_process = state;
	doPTT(FSK_process);
}

void FSK_clear_text()
{
	FSK_txt_to_send->value("");
}

void FSK_msg_cb(int n)
{
}

void FSK_label_cb(int n)
{
}

void FSK_exec_msg(int n)
{
	if (!FSK_editor) FSK_editor = FSK_make_message_editor();
	if ((Fl::event_state() & FL_CTRL) == FL_CTRL) {
		for (int n = 0; n < 12; n++) {
			FSK_edit_label[n]->value(progStatus.FSK_labels[n].c_str());
			FSK_edit_msg[n]->value(progStatus.FSK_msgs[n].c_str());
		}
		FSK_editor->show();
		return;
	}
	FSK_add(progStatus.FSK_msgs[n]);
}

void FSK_cancel_edit()
{
	FSK_editor->hide();
}

void FSK_apply_edit()
{
	for (int n = 0; n < 12; n++) {
		progStatus.FSK_labels[n] = FSK_edit_label[n]->value();
		progStatus.FSK_msgs[n] = FSK_edit_msg[n]->value();
		FSK_btn_msg[n]->label(progStatus.FSK_labels[n].c_str());
		FSK_btn_msg[n]->redraw_label();
	}
}

void FSK_done_edit()
{
	FSK_editor->hide();
}

// Alt-P pause transmit
// Alt-S start sending text
// F1 - F12 same as function-button mouse press

void FSK_control_function_keys()
{
	int key = Fl::event_key();
	int state = Fl::event_state();

	if (state & FL_ALT) {
		if (key == 'p') {
			btn_fskioSEND->value(0);
			btn_fskioSEND->redraw();
			FSK_send_text(false);
			return;
		}
		if (key == 's') {
			btn_fskioSEND->value(1);
			btn_fskioSEND->redraw();
			FSK_send_text(true);
			return;
		}
		if (key == 'c') {
			FSK_txt_to_send->value("");
			return;
		}
	}
	if ((key >= FL_F) && (key <= FL_F_Last)) {
		FSK_exec_msg( key - FL_F - 1);
	}
}

void FSK_open_config()
{
	if (!FSK_configure) FSK_configure = fskio_config_dialog();
	switch ( progStatus.FSK_SHARED) {
		default:
		case FSK_USES_FSK:
			select_fskioPORT->activate();
			btn_fskioCONNECT->activate();
			btn_fskioCAT->activate();
			btn_fskioAUX->activate();
			btn_fskioSEP->activate();
			btn_fskioSHARED->activate();
			break;
		case FSK_USES_AUX:
			select_fskioPORT->deactivate();
			btn_fskioCONNECT->deactivate();
			btn_fskioCAT->deactivate();
			btn_fskioAUX->activate();
			btn_fskioSEP->deactivate();
			btn_fskioSHARED->deactivate();
			break;
		case FSK_USES_CAT:
			select_fskioPORT->deactivate();
			btn_fskioCONNECT->deactivate();
			btn_fskioCAT->activate();
			btn_fskioAUX->deactivate();
			btn_fskioSEP->deactivate();
			btn_fskioSHARED->deactivate();
			break;
		case FSK_USES_SEP:
			select_fskioPORT->deactivate();
			btn_fskioCONNECT->deactivate();
			btn_fskioCAT->deactivate();
			btn_fskioAUX->deactivate();
			btn_fskioSEP->activate();
			btn_fskioSHARED->deactivate();
			break;
		case FSK_USES_CWIO:
			select_fskioPORT->deactivate();
			btn_fskioCONNECT->deactivate();
			btn_fskioCAT->deactivate();
			btn_fskioAUX->deactivate();
			btn_fskioSEP->deactivate();
			btn_fskioSHARED->activate();
			break;
	}
	FSK_configure->show();
}
