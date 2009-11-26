#include "dialogs.h"
#include "util.h"
#include "debug.h"
#include "serial.h"
#include "support.h"
#include "rigpanel.h"
#include "sockxml_io.h"

#include <string>

#include <iostream>

using namespace std;

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef __WIN32__
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>
#endif

Fl_Double_Window *dlgDisplayConfig = NULL;
Fl_Double_Window *dlgXcvrConfig = NULL;
Fl_Double_Window *dlgMemoryDialog = NULL;

//==============================================================
// Transceiver dialog

CPT  commPortTable[12];
int  commportnbr = 0;
int  iNbrCommPorts  = 0;
char szttyport[20] = "";

void initCommPortTable () {
#ifdef __WIN32__
	char szTestPort[] = "COMxx";
	for (int i = 0; i < 12; i++) {
		commPortTable[i].nbr = 0;
		commPortTable[i].szPort = 0;
	}
	commPortTable[0].szPort = new char(6);
	strcpy(commPortTable[0].szPort,"NONE");
	iNbrCommPorts = 0;
	for (int i = 1; i < 12; i++) {
		sprintf(szTestPort, "COM%d", i);
		if (RigSerial.CheckPort (szTestPort) == true) {
			iNbrCommPorts++;
			commPortTable[iNbrCommPorts].szPort = new char(strlen(szTestPort)+1);
			commPortTable[iNbrCommPorts].nbr = i;
			strcpy(commPortTable[iNbrCommPorts].szPort, szTestPort);
		}
	}
#else
	char szTestPort[] = "ttySx";
	char szTestUSB[] = "ttyUSBx";
	for (int i = 0; i < 8; i++) {
		commPortTable[i].nbr = 0;
		commPortTable[i].szPort = 0;
	}
	commPortTable[0].szPort = new char(15);
	strcpy(commPortTable[0].szPort,"NONE");
	iNbrCommPorts = 0;
	for (int i = 1; i < 8; i++) {
		szTestPort[4] = '0' + i - 1;
		if (RigSerial.CheckPort(szTestPort)) {
			iNbrCommPorts++;
			commPortTable[iNbrCommPorts].szPort = new char(15);
			commPortTable[iNbrCommPorts].nbr = i;
			strcpy(commPortTable[iNbrCommPorts].szPort, szTestPort);
		}
	}
	int j = 0;
	for (int k = iNbrCommPorts; k < 8; k++, j++) {
		szTestUSB[6] = '0' + j - 1;
		if (RigSerial.CheckPort(szTestUSB)) {
			iNbrCommPorts++;
			commPortTable[iNbrCommPorts].szPort = new char(15);
			commPortTable[iNbrCommPorts].nbr = j + 8;
			strcpy(commPortTable[iNbrCommPorts].szPort, szTestUSB);
		}
	}
#endif
}

void cbCancelXcvrDialog()
{
	bypass_serial_thread_loop = false;

	btnOkXcvrDialog->labelcolor(FL_BLACK);
	dlgXcvrConfig->hide();
}

void cbOkXcvrDialog()
{
	string p1 = selectCommPort->value();
	string p2 = selectAuxPort->value();
	string p3 = selectSepPTTPort->value();

	if (p1.compare("NONE") != 0)
		if (p1 == p2 || p1 == p3) {
			fl_message("Select separate ports");
			return;
		}

	if (p2.compare("NONE") != 0)
		if (p2 == p3) {
			fl_message("Select separate ports");
			return;
		}

	if (rig_nbr != selectRig->index()) {
		clearList();
		saveFreqList();
		rig_nbr = selectRig->index();
		selrig = rigs[rig_nbr];
	}

	pthread_mutex_lock(&mutex_serial);
		RigSerial.ClosePort();
		AuxSerial.ClosePort();
		SepSerial.ClosePort();
	pthread_mutex_unlock(&mutex_serial);

	progStatus.rig_nbr = rig_nbr;

	progStatus.xcvr_serial_port = selectCommPort->value();
	progStatus.aux_serial_port = selectAuxPort->value();
	progStatus.sep_serial_port = selectSepPTTPort->value();

	progStatus.comm_baudrate = mnuBaudrate->index();
	progStatus.stopbits = btnOneStopBit->value() ? 1 : 2;
	progStatus.comm_retries = (int)cntRigCatRetries->value();
	progStatus.comm_timeout = (int)cntRigCatTimeout->value();
	progStatus.comm_wait = (int)cntRigCatWait->value();
	progStatus.comm_echo = btnRigCatEcho->value();
	progStatus.comm_rtsptt = btnrtsptt->value();
	progStatus.comm_catptt = btncatptt->value();
	progStatus.comm_dtrptt = btndtrptt->value();
	progStatus.comm_rtscts = chkrtscts->value();
	progStatus.comm_rtsplus = btnrtsplus->value();
	progStatus.comm_dtrplus = btndtrplus->value();

	progStatus.sep_dtrplus = btnSepDTRplus->value();
	progStatus.sep_dtrptt = btnSepDTRptt->value();
	progStatus.sep_rtsplus = btnSepRTSplus->value();
	progStatus.sep_rtsptt = btnSepRTSptt->value();

	progStatus.opMODE = selrig->def_mode;
	progStatus.opBW   = selrig->defbw_;
	progStatus.freq   = selrig->deffreq_;

	init_title();

	if (!startXcvrSerial()) {
		if (progStatus.xcvr_serial_port.compare("NONE") == 0) {
			LOG_WARN("No comm port ... test mode");
		} else {
			progStatus.xcvr_serial_port = "NONE";
			selectCommPort->value(progStatus.xcvr_serial_port.c_str());
		}
	}
	if (!startAuxSerial()) {
		if (progStatus.aux_serial_port.compare("NONE") != 0) {
			progStatus.aux_serial_port = "NONE";
			selectAuxPort->value(progStatus.aux_serial_port.c_str());
		}
	}
	if (!startSepSerial()) {
		if (progStatus.sep_serial_port.compare("NONE") != 0) {
			progStatus.sep_serial_port = "NONE";
			selectSepPTTPort->value(progStatus.sep_serial_port.c_str());
		}
	}

	initRig();

//	wait_query = true;
	send_name();
	send_modes();
	send_bandwidths();
	send_mode_changed();
	send_sideband();
	send_bandwidth_changed();
	send_new_freq();
//	wait_query = false;

	cbCancelXcvrDialog();
}

void configXcvr()
{
	pthread_mutex_lock(&mutex_serial);
		bypass_serial_thread_loop = true;
	pthread_mutex_unlock(&mutex_serial);

	selectCommPort->value(progStatus.xcvr_serial_port.c_str());
	selectAuxPort->value(progStatus.aux_serial_port.c_str());

	dlgXcvrConfig->show();
}

void createXcvrDialog()
{
	dlgXcvrConfig = XcvrDialog();
	initCommPortTable();
	selectCommPort->clear();
	selectAuxPort->clear();
	selectSepPTTPort->clear();
	for (int i = 0; i <= iNbrCommPorts; i++) {
		selectCommPort->add(commPortTable[i].szPort);
		selectAuxPort->add(commPortTable[i].szPort);
		selectSepPTTPort->add(commPortTable[i].szPort);
	}
	mnuBaudrate->clear();
	for (int i = 0; szBaudRates[i] != NULL; i++)
		mnuBaudrate->add(szBaudRates[i]);

	cbo_agc_level->clear();
	cbo_agc_level->add("slow");
	cbo_agc_level->add("med");
	cbo_agc_level->add("fast");
	cbo_agc_level->index(progStatus.agc_level);

	initRigCombo();
}

// Frequency display colors

uchar fg_red, fg_green, fg_blue;
uchar bg_red, bg_green, bg_blue;
uchar sl_red, sl_green, sl_blue;

void cbPrefBackground()
{
	const char *title = "Background color";
	Fl::get_color(lblTest->color(), bg_red, bg_green, bg_blue);
	if (fl_color_chooser(title, bg_red, bg_green, bg_blue))
		lblTest->color(fl_rgb_color (bg_red, bg_green, bg_blue));
	dlgDisplayConfig->redraw();
}

void cbPrefForeground()
{
	const char *title = "Foreground color";
	Fl::get_color(lblTest->labelcolor(), bg_red, bg_green, bg_blue);
	if (fl_color_chooser(title, fg_red, fg_green, fg_blue))
		lblTest->labelcolor(fl_rgb_color (fg_red, fg_green, fg_blue));
	dlgDisplayConfig->redraw();
}

void cbBacklightColor()
{
	uchar red, green, blue;
	Fl::get_color(scaleSmeterColor->color(), red, green, blue);
	if (fl_color_chooser("Backlight color", red, green, blue)) {
		scaleSmeterColor->color(fl_rgb_color (red, green, blue));
		grpMeterColor->color(fl_rgb_color (red, green, blue));
	}
	grpMeterColor->redraw();
}

void cbSMeterColor()
{
	uchar red, green, blue;
	Fl::get_color(sldrRcvSignalColor->color(), red, green, blue);
	if (fl_color_chooser("S Meter color", red, green, blue))
		sldrRcvSignalColor->color(fl_rgb_color (red, green, blue));
	grpMeterColor->redraw();
}

uchar pRed, pGreen, pBlue;

void cbPwrMeterColor()
{
	pRed = progStatus.pwrRed;
	pGreen = progStatus.pwrGreen;
	pBlue = progStatus.pwrBlue;
	fl_color_chooser("Power color", pRed, pGreen, pBlue);
}

uchar sRed, sGreen, sBlue;

void cbSWRMeterColor()
{
	sRed = progStatus.swrRed;
	sGreen = progStatus.swrGreen;
	sBlue = progStatus.swrBlue;
	fl_color_chooser("Power color", sRed, sGreen, sBlue);
}

void cbOkDisplayDialog()
{
	FreqDisp->SetONOFFCOLOR(lblTest->labelcolor(), lblTest->color());
	
	txtInactive->color(lblTest->color());
	txtInactive->labelcolor(lblTest->labelcolor());
	txtInactive->redraw();

	scaleSmeter->color(scaleSmeterColor->color());
	scalePower->color(scaleSmeterColor->color());
	btnALC_SWR->color(scaleSmeterColor->color());
	grpMeters->color(scaleSmeterColor->color());
	
	sldrRcvSignal->color(sldrRcvSignalColor->color(), scaleSmeter->color());
	sldrFwdPwr->color(fl_rgb_color(pRed, pGreen, pBlue), scaleSmeter->color());
	sldrALC_SWR->color(fl_rgb_color(sRed, sGreen, sBlue), scaleSmeter->color());
	progStatus.swrRed = sRed;
	progStatus.swrGreen = sGreen;
	progStatus.swrBlue = sBlue;
	progStatus.pwrRed = pRed;
	progStatus.pwrGreen = pGreen;
	progStatus.pwrBlue = pBlue;

	grpMeters->redraw();

	dlgDisplayConfig->hide();
}

void cbCancelDisplayDialog()
{
	dlgDisplayConfig->hide();
}

void setDisplayColors()
{
	unsigned char red, green, blue;

	if (dlgDisplayConfig == NULL)
		return;

	FreqDisp->GetONCOLOR(red,green,blue);
	lblTest->labelcolor(fl_rgb_color(red,green,blue));
	FreqDisp->GetOFFCOLOR(red,green,blue);
	lblTest->color(fl_rgb_color(red,green,blue));
	scaleSmeterColor->color(scaleSmeter->color());
	grpMeterColor->color(scaleSmeterColor->color());
	sldrRcvSignalColor->color(sldrRcvSignal->color(), scaleSmeter->color());
	sldrRcvSignalColor->minimum(0);
	sldrRcvSignalColor->maximum(100);
	sldrRcvSignalColor->value(25);
	dlgDisplayConfig->show();
}

void cbCloseMemory()
{
	dlgMemoryDialog->hide();
}

void openMemoryDialog()
{
	if (dlgMemoryDialog == NULL)
		return;
	dlgMemoryDialog->show();
}

