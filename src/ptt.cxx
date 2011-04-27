#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>

#include "ptt.h"
#include "debug.h"
#include "rig_io.h"
#include "rig.h"
#include "support.h"

using namespace std;

// used for transceivers with a single vfo, called only by rigPTT
static void fake_split(int on)
{
	if (on) {
		selrig->set_vfoA(vfoB.freq);
		selrig->set_modeA(vfoB.imode);
		selrig->set_bwA(vfoB.iBW);
	} else {
		selrig->set_vfoA(vfoA.freq);
		selrig->set_modeA(vfoA.imode);
		selrig->set_bwA(vfoA.iBW);
	}
}

// add fake rit to this function and to set_vfoA ??

void rigPTT(bool on)
{
//	wait_query = true;
	pthread_mutex_lock(&mutex_serial);

	if (on && progStatus.split && !selrig->can_split())
		fake_split(on);

	if (progStatus.comm_catptt) {
		selrig->set_PTT_control(on);
	} else if (progStatus.comm_dtrptt || progStatus.comm_rtsptt)
		RigSerial.SetPTT(on);
	else if (SepSerial.IsOpen() && (progStatus.sep_dtrptt || progStatus.sep_rtsptt) )
		SepSerial.SetPTT(on);
	else
		LOG_INFO("No PTT i/o connected");

	if (!on && progStatus.split && !selrig->can_split())
		fake_split(on);

	pthread_mutex_unlock(&mutex_serial);
//	wait_query = false;
}
