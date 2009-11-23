//======================================================================
// xml_io.h
//
// copyright 2009, W1HKJ
//
//======================================================================
#ifndef XML_IO_H
#define XML_IO_H

#include <sys/types.h>
#include <pthread.h>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client.hpp>


extern void send_new_freq();
extern void send_modes();
extern void send_bandwidths();
extern void send_name();
extern void send_rig_info();
extern void send_ptt_changed(bool on);
extern void send_mode_changed();
extern void send_sideband();
extern void send_bandwidth_changed();
extern void setvfo(void *d);

extern void open_xmlrpc();

extern bool xmlrpc_thread_done;
extern bool bypass_xmlrpc_loop;
extern bool xmlrpc_query;
extern bool wait_query;
extern bool xmlrpc_disabled;

extern pthread_t *xmlrpc_thread;
extern pthread_mutex_t mutex_xmlrpc;

extern void * xmlrpc_thread_loop(void *d);

extern void xmlrpc_timer(void *d);

extern void post_name();

#endif
