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

#ifndef FSKLOG_H
#define FSKLOG_H

extern bool fsklog_editing;

extern void fsklog_sort_by_call();
extern void fsklog_sort_by_datetime();
extern void fsklog_sort_by_nbr();
extern void fsklog_sort_by_freq();

extern void fsklog_clear_qso();
extern void fsklog_save_qso();

extern void fsklog_edit_entry();
extern void fsklog_delete_entry();
extern void fsklog_view();

extern void fsklog_close();
extern void fsklog_open();
extern void fsklog_new();

extern void fsklog_load();
extern void fsklog_save();
extern void fsklog_save_as();

extern void fsklog_export_adif();
extern void fsklog_import_adif();

#endif
