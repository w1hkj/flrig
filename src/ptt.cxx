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

void rigPTT(bool on)
{
	if (progStatus.comm_catptt) {
		pthread_mutex_lock(&mutex_serial);
			selrig->set_PTT_control(on);
		pthread_mutex_unlock(&mutex_serial);
	} else if (progStatus.comm_dtrptt || progStatus.comm_rtsptt)
		RigSerial.SetPTT(on);
	else if (SepSerial.IsOpen() && (progStatus.sep_dtrptt || progStatus.sep_rtsptt) )
		SepSerial.SetPTT(on);
	else
		LOG_WARN("No PTT i/o connected");
}
