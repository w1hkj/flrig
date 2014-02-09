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

#ifndef SOCK_XML_IO_H
#define SOCK_XML_IO_H

extern bool wait_query;
extern bool fldigi_online;
extern bool run_digi_loop;
extern bool bypass_digi_loop;
extern bool rig_reset;

extern void open_rig_xmlrpc();
extern void close_rig_xmlrpc();
extern void send_new_freq(long);
extern void send_modes();
extern void send_bandwidths();
extern void send_name();
extern void send_ptt_changed(bool);
extern void send_new_mode(int);
extern void send_new_bandwidth(int);
//! extern void send_rig_info();
extern void send_sideband();
extern void send_no_rig();
extern void send_xml_freq(long);
extern void send_new_notch(int);

extern void * digi_loop(void *d);

#endif
