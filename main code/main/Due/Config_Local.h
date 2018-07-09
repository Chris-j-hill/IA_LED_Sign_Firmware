
#ifndef Configuration_H
#define Configuration_H
#ifndef Config_Local_h
#define Config_Local_h
#include "Arduino.h"
//#include "Due_Libraries.h"



//#define DEBUG       //enable debugging by default 

#ifdef DEBUG
#define Sprintln(a) (Serial.println(a))   // use to print debug errors
#define Sprint(a) (Serial.print(a))

#else

#define Sprintln(a)    // prints nothing if debug not defined
#define Sprint(a)
#endif



#define USE_SERIAL_TO_MEGAS
//#define USE_I2C_TO_MEGAS

#define NAVIGATE_MENU_OPTIONS_OVER_USB_SERIAL   //initialises serial regardless of DEBUG
#define USING_SIMPLE_BINARY_NODE_READY_PROTOCOL   // one wire protocol to establish sync boards 
#ifndef USING_SIMPLE_BINARY_NODE_READY_PROTOCOL
#define STARTUP_DUE_DELAY_PERIOD 500             //if no one wire protocol, just delay, system should be stable so should autosync after a while
#endif

#define SINGLE_MATRIX_WIDTH 64
#define SINGLE_MATRIX_HEIGHT 32
#define NUM_SCREENS 4    //per side
#define TOTAL_WIDTH SINGLE_MATRIX_WIDTH * NUM_SCREENS

// Select if the due is enabled or if all megas enabled, if not all megas enabled, choose which
#define DUE_ENABLED                        
#define ALL_MEGAS_ENABLED

#ifndef ALL_MEGAS_ENABLED
#define MEGA_1_ENABLED true
#define MEGA_2_ENABLED true
#define MEGA_3_ENABLED true
#define MEGA_4_ENABLED true
#else
#define MEGA_1_ENABLED false
#define MEGA_2_ENABLED false
#define MEGA_3_ENABLED false
#define MEGA_4_ENABLED false
#endif


//select one of these colour scales
//#define USING_COLOUR_SET_333
//#define USING_COLOUR_SET_444    // highest resolution without loosing data
#define USING_COLOUR_SET_888      // highest resolution, requires down sampling inputs (actual output colour might not be as expected)


// ______ SD Variables _________

#define EXPECTS_SD_ON_STARTUP   //if not defined, will not search for sd card, will carry on with default string since no internet connectivity


#define EXTERNAL_SD_CARD_DIRECTORY_NAME "/EXTERNAL"
#define INTERNAL_SD_CARD_DIRECTORY_NAME "/INTERNAL"

#define NETWORK_LOGIN_FILENAME "Networks.BIN"


#define ENABLE_FANS
#define ENABLE_LED_STRIP
#define ENABLE_TEMPERATURE_SENSORS
#define ENABLE_LIGHT_SENSORS
#define ENABLE_CURRENT_METER

#define ENABLE_ENCODER
#define ENABLE_BUTTON
#define ENABLE_SD_CARDS
#define ENABLE_ETHERNET
#define ENABLE_WIFI

#define RUN_I2C_SCANNER_TEST  // identify connected addresses

#define DISABLE_WARNING_MESSAGES  //option to disable warning messages within the menu system

#define MAX_NUM_OF_TEXT_OBJECTS 5

// ______ Prefixes for sensor data _______


// NOTE: Prefix values must be within byte range (0-255)
#define PREFIX_CURRENT_1                10
#define PREFIX_CURRENT_2                11
#define PREFIX_TEMP_1                   20
#define PREFIX_TEMP_2                   21
#define PREFIX_TEMP_3                   22
#define PREFIX_LDR_1                    30
#define PREFIX_LDR_2                    31
#define PREFIX_FAN_SPEED                40
#define PREFIX_FAN_MINIMUM_SPEED        41
#define PREFIX_LED_STRIP_BRIGHTNESS     50
#define PREFIX_SD1_DETECTED             60
#define PREFIX_SD2_DETECTED             61
#define PREFIX_EHTERNET_CONNECTED       70
#define PREFIX_WIFI_CONNECTED           80
#define PREFIX_SCREEN_BRIGHTNESS        90
#define PREFIX_TEXT_SIZE_0                100
#define PREFIX_TEXT_COLOUR_R_0            110
#define PREFIX_TEXT_COLOUR_G_0            120
#define PREFIX_TEXT_COLOUR_B_0            130
#define PREFIX_TEXT_HUE_MSB_0             140
#define PREFIX_TEXT_HUE_LSB_0             150
#define PREFIX_TEXT_USE_HUE_0             160
#define PREFIX_DEBUG_STATE              170
#define PREFIX_SCREEN_MODE              180
#define PREFIX_SD_MOUNTED_1             190
#define PREFIX_SD_MOUNTED_2             191
#define PREFIX_TEXT_SCROLL_SPEED_X_0      200
#define PREFIX_TEXT_SCROLL_SPEED_Y_0      201



//#define SKIP_INTITAL_STARTUP_SEQUENCE

#define DEFAULT_MENU_WIDTH 64



#endif // Config_Local_h
#endif // Configuration_H
