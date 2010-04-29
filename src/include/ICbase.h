#ifndef _ICbase_H
#define _ICbase_H

#include "rigbase.h"
#include "debug.h"
#include "support.h"

class RIG_ICOM : public rigbase {
protected:
	string pre_to;
	string pre_fm;
	string post;
	string ok;
	string bad;
	const char *_mode_type;
public:
	RIG_ICOM() {
		CIV = 0x56;
		pre_to = "\xFE\xFE\x56\xE0";
		pre_fm = "\xFE\xFE\xE0\x56";
		post = "\xFD";
		ok = "\xFE\xFE\xE0\x56\xFB\xFD";
		bad = "\xFE\xFE\xE0\x56\xFA\xFD";
	}
	virtual ~RIG_ICOM() {}
	void checkresponse(int n);
	bool sendICcommand(string str, int nbr);
	void adjustCIV(uchar adr);
};

#endif
