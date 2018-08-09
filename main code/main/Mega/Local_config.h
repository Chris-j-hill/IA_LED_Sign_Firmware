#ifndef LOCAL_CONFIG_H
#define LOCAL_CONFIG_H      

#ifndef CONFIGURATION_H   // if main file not compiled define these 


//  __________  Common Variables  ____________
#define DEBUG       //enable debugging by default 

#ifdef DEBUG
#define Sprintln(a) (Serial.println(a))   // use to print debug errors
#define Sprint(a) (Serial.print(a))

#else

#define Sprintln(a)    // prints nothing if debug not defined
#define Sprint(a)
#endif

#define DO_ERROR_CHECKING //calculate checksum

#ifdef DO_ERROR_CHECKING
#define DO_HEAVY_ERROR_CHECKING   // parity checking each byte and checksum
//#define DO_HEADER_ERROR_CORRECTING  // hamming encoded header bytes for one bit correction
#endif

#define USE_SERIAL_TO_MEGAS
#define USE_I2C_TO_MEGAS


#define SINGLE_MATRIX_WIDTH 64
#define SINGLE_MATRIX_HEIGHT 32
#define NUM_SCREENS 4    //per side
#define TOTAL_WIDTH SINGLE_MATRIX_WIDTH * NUM_SCREENS

#define SINGLE_MATRIX_WIDTH_AS_POW_2 6    //2^6 == 64 <- useful for shifting instead of multiplying
#define SINGLE_MATRIX_HEIGHT_AS_POW_2 5   //2^5 == 32

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
//#define USING_COLOUR_SET_333
//#define USING_COLOUR_SET_444    // highest resolution without loosing data
#define USING_COLOUR_SET_888      // highest resolution, requires down sampling inputs (actual output colour might not be as expected)



#define DEFAULT_TEXT_SIZE 2

#ifdef USING_COLOUR_SET_888
#define COLOUR_MAX_LEVEL 255
#elif USING_COLOUR_SET_15
#define COLOUR_MAX_LEVEL 255
#else
#define COLOUR_MAX_LEVEL 7
#endif

#define DEFAULT_TEXT_RED_BRIGHTNESS COLOUR_MAX_LEVEL
#define DEFAULT_TEXT_GREEN_BRIGHTNESS COLOUR_MAX_LEVEL
#define DEFAULT_TEXT_BLUE_BRIGHTNESS COLOUR_MAX_LEVEL

#define DELAY_FEEBDACK_PINS


#define DEFAULT_REFRESH_RATE 20   // use this to define the interrupt rate of the matrix library



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

#define MEGA_SERIAL_BUFFER_LENGTH 32
#define MAX_TWEET_SIZE 100
#define MAX_FRAME_SIZE MAX_TWEET_SIZE+((MAX_TWEET_SIZE % MEGA_SERIAL_BUFFER_LENGTH)*FRAME_OVERHEAD)   // max amount of data to be sent in one go by either the text_frame and limit for sensor_data_frame

//#define SKIP_INTITAL_STARTUP_SEQUENCE


#define DEFAULT_MENU_WIDTH 64


                          
#endif //CONFIGURATION_H
#endif // LOCAL_CONFIG_H
