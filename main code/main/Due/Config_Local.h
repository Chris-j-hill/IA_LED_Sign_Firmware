
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

#define DO_ERROR_CHECKING //calculate checksum

#ifdef DO_ERROR_CHECKING
#define DO_HEAVY_ERROR_CHECKING     // parity checking each byte and checksum
//#define DO_HEADER_ERROR_CORRECTING  // hamming encoded header bytes for bit correction
#endif


// some variables used to test the system with no mega attached

#define MEGA_SERIAL_CONNECTION_TESTING  // define to fake a positive response from ping test on initialisation for megas
                                        // useful if megas not loaded with correct code (eg megas running serial passthrough)

//#define DISABLE_POS_TRANSMISSION    //stops transmission of pos in interrupt
#define FORCE_TEXT_FRAME_TRANSMISSION


#ifdef FORCE_TEXT_FRAME_TRANSMISSION
#define NUM_TEXT_TRANSMISSIONS  1
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


//default startup conditions
#define ENABLE_FANS
#define ENABLE_LED_STRIP
#define ENABLE_TEMPERATURE_SENSORS
#define ENABLE_LIGHT_SENSORS
#define ENABLE_CURRENT_METER

#define ENABLE_ENCODER
#define ENABLE_BUTTON
#define ENABLE_SD_CARDS
#define ENABLE_CARD_LED
#define ENABLE_ETHERNET
#define ENABLE_WIFI


#define DISABLE_WARNING_MESSAGES  //option to disable warning messages within the menu system

#define MAX_NUM_OF_TEXT_OBJECTS 5

//#define ALLOW_SCREEN_BRIGHTNESS_AUTO_ADJUSTMENT

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
#define PREFIX_FAN_ENABLED              42
#define PREFIX_LED_STRIP_BRIGHTNESS     50
#define PREFIX_LED_STRIP_ENABLED        51
#define PREFIX_SD1_DETECTED             60
#define PREFIX_SD2_DETECTED             61
#define PREFIX_EHTERNET_CONNECTED       70
#define PREFIX_WIFI_CONNECTED           80
#define PREFIX_SCREEN_BRIGHTNESS        90

#define PREFIX_TEXT_SIZE_0                100
#define PREFIX_TEXT_SIZE_1                101
#define PREFIX_TEXT_SIZE_2                102
#define PREFIX_TEXT_SIZE_3                103
#define PREFIX_TEXT_SIZE_4                104

#define PREFIX_TEXT_COLOUR_R_0            110
#define PREFIX_TEXT_COLOUR_R_1            111
#define PREFIX_TEXT_COLOUR_R_2            112
#define PREFIX_TEXT_COLOUR_R_3            113
#define PREFIX_TEXT_COLOUR_R_4            114

#define PREFIX_TEXT_COLOUR_G_0            120
#define PREFIX_TEXT_COLOUR_G_1            121
#define PREFIX_TEXT_COLOUR_G_2            122
#define PREFIX_TEXT_COLOUR_G_3            123
#define PREFIX_TEXT_COLOUR_G_4            124

#define PREFIX_TEXT_COLOUR_B_0            130
#define PREFIX_TEXT_COLOUR_B_1            131
#define PREFIX_TEXT_COLOUR_B_2            132
#define PREFIX_TEXT_COLOUR_B_3            133
#define PREFIX_TEXT_COLOUR_B_4            134

#define PREFIX_TEXT_HUE_MSB_0             140
#define PREFIX_TEXT_HUE_MSB_1             141
#define PREFIX_TEXT_HUE_MSB_2             142
#define PREFIX_TEXT_HUE_MSB_3             143
#define PREFIX_TEXT_HUE_MSB_4             144

#define PREFIX_TEXT_HUE_LSB_0             150
#define PREFIX_TEXT_HUE_LSB_1             151
#define PREFIX_TEXT_HUE_LSB_2             152
#define PREFIX_TEXT_HUE_LSB_3             153
#define PREFIX_TEXT_HUE_LSB_4             154

#define PREFIX_TEXT_USE_HUE_0             160
#define PREFIX_TEXT_USE_HUE_1             161
#define PREFIX_TEXT_USE_HUE_2             162
#define PREFIX_TEXT_USE_HUE_3             163
#define PREFIX_TEXT_USE_HUE_4             164

#define PREFIX_DEBUG_STATE              170
#define PREFIX_SCREEN_MODE              180
#define PREFIX_SD_MOUNTED_1             190
#define PREFIX_SD_MOUNTED_2             191

#define PREFIX_TEXT_SCROLL_SPEED_X_0      200
#define PREFIX_TEXT_SCROLL_SPEED_X_1      201
#define PREFIX_TEXT_SCROLL_SPEED_X_2      202
#define PREFIX_TEXT_SCROLL_SPEED_X_3      203
#define PREFIX_TEXT_SCROLL_SPEED_X_4      204

#define PREFIX_TEXT_SCROLL_SPEED_Y_0      210
#define PREFIX_TEXT_SCROLL_SPEED_Y_1      211
#define PREFIX_TEXT_SCROLL_SPEED_Y_2      212
#define PREFIX_TEXT_SCROLL_SPEED_Y_3      213
#define PREFIX_TEXT_SCROLL_SPEED_Y_4      214

#define PREFIX_TEXT_OBJ_ENABLED_0         220
#define PREFIX_TEXT_OBJ_ENABLED_1         221
#define PREFIX_TEXT_OBJ_ENABLED_2         222
#define PREFIX_TEXT_OBJ_ENABLED_3         223
#define PREFIX_TEXT_OBJ_ENABLED_4         224

#define PREFIX_TEXT_OBJ_SELECTED          230

// NB: do not use prefix value of 251-254, these are frame start and end bytes


//#define SKIP_INTITAL_STARTUP_SEQUENCE

#define DEFAULT_MENU_WIDTH 64
#define MEGA_SERIAL_BUFFER_LENGTH 32 // must match or be less than SERIAL_RX_BUFFER_SIZE in hardware->arduino->avr->cores->arduino->HardwareSerial.h


//________configure interrupt priority___________

//https://forum.arduino.cc/index.php?topic=506814.0
//https://www.keil.com/pack/doc/cmsis/Core/html/group__NVIC__gr.html#ga7e1129cd8a196f4284d41db3e82ad5c8
#define SOFT_SERIAL_PRIORITY  0  // set timers to highest priority
#define POS_FRAME_PRIORITY    1  // set pos frame as mid priority
#define LED_STRIP_PRIORITY    2  //set everything else as low priority
#define FAN_PRIORITY          2


#define POS_UPDATE_ISR_FREQ 2//hz

#define XY_SPEED_UNITS 0.1//speed of +1 moves XY_SPEED_UNITS pixels per second



#endif // Config_Local_h
#endif // Configuration_H
