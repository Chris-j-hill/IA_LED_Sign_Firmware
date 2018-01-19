
#ifndef Configuration_H
#define Configuration_H
#ifndef Config_Local_h
#define Config_Local_h
#include "Arduino.h"
//#include "Due_Libraries.h"



#define DEBUG       //enable debugging by default 

#ifdef DEBUG
#define Sprintln(a) (Serial.println(a))   // use to print debug errors
#define Sprint(a) (Serial.print(a))

#else

#define Sprintln(a)    // prints nothing if debug not defined
#define Sprint(a)
#endif



#define USE_SERIAL_TO_MEGAS
#define USE_I2C_TO_MEGAS

#define NAVIGATE_MENU_OPTIONS_OVER_USB_SERIAL   //initialises serial regardless of DEBUG
#define USING_SIMPLE_BINARY_NODE_READY_PROTOCOL   // one wire protocol to establish sync boards 
#ifndef USING_SIMPLE_BINARY_NODE_READY_PROTOCOL
#define STARTUP_DUE_DELAY_PERIOD 500             //if no one wire protocol, just delay, system should be stable so should autosync after a while
#endif

#define single_matrix_width 64
#define single_matrix_height 32
#define NUM_SCREENS 4    //per side
#define TOTAL_WIDTH single_matrix_width * NUM_SCREENS

// Select if the due is enabled or if all megas enabled, if not all megas enabled, choose which
#define DUE_ENABLED                        
#define ALL_MEGAS_ENABLED

#ifndef ALL_MEGAS_ENABLED
#define MEGA_1_ENABLED true
#define MEGA_2_ENABLED true
#define MEGA_3_ENABLED true
#define MEGA_4_ENABLED true
#endif

//select one of these colour scales
#define USING_COLOUR_SET_333
//#define USING_COLOUR_SET_444    // NOT FINISHED IMPLEMENTATION



// ______ Text Variables _________

//how often will the text buffer in the megas be refreshed. NB not cursor position 
#define TEXT_TRANSMIT_PERIOD 2000
#define DEFAULT_TEXT_SIZE 2
#define DEFAULT_TEXT_RED_BRIGHTNESS 7
#define DEFAULT_TEXT_GREEN_BRIGHTNESS 7
#define DEFAULT_TEXT_BLUE_BRIGHTNESS 7

//byte frame_type;             // frame type defines what is to be done with info, ie where string should go or other operation
//byte frame_length = 150;     // length of current frame
#define max_frame_size = 150;    // max frame size 150, ie max length of a tweet plus some frame info
// byte frame[150];             //frame array


// ______ SD Variables _________

#define EXPECTS_SD_ON_STARTUP   //if not defined, will not search for sd card, will carry on wiht default string since no internet connectivity


#define EXTERNAL_SD_CARD_DIRECTORY_NAME "EXTERNAL"
#define INTERNAL_SD_CARD_DIRECTORY_NAME "INTERNAL"

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


#endif // Config_Local_h
#endif // Configuration_H
