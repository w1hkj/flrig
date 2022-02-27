// ----------------------------------------------------------------------------
// Copyright (C) 2020
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

#include <iostream>

#include "gpio_ptt.h"

#include "support.h"

//-------------------- gpio port PTT --------------------//
#ifndef __MINGW32__
void gpioEXEC(std::string execstr)
{
	int pfd[2];
	if (pipe(pfd) == -1) {
		LOG_PERROR("pipe");
		return;
	}
	int pid;
	switch (pid = fork()) {
		case -1:
			LOG_PERROR("fork");
			return;
		case 0: // child
			close(pfd[0]);
			if (dup2(pfd[1], STDOUT_FILENO) != STDOUT_FILENO) {
				LOG_PERROR("dup2");
				exit(EXIT_FAILURE);
			}
			close(pfd[1]);
			execl("/bin/sh", "sh", "-c", execstr.c_str(), (char *)NULL);
			perror("execl");
			exit(EXIT_FAILURE);
	}

	// parent
	close(pfd[1]);
}
#else // !__MINGW32__

void gpioEXEC(std::string execstr)
{
	char* cmd = strdup(execstr.c_str());

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	memset(&pi, 0, sizeof(pi));
	if (!CreateProcess(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
		LOG_ERROR("CreateProcess failed with error code %ld", GetLastError());
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	free(cmd);
}
#endif // !__MINGW32__

static const char *gpio_name[] = {
		"17", "18", "27", "22", "23",
		"24", "25", "4",  "5",  "6",
		"13", "19", "26", "12", "16",
		"20", "21"};

void export_gpio(int bcm)
{
	if (bcm < 0 || bcm > 16) return;
	std::string exec_str = "gpio export ";
	exec_str.append(gpio_name[bcm]).append(" out");
	gpioEXEC(exec_str);
	LOG_INFO("%s", exec_str.c_str());
}

void unexport_gpio(int bcm)
{
	if (bcm < 0 || bcm > 16) return;
	std::string exec_str = "gpio unexport ";
	exec_str.append(gpio_name[bcm]);
	gpioEXEC(exec_str);
	LOG_INFO("%s", exec_str.c_str());
}

void open_gpio(void)
{
	bool enabled = false;
	for (int i = 0; i < 17; i++) {
		enabled = (progStatus.enable_gpio >> i) & 0x01;
		if (enabled) export_gpio(i);
	}
}

void close_gpio(void)
{
	bool enabled = false;
	for (int i = 0; i < 17; i++) {
		enabled = (progStatus.enable_gpio >> i) & 0x01;
		if (enabled) unexport_gpio(i);
	}
}

static int enabled = 0;
int get_gpio()
{
	return enabled;
}

void set_gpio(bool ptt)
{
#define VALUE_MAX 30
	static const char s_values_str[] = "01";

	std::string portname = "/sys/class/gpio/gpio";
	std::string ctrlport;
	enabled = false;
	int val = 0;
	int fd;

	for (int i = 0; i < 17; i++) {
		enabled = (progStatus.enable_gpio >> i) & 0x01;

		if (enabled) {
			val = (progStatus.gpio_on >> i) & 0x01;
			ctrlport = portname;
			ctrlport.append(gpio_name[i]);
			ctrlport.append("/value");
			fd = fl_open(ctrlport.c_str(), O_WRONLY);

			bool ok = false;
			if (fd == -1) {
				LOG_ERROR("Failed to open gpio (%s) for writing!", ctrlport.c_str());
			} else {
				if (progStatus.gpio_pulse_width == 0) {
					if (ptt) { if (val == 1) val = 1; else val = 0;}
					if (!ptt)  { if (val == 1) val = 0; else val = 1;}
					if (write(fd, &s_values_str[val], 1) == 1)
						ok = true;
				} else {
					if (write(fd, &s_values_str[val], 1) == 1) {
						MilliSleep(progStatus.gpio_pulse_width);
						if (write(fd, &s_values_str[val == 0 ? 1 : 0], 1) == 1)
							ok = true;
					}
				}
				if (ok)
					LOG_INFO("Set GPIO ptt on %s %s%s",
						ctrlport.c_str(),
						(progStatus.gpio_pulse_width > 0) ?
							"pulsed " : "",
						(val == 1 ? "HIGH" : "LOW")
					);
				else
					LOG_ERROR("Failed to write value!");

				close(fd);
			}
		}
	}
}
