#ifndef SOCK_XML_IO_H
#define SOCK_XML_IO_H

extern bool wait_query;
extern bool fldigi_online;
extern bool run_digi_loop;
extern bool rig_reset;

extern void open_rig_xmlrpc();
extern void send_new_freq();
extern void send_modes();
extern void send_bandwidths();
extern void send_name();
extern void send_ptt_changed(bool);
extern void send_mode_changed();
extern void send_bandwidth_changed();
//! extern void send_rig_info();
extern void send_sideband();
extern void send_no_rig();

extern void * digi_loop(void *d);

#endif
