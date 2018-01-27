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

//#define DO_ERROR_CHECKING

#define USE_SERIAL_TO_MEGAS
#define USE_I2C_TO_MEGAS


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
#endif

#define USING_COLOUR_SET_333
//#define USING_COLOUR_SET_444    // NOT FINISHED IMPLEMENTATION


#define DEFAULT_TEXT_SIZE 2
#define DEFAULT_TEXT_RED_BRIGHTNESS 7
#define DEFAULT_TEXT_GREEN_BRIGHTNESS 7
#define DEFAULT_TEXT_BLUE_BRIGHTNESS 7

#define DELAY_FEEBDACK_PINS


#define DEFAULT_REFRESH_RATE 20   // use this to define the interrupt rate of the matrix library



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
#define PREFIX_LED_STRIP_BRIGHTNESS     50
#define PREFIX_SD1_DETECTED             60
#define PREFIX_SD2_DETECTED             61
#define PREFIX_EHTERNET_CONNECTED       70
#define PREFIX_WIFI_CONNECTED           80
#define PREFIX_SCREEN_BRIGHTNESS        90
#define PREFIX_TEXT_SIZE                100
#define PREFIX_TEXT_COLOUR_R            110
#define PREFIX_TEXT_COLOUR_G            120
#define PREFIX_TEXT_COLOUR_B            130
#define PREFIX_TEXT_HUE_MSB             140
#define PREFIX_TEXT_HUE_LSB             150
#define PREFIX_TEXT_USE_HUE             160
#define PREFIX_DEBUG_STATE              170
#define PREFIX_SCREEN_MODE              180




#define MEGA_SERIAL_BUFFER_LENGTH 32
#define MAX_TWEET_SIZE 280
#define MAX_FRAME_SIZE MAX_TWEET_SIZE+((MAX_TWEET_SIZE % MEGA_SERIAL_BUFFER_LENGTH)*FRAME_OVERHEAD)   // max amount of data to be sent in one go by either the text_frame and limit for sensor_data_frame

//#define SKIP_INTITAL_STARTUP_SEQUENCE


                          
#endif //CONFIGURATION_H
#endif // LOCAL_CONFIG_H
