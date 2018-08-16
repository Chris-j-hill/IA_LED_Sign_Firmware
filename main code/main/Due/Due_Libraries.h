
//includes for all 3rd party libraries used

#ifndef Due_Libraries_H
#define Due_Libraries_H

//#include <Wire.h>

 

#include "src/Due_Sd_Cards/SdFat/SdFat.h"
#include "src/Due_Sd_Cards/SdFat/SdFatUtil.h"

#if !USE_MULTIPLE_CARDS
#error You must set USE_MULTIPLE_CARDS nonzero in SdFatConfig.h
#endif

//https://github.com/ivanseidel/DueTimer
#include "src/DueTimer/DueTimer.h"       // wrapper library to handle timers 0-4 

//https://github.com/antodom/soft_uart
#include "src/Due_Software_Serial/soft_uart.h"

#include "src/Due_DHT11/Due_DHT11.h"

#include "Coms.h"
//#include "Coms_I2C.h"
#include "Coms_Serial.h"
#include "Encoder.h"
#include "Fans.h"
#include "Graphics.h"
#include "Internet.h"
#include "Led_strip.h"
#include "Menu_tree.h"
#include "SD_Cards.h"
#include "Current_control.h"
#include "EEPROM.h"
#include "Host.h"
//#include "LUTS.h"

#endif //Due_Libraries_H
