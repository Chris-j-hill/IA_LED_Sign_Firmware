
#ifndef Configuration_H
#ifndef Config_Local_h
#define Config_Local_h
#include "Arduino.h"

#define USE_SERIAL_TO_MEGAS
#define USE_I2C_TO_MEGAS
#define DEBUG       //enable debugging by default 

#define NAVIGATE_MENU_OPTIONS_OVER_USB_SERIAL   //initialised serial regardless of DEBUG
#define USING_SIMPLE_BINARY_NODE_READY_PROTOCOL   // one wire protocol to establish sync boards 
#ifndef USING_SIMPLE_BINARY_NODE_READY_PROTOCOL
#define STARTUP_DUE_DELAY_PERIOD 500             //if no one wire protocol, just delay, system should be stable so should autosync after a while
#endif

#define single_matrix_width = 64;
#define single_matrix_height = 32;
#define NUM_SCREENS = 4;    //per side
#define total_width = single_matrix_width * NUM_SCREENS;

// Select if the due is enabled or if all megas enabled, if not all megas enabled, choose which
#define DUE_ENABLED                        
#define ALL_MEGAS_ENABLED

#ifndef ALL_MEGAS_ENABLED
#define MEGA_1_ENABLED true
#define MEGA_2_ENABLED true
#define MEGA_3_ENABLED true
#define MEGA_4_ENABLED true
#endif




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
#define FAN_DEFUALT_SPEED 180   

#define ENABLE_LED_STRIP
#define LED_STRIP_DEFUALT_BRIGHTNESS 255   

#define ENABLE_TEMPERATURE_SENSORS
#define ENABLE_LIGHT_SENSORS
#define ENABLE_CURRENT_METER


#ifdef DEBUG
#define Sprintln(a) (Serial.println(a))   // use to print debug errors
#define Sprint(a) (Serial.print(a))

#else

#define Sprintln(a)    // prints nothing if debug not defined
#define Sprint(a)
#endif



//data values of each parameter 
byte current1 = 2;
byte current2 = 3;
byte temp1 = 4;
byte temp2 = 5;
byte temp3 = 6;
byte light1 =7;
byte light2 =8;


bool ethernet_connected = true;
bool wifi_connected = true;
byte screen_brightness = 255;
byte text_size = DEFAULT_TEXT_SIZE;
byte text_colour_r = DEFAULT_TEXT_RED_BRIGHTNESS;
byte text_colour_g = DEFAULT_TEXT_GREEN_BRIGHTNESS;
byte text_colour_b = DEFAULT_TEXT_BLUE_BRIGHTNESS;
byte text_colour_hue_MSB = 0;   //value between +/- 1536 
byte text_colour_hue_LSB = 0;
bool use_hue = true;

int  text_colour_hue = 1000;

// list of valid sensor prefix's for sending non string data to the megas.
// append this as required and add to switch statements in due and mega code
const byte to_mega_prefix_array[] = {10, 11, 20, 21, 22, 30, 31, 40, 50, 60, 61, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180};  


byte mega_last_sent_time = 0;
bool send_text_now = false;
volatile bool send_pos_now = false;   //variable set in interrupt to trigger send pos function in main loop. (serial doesnt work in interrutps)



serial_tc5_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);
auto& Serial_1=serial_tc5;
serial_tc6_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);
auto& Serial_2=serial_tc6;
serial_tc7_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);
auto& Serial_3=serial_tc7;
serial_tc8_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);
auto& Serial_4=serial_tc8;




bool enable_fans = true;       // variables to initialise specific functionality
bool enable_led_strip = true;
bool enable_encoder = true;
bool enable_button = true;
bool enable_sd_cards = false;
bool enable_current_meter = true;
bool enable_temp_sensor = false;
bool enable_LDR = false;
bool enable_ethernet = false;
bool enable_wifi = false;
bool test_i2c = false;

bool fans_enabled = false;            //verification variables, if something failes to initialise code may continue (eg wifi), use this to not run related code later
bool led_strip_enabled = false;
bool encoder_enabled = false;
bool button_enabled = false;
bool sd_cards_enabled = false;
bool current_meter_enabled = false;
bool temp_sensor_enabled = false;
bool LDR_enabled = false;
bool ethernet_enabled = false;
bool wifi_enabled = false;


int notifier = NOTIFIER;      //led pin, set to indicate data stuff



#ifndef ALL_MEGAS_ENABLED
bool mega_enabled[4] = {MEGA_1_ENABLED, MEGA_2_ENABLED, MEGA_3_ENABLED, MEGA_4_ENABLED};  
#else
bool mega_enabled[4] = {true, true, true, true};  // ignore communication if other board is false
#endif

bool enable_serial = true;   //select if comms is enabled, may be useful to disable for testing peripherals
bool enable_i2c = false;    //not using anymore dont initialise




bool serial_enabled = false;
bool i2c_enabled = false;

byte screen_mode=0;   //mode of operation on startup should be both displaying
                      //mode0: both on
                      //mode1: one side on
                      //mode2: both off
                      //mode3: other side on



#endif // Config_Local_h
#endif // Configuration_H
