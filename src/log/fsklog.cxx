// ----------------------------------------------------------------------------
// Copyright (C) 2022
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

#include <FL/Fl.H>
#include <FL/x.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Native_File_Chooser.H>

#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef __WIN32__
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>
#include <glob.h>
#endif

#include "dialogs.h"
#include "rigs.h"
#include "util.h"
#include "debug.h"
#include "serial.h"
#include "support.h"
#include "rigpanel.h"
#include "rigbase.h"
#include "font_browser.h"
#include "ui.h"
#include "status.h"
#include "rig.h"
#include "socket_io.h"
#include "rigpanel.h"
#include "gettext.h"

#include "fskioUI.h"
#include "fsklog.h"

// =====================================================================
// fsklogbook support code
// =====================================================================

bool fsklog_editing = false;
int  fsklog_edit_nbr = 0;

bool fsklog_changed = false;
bool fsklog_is_open = false;

void fsklog_set_edit(bool on)
{
	fsklog_editing = on;
	if (on) {
		btn_fsklog_edit_entry->label("Delete");
		btn_fsklog_edit_entry->redraw_label();
		btn_fsklog_clear_qso->label("Cancel");
		btn_fsklog_clear_qso->redraw_label();
	} else {
		btn_fsklog_edit_entry->label("Edit");
		btn_fsklog_edit_entry->redraw_label();
		btn_fsklog_clear_qso->label("Clear");
		btn_fsklog_clear_qso->redraw_label();
	}
}

bool fsklog_compare( int &dir, int fld, std::string &s1, std::string &s2) {
	size_t p1 = 0, p2 = 0;
	for (int n = 0; n < fld; n++) {
		p1 = s1.find('\t', p1 + 1);
		p2 = s2.find('\t', p2 + 1);
	}
	if (dir == 1) return (s2.substr(p2) < s1.substr(p1));
	return (s2.substr(p2) > s1.substr(p1));
}

bool fsklog_freq_compare( int &dir, int fld, std::string &s1, std::string &s2) {
	size_t p1 = 0, p2 = 0;
	for (int n = 0; n < fld; n++) {
		p1 = s1.find('\t', p1 + 1);
		p2 = s2.find('\t', p2 + 1);
	}
	float f1 = atof(s1.substr(p1).c_str());
	float f2 = atof(s2.substr(p2).c_str());

	if (dir == 1) return (f2 < f1);
	return (f2 > f1);
}

static int dtdir = 1;
static int dtpos = 0;
void fsklog_sort_by_datetime() {
	if (fsklog_editing) return;
	size_t nbr = brwsr_fsklog_entries->size();
	if (nbr == 0) return;
	std::string entries[nbr];
	for (size_t n = 0; n < nbr; n++) entries[n] = brwsr_fsklog_entries->text(n+1);
	std::string temp;
	if (nbr > 1) {
		for (size_t n = 0; n < nbr - 1; n++) {
			for (size_t j = n + 1; j < nbr; j++) {
				if (fsklog_compare (dtdir, dtpos, entries[n], entries[j])) {
					temp = entries[j];
					entries[j] = entries[n];
					entries[n] = temp;
				}
			}
		}
	}
	brwsr_fsklog_entries->clear();
	for (size_t i = 0; i < nbr; i++)
		brwsr_fsklog_entries->add(entries[i].c_str());
	brwsr_fsklog_entries->redraw();
	if (dtdir == 1) dtdir = -1;
	else dtdir = 1;
}

static int freqdir = 1;
static int freqpos = 2;
void fsklog_sort_by_freq() {
	if (fsklog_editing) return;
	size_t nbr = brwsr_fsklog_entries->size();
	if (nbr == 0) return;
	std::string entries[nbr];
	for (size_t n = 0; n < nbr; n++) entries[n] = brwsr_fsklog_entries->text(n+1);
	std::string temp;
	if (nbr > 1) {
		for (size_t n = 0; n < nbr - 1; n++) {
			for (size_t j = n + 1; j < nbr; j++) {
				if (fsklog_freq_compare (freqdir, freqpos, entries[n], entries[j])) {
					temp = entries[j];
					entries[j] = entries[n];
					entries[n] = temp;
				}
			}
		}
	}
	brwsr_fsklog_entries->clear();
	for (size_t i = 0; i < nbr; i++)
		brwsr_fsklog_entries->add(entries[i].c_str());
	brwsr_fsklog_entries->redraw();
	if (freqdir == 1) freqdir = -1;
	else freqdir = 1;
}

static int calldir = 1;
static int callpos = 3;
void fsklog_sort_by_call() {
	if (fsklog_editing) return;
	size_t nbr = brwsr_fsklog_entries->size();
	if (nbr == 0) return;
	std::string entries[nbr];
	for (size_t n = 0; n < nbr; n++) entries[n] = brwsr_fsklog_entries->text(n+1);
	std::string temp;
	if (nbr > 1) {
		for (size_t n = 0; n < nbr - 1; n++) {
			for (size_t j = n + 1; j < nbr; j++) {
				if (fsklog_compare (calldir, callpos, entries[n], entries[j])) {
					temp = entries[j];
					entries[j] = entries[n];
					entries[n] = temp;
				}
			}
		}
	}
	brwsr_fsklog_entries->clear();
	for (size_t i = 0; i < nbr; i++)
		brwsr_fsklog_entries->add(entries[i].c_str());
	brwsr_fsklog_entries->redraw();
	if (calldir == 1) calldir = -1;
	else calldir = 1;
}

static int nbrdir = 1;
static int nbrpos = 7;
void fsklog_sort_by_nbr() {
	if (fsklog_editing) return;
	size_t nbr = brwsr_fsklog_entries->size();
	if (nbr == 0) return;
	std::string entries[nbr];
	for (size_t n = 0; n < nbr; n++) entries[n] = brwsr_fsklog_entries->text(n+1);
	std::string temp;
	if (nbr > 1) {
		for (size_t n = 0; n < nbr - 2; n++) {
			for (size_t j = n + 1; j < nbr - 1; j++) {
				if (fsklog_compare (nbrdir, nbrpos, entries[n], entries[j])) {
					temp = entries[j];
					entries[j] = entries[n];
					entries[n] = temp;
				}
			}
		}
	}
	brwsr_fsklog_entries->clear();
	for (size_t i = 0; i < nbr; i++)
		brwsr_fsklog_entries->add(entries[i].c_str());
	brwsr_fsklog_entries->redraw();
	if (nbrdir == 1) nbrdir = -1;
	else nbrdir = 1;
}

void fsklog_clear_qso()
{
	fsk_date->value("");
	fsk_time->value("");
	fsk_op_freq->value("");
	fsk_op_call->value("");
	fsk_op_name->value("");
	fsk_rst_in->value("");
	fsk_rst_out->value("");
	fsk_xchg_in->value("");

	if (fsklog_editing)
		fsklog_set_edit(false);

}

void fsklog_save_qso()
{
	char line[256];
	if (fsklog_editing) {
		snprintf(line, sizeof(line),
			"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%05d\t%s",
			fsk_date->value(),
			fsk_time->value(),
			fsk_op_freq->value(),
			fsk_op_call->value(),
			fsk_op_name->value(),
			fsk_rst_in->value(),
			fsk_rst_out->value(),
			(int)cntr_fsk_log_nbr->value(),
			fsk_xchg_in->value());
		brwsr_fsklog_entries->insert(fsklog_edit_nbr, line);
		brwsr_fsklog_entries->remove(fsklog_edit_nbr + 1);
		brwsr_fsklog_entries->select(fsklog_edit_nbr);
		fsklog_set_edit(false);
	} else {
		snprintf(line, sizeof(line),
			"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%05d\t%s",
			fsk_date->value(),
			fsk_time->value(),
			fsk_op_freq->value(),
			fsk_op_call->value(),
			fsk_op_name->value(),
			fsk_rst_in->value(),
			fsk_rst_out->value(),
			(int)cntr_fsk_log_nbr->value(),
			fsk_xchg_in->value());
		brwsr_fsklog_entries->add(line);
	}
	fsklog_changed = true;
}

void fsklog_delete_entry()
{
	brwsr_fsklog_entries->remove(fsklog_edit_nbr);
	brwsr_fsklog_entries->select(fsklog_edit_nbr, false);
	brwsr_fsklog_entries->redraw();
	fsklog_clear_qso();
	fsklog_changed = true;
}

void fsklog_edit_entry()
{
	if (fsklog_editing) {
		fsklog_delete_entry();
		return;
	}

	fsklog_edit_nbr = brwsr_fsklog_entries->value();
	if (!fsklog_edit_nbr) return;

	fsklog_clear_qso();
	size_t ptr = 0;
	std::string entry = brwsr_fsklog_entries->text(fsklog_edit_nbr);

	ptr = entry.find('\t');
	fsk_date->value(entry.substr(0, ptr).c_str());
	entry.erase(0, ptr+1);

	ptr = entry.find('\t');
	fsk_time->value(entry.substr(0,ptr).c_str());
	entry.erase(0, ptr+1);

	ptr = entry.find('\t');
	fsk_op_freq->value(entry.substr(0,ptr).c_str());
	entry.erase(0, ptr+1);

	ptr = entry.find('\t');
	fsk_op_call->value(entry.substr(0,ptr).c_str());
	entry.erase(0, ptr+1);

	ptr = entry.find('\t');
	fsk_op_name->value(entry.substr(0,ptr).c_str());
	entry.erase(0, ptr+1);

	ptr = entry.find('\t');
	fsk_rst_in->value(entry.substr(0,ptr).c_str());
	entry.erase(0, ptr+1);

	ptr = entry.find('\t');
	fsk_rst_out->value(entry.substr(0,ptr).c_str());
	entry.erase(0, ptr+1);

	ptr = entry.find('\t');
	cntr_fsk_log_nbr->value(atoi(entry.substr(0,ptr).c_str()));
	entry.erase(0, ptr+1);

	ptr = entry.find('\t');
	fsk_xchg_in->value(entry.substr(0,ptr).c_str());
	entry.erase(0, ptr+1);

	fsklog_set_edit(true);
}

void fsklog_view()
{
	if (!fsklog_viewer) { 
		fsklog_viewer = new_fsklogbook_dialog();
		if (!progStatus.fsk_log_name.empty()) {
			txt_fsklog_file->value(progStatus.fsk_log_name.c_str());
			fsklog_load();
		} else
			fsklog_open();
	}
	fsklog_viewer->show();
}

void fsklog_save()
{
	if (progStatus.fsk_log_name.empty())
		return;
	std::ofstream oLog(progStatus.fsk_log_name.c_str());
	if (!oLog) {
		fl_message ("Could not write to %s", progStatus.fsk_log_name.c_str());
		return;
	}
	size_t n = brwsr_fsklog_entries->size();
	std::string oline;
	for (size_t i = 1; i <= n; i++) {
		oline = brwsr_fsklog_entries->text(i);
		if (oline.empty()) continue;
		oLog << oline << std::endl;
	}
	oLog.close();
	fsklog_changed = false;
}

void fsklog_load()
{
	std::ifstream iLog(progStatus.fsk_log_name.c_str());
	if (!iLog) return;
	brwsr_fsklog_entries->clear();
	char line[256];
	std::string sline;
	while (!iLog.eof()) {
		memset(line, 0, 256);
		iLog.getline(line, 256);
		sline = line;
		if (!sline.empty())
			brwsr_fsklog_entries->add(sline.c_str());
	}
	iLog.close();
	brwsr_fsklog_entries->redraw();
	fsklog_is_open = true;
}

void fsklog_save_as()
{
// Create and post the local native file chooser
	Fl_Native_File_Chooser fnfc;
	fnfc.title("Save As log file");
	fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	fnfc.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM);
	fnfc.filter("CW Log\t*.txt");
// default directory to use
	fnfc.directory(RigHomeDir.c_str());
	fnfc.preset_file(progStatus.fsk_log_name.c_str());
// Show native chooser
	switch ( fnfc.show() ) {
		case -1:
			fl_message ("ERROR: %s", fnfc.errmsg());
			return; // ERROR
		case 1: 
			return; // CANCEL
		default:
			progStatus.fsk_log_name = fnfc.filename();
			txt_fsklog_file->value(progStatus.fsk_log_name.c_str());
	}
	fsklog_save();
}

void fsklog_open()
{
	if (fsklog_is_open && fsklog_changed)
		fsklog_save();

// Create and post the local native file chooser
	Fl_Native_File_Chooser fnfc;
	fnfc.title("Select log file");
	fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
	fnfc.filter("CW Log\t*.txt");
// default directory to use
	fnfc.directory(RigHomeDir.c_str());
// Show native chooser
	switch ( fnfc.show() ) {
		case -1:
			fl_message ("ERROR: %s", fnfc.errmsg());
			return; // ERROR
		case 1: 
			return; // CANCEL
		default:
			progStatus.fsk_log_name = fnfc.filename();
			txt_fsklog_file->value(progStatus.fsk_log_name.c_str());
			txt_fsklog_file->redraw();
			fsklog_load();
	}
}

void fsklog_new()
{
	if (fsklog_is_open && fsklog_changed)
		fsklog_save();
	brwsr_fsklog_entries->clear();
	brwsr_fsklog_entries->redraw();
	progStatus.fsk_log_name.clear();
	txt_fsklog_file->value(progStatus.fsk_log_name.c_str());
	txt_fsklog_file->redraw();

	Fl_Native_File_Chooser fnfc;
	fnfc.title("Create new log file");
	fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	fnfc.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM);
	fnfc.filter("CW Log\t*.txt");
// default directory to use
	fnfc.directory(RigHomeDir.c_str());
	fnfc.preset_file("fsklog.txt");
// Show native chooser
	switch ( fnfc.show() ) {
		case -1:
			fl_message ("ERROR: %s", fnfc.errmsg());
			return; // ERROR
		case 1: 
			return; // CANCEL
		default:
			progStatus.fsk_log_name = fnfc.filename();
			txt_fsklog_file->value(progStatus.fsk_log_name.c_str());
	}
}

void fsklog_close()
{
	if (fsklog_is_open && fsklog_changed)
		fsklog_save();
}

void fsklog_export_adif()
{
// Create and post the local native file chooser
	Fl_Native_File_Chooser fnfc;
	fnfc.title("Export to ADIF file");
	fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	fnfc.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM);
	fnfc.filter("ADIF Log\t*.{adi,adif}");
// default directory to use
	fnfc.directory(RigHomeDir.c_str());
// Show native chooser
	switch ( fnfc.show() ) {
		case -1:
			fl_message ("ERROR: %s", fnfc.errmsg());
			return; // ERROR
		case 1: 
			return; // CANCEL
		default:
			break;
	}

	std::string export_fname = fnfc.filename();
	std::ofstream oExport(export_fname.c_str());
	if (!oExport) {
		fl_message ("Could not write to %s", export_fname.c_str());
		return;
	}

	std::string logline,
				fsk_date, fsk_time,
				fsk_op_freq,
				fsk_op_call,
				fsk_op_name,
				fsk_rst_in, fsk_rst_out,
				fsk_log_nbr,
				qso_notes;

	size_t ptr = std::string::npos;
	size_t n = brwsr_fsklog_entries->size();
	for (size_t i = 1; i <= n; i++) {
		logline = brwsr_fsklog_entries->text(i);
		if (logline.empty()) continue;

		ptr = logline.find('\t');
		fsk_date = logline.substr(0, ptr);
		logline.erase(0, ptr+1);

		ptr = logline.find('\t');
		fsk_time = logline.substr(0, ptr);
		logline.erase(0, ptr+1);

		ptr = logline.find('\t');
		fsk_op_freq = logline.substr(0, ptr);
		logline.erase(0, ptr+1);

		ptr = logline.find('\t');
		fsk_op_call = logline.substr(0, ptr);
		logline.erase(0, ptr+1);

		ptr = logline.find('\t');
		fsk_op_name = logline.substr(0, ptr);
		logline.erase(0, ptr+1);

		ptr = logline.find('\t');
		fsk_rst_in = logline.substr(0, ptr);
		logline.erase(0, ptr+1);

		ptr = logline.find('\t');
		fsk_rst_out = logline.substr(0, ptr);
		logline.erase(0, ptr+1);

		ptr = logline.find('\t');
		fsk_log_nbr = logline.substr(0, ptr);
		logline.erase(0, ptr+1);

		qso_notes = logline;

		oExport << "<QSO_DATE:" << fsk_date.length() << ">" << fsk_date
				<< "<TIME_ON:" << fsk_time.length() << ">" << fsk_time
				<< "<FREQ:" << fsk_op_freq.length() << ">" << fsk_op_freq
				<< "<MODE:4>RTTY"
				<< "<CALL:" << fsk_op_call.length() << ">" << fsk_op_call
				<< "<NAME:" << fsk_op_name.length() << ">" << fsk_op_name
				<< "<RST_RCVD:" << fsk_rst_in.length() << ">" << fsk_rst_in
				<< "<RST_SENT:" << fsk_rst_out.length() << ">" << fsk_rst_out
				<< "<STX:" << fsk_log_nbr.length() << ">" << fsk_log_nbr
				<< "<NOTES:" << qso_notes.length() << ">" << qso_notes
				<< "<EOR>" << std::endl;
	}
	oExport.close();
}

std::string fsklog_adif_extract( std::string FIELD, std::string line)
{
	size_t p1, p2;
	p1 = line.find(FIELD);
	if (p1 != std::string::npos)  {
		p1 = line.find(">", p1);
		if (p1 != std::string::npos) {
			p1++;
			p2 = line.find("<", p1);
			if (p2 != std::string::npos)
				return line.substr(p1, p2 - p1);
		}
	}
	return "";
}

void fsklog_import_adif()
{
	std::string import_fname;
	Fl_Native_File_Chooser fnfc;
	fnfc.title("Import from ADIF file");
	fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
	fnfc.filter("ADIF Log\t*.{adi,adif}");
// default directory to use
	fnfc.directory(RigHomeDir.c_str());
// Show native chooser
	switch ( fnfc.show() ) {
		case -1:
			fl_message ("ERROR: %s", fnfc.errmsg());
			return; // ERROR
		case 1: 
			return; // CANCEL
		default:
			break;
	}
	import_fname = fnfc.filename();
	std::ifstream iImport(import_fname.c_str());
	if (!iImport) return;

// current log
	size_t n = brwsr_fsklog_entries->size();
	size_t p;
	std::string fulllog;
	std::string teststr;
	for (size_t i = 1; i <= n; i++) {
		fulllog.append(brwsr_fsklog_entries->text(i)).append("\n");
	}

	char buff[512];
	std::string line, ldate, ltime, lfreq, lcall, lname, lrst_in, lrst_out, lnbr, lnotes, lbrwsr;
	while (!iImport.eof()) {
		iImport.getline(buff, 512);
		line = buff;
		if (fsklog_adif_extract("MODE", line) == "RTTY") {
			ldate = fsklog_adif_extract("QSO_DATE", line);
			ltime = fsklog_adif_extract("TIME_ON", line).substr(0,4);
			lfreq = fsklog_adif_extract("FREQ", line);
			lcall = fsklog_adif_extract("CALL", line);
			lname = fsklog_adif_extract("NAME", line);
			lrst_in = fsklog_adif_extract("RST_RCVD", line);
			lrst_out = fsklog_adif_extract("RST_SENT", line);
			lnbr = fsklog_adif_extract("STX", line);
			lnotes = fsklog_adif_extract("NOTES", line);
			lbrwsr.assign(ldate).append("\t");
			lbrwsr.append(ltime).append("\t");
			lbrwsr.append(lfreq).append("\t");
			lbrwsr.append(lcall).append("\t");
			teststr = lbrwsr;
			lbrwsr.append(lname).append("\t");
			lbrwsr.append(lrst_in).append("\t");
			lbrwsr.append(lrst_out).append("\t");
			lbrwsr.append(lnbr).append("\t");
			lbrwsr.append(lnotes);
			p = lbrwsr.find("\n");
			if (p != std::string::npos)
				lbrwsr.erase(p);
			if (fulllog.find(teststr) == std::string::npos &&
				!ldate.empty() &&
				!ltime.empty()) {
				fulllog.append(lbrwsr).append("\n");
				brwsr_fsklog_entries->add(lbrwsr.c_str());
			}
		}
	}
	fsklog_changed = true;
	iImport.close();
}

