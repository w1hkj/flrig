#include "images.h"

//#include "dbm.xbm"
//#include "rev.xbm"
//#include "swr.xbm"
//#include "mvolts.xbm"
//#include "smeter.xbm"

#include "images/P25.xbm"
#include "images/P50.xbm"
#include "images/P100.xbm"
#include "images/P200.xbm"
#include "images/P200log.xbm"

#include "images/S60.xbm"
#include "images/SWR.xbm"
#include "images/alc.xbm"

Fl_Bitmap image_p25(P25_bits, P25_width, P25_height);
Fl_Bitmap image_p50(P50_bits, P50_width, P50_height);
Fl_Bitmap image_p100(P100_bits, P100_width, P100_height);
Fl_Bitmap image_p200(P200_bits, P200_width, P200_height);
Fl_Bitmap image_p200log(P200log_bits, P200log_width, P200log_height);

Fl_Bitmap image_smeter(S60_bits, S60_width, S60_height);
Fl_Bitmap image_swr(SWR_bits, SWR_width, SWR_height);
Fl_Bitmap image_alc(alc_bits, alc_width, alc_height);



