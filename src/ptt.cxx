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

// add fake rit to this function and to set_vfoA ??

void rigPTT(bool on)
{
	if (on && progStatus.split)
		cbABactive();

	if (progStatus.comm_catptt) {
		pthread_mutex_lock(&mutex_serial);
			selrig->set_PTT_control(on);
		pthread_mutex_unlock(&mutex_serial);
	} else if (progStatus.comm_dtrptt || progStatus.comm_rtsptt)
		RigSerial.SetPTT(on);
	else if (SepSerial.IsOpen() && (progStatus.sep_dtrptt || progStatus.sep_rtsptt) )
		SepSerial.SetPTT(on);
	else
		LOG_INFO("No PTT i/o connected");

	if (!on && progStatus.split)
		cbABactive();
}
