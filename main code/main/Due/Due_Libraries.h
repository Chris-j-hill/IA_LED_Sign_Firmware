
//includes for all 3rd party libraries used

#ifndef Due_Libraries_H
#define Due_Libraries_H

#include <Wire.h>

 
//C:\Users\Chris\Documents\ElecSoc\innovation academy sign\sign_code\libs\sdfatlib\SdFat
#include "libs/sdfatlib/SdFat/SdFat.h"
#include "libs/sdfatlib/SdFat/SdFatUtil.h"

#if !USE_MULTIPLE_CARDS
#error You must set USE_MULTIPLE_CARDS nonzero in SdFatConfig.h
#endif

//https://github.com/ivanseidel/DueTimer
#include "DueTimer.h"       // wrapper library to handle timers 0-4 

//https://github.com/antodom/soft_uart
#include "soft_uart.h"







#endif //Due_Libraries_H
