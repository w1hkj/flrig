/*
 * Icom 7700 ... drivers
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

#include "IC7700.h"
#include "debug.h"
#include "support.h"

//=============================================================================
// IC-7700

const char IC7700name_[] = "IC-7700";

RIG_IC7700::RIG_IC7700() {
	defaultCIV = 0x74;
	name_ = IC7700name_;
	adjustCIV(defaultCIV);
};
