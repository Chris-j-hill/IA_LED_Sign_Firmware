
//create all global variables here. main code file should be clean

#ifndef Vars_H
#define Vars_H

#include "Pins.h"     //inlcude pins if not included already

#include "libraries.h"


#define DEBUG       //comment this out to dissable debugging

#ifdef DEBUG
  #define Sprintln(a) (Serial.println(a))   // use to print debug errors
  #define Sprint(a) (Serial.print(a))

#else

  #define Sprintln(a)    // prints nothing if debug not defined
  #define Sprint(a) 
#endif

bool due_enabled = true;                        // define if the board is to be used. Halt execution if this board is false
bool mega_enabled[4] = {true,true,true, true};  // ignore communication if other board is false

// mega_enabled[0] = true;
// mega_enabled[1] = true;
// mega_enabled[2] = true;
// mega_enabled[3] = true;

bool debug = true;         //debug mode, if true relay program info to serial monitor, assumes enable_serial serial = true
bool enable_serial = true;   //select if comms is enabled, may be useful to disable for testing peripherals
bool enable_i2c = false;    //not using anymore dont initialise




bool serial_enabled = false;
bool i2c_enabled = false;

byte screen_mode=0;   //mode of operation on startup should be both displaying
                      //mode0: both on
                      //mode1: one side on
                      //mode2: both off
                      //mode3: other side on

// prefixes for sending sensor data iover i2c. useful to display current values on screen
// send data as a set of two byte, use prefix in first byte, and data is second value
// the prefix byte is structured as the the first two characters in a the character number (ie 123 -> 12)
// define the type of sensor info, and the third characters is the sensor number of the defines type
// (ie 123 ->3). 

// #define CURRENT_PREFIX                  1
// #define TEMP_PREFIX                     2
// #define LIGHT_PREFIX                    3
// #define FAN_PREFIX                      4
// #define LED_STRIP_PREFIX                5
// #define SD_DETECTED_PREFIX              6
// #define ETHERNET_CONNECTED_PREFIX       7
// #define WIFI CONNECTED_PREFIX           8
// #define SCREEN_BRIGHTNESS_PREFIX        10
// #define TEXT_SIZE_PREFIX                11
// #define TEXT_COLOUR_R_PREFIX            12
// #define TEXT_COLOUR_G_PREFIX            13
// #define TEXT_COLOUR_B_PREFIX            14
// #define TEXT_COLOUR_HUE_MSB_PREFIX      15
// #define TEXT_COLOUR_HUE_MSB_PREFIX      16
// #define USE_HUE_PREFIX                  17
// #define DUBUG_PREFIX                    18

// #define OBJECT_PREFIX                   23    // specify an object to reference when updating its characteristics, ignore range, 
//                                               // object identified as prefix byte - 230(allows up to 25 objects

// #define PREFIX_RANGE    10    // value specifying range the prefix can cover. (ie 10 means each prefix can account for 10 value)

#define OVERHEAD 5        //number of overhead bytes -> frame length, frame type, num frames, frame num, checksum


const char dummy_frame[] = "this is a test60";   //this is probably wrong, specify string that is full frame


#define COMS_SPEED 38400         //speed of coms between due and megas when using serial



#if defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)     //mega variables

int address = 0;

//set these to correct locations later
int x_pos = 0;
int y_pos = 0;
byte x_pos_LSB = 0;
byte x_pos_MSB = 0;
byte y_pos_LSB = 0;
byte y_pos_MSB = 0;
byte x_pos_dir = 0;
byte y_pos_dir = 0;
byte comms_delay = 0;
byte pos_update_freq = 0;

byte temp1 = 0;
byte temp2 = 0;
byte temp3 = 0;
byte light1 = 0;
byte light2 = 0;
byte current1 = 0;
byte current2 = 0;
byte current3 = 0;
byte fan_speed = 0;
byte led_strip_brightness = 0;
bool sd_card1_detected = 0;
bool sd_card2_detected = 0;
bool ethernet_connected = 0;
bool wifi_connected = 0;
byte screen_brightness = 255;
byte text_size = 1;
byte text_colour_r = 1;
byte text_colour_g = 1;
byte text_colour_b = 1;
byte text_colour_hue_MSB = 0;   //value between +/- 1536 (god knows why)
byte text_colour_hue_LSB = 0;
bool use_hue = true;
//bool debug = 0;
int  text_colour_hue = 0;



#else           // due variables

byte mega_last_sent_time = 0;
bool send_text_now = false;

volatile bool send_pos_now = false;   //variable set in interrupt to trigger send pos function in main loop. (serial doesnt work in interrutps)

// variables for initialising soft serial for comms
using namespace arduino_due;

#define SOFT_UART_BIT_RATE COMS_SPEED // 57600 38400 1200 19200 9600 115200 300
#define RX_BUF_LENGTH 256 // software serial port's reception buffer length
#define TX_BUF_LENGTH 256 // software serial port's transmision buffer length


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


//sd card objects,
SdFat sd_external;            //external sd card

SdFat sd_internal;            //onboard sd card

SdFile file1;                 // file handling objects, use to address two open files at once
SdFile file2;                 // nb: if modifying files, make sure to close at the end of function

const uint8_t SD2_CS = SD1_ENABLE;   // chip select for sd_internal
const uint8_t SD1_CS = SD2_ENABLE;  // chip select for sd_external

const uint8_t BUF_DIM = 100;
uint8_t buf[BUF_DIM];

char read_buffer[67];       //buffer to read some data, dont need to read whole file at once, and doing so could be problematic if file large,
//read 15 bytes to recognise id word (eg Network) and have 50 bytes for string (default could be long)

// print error msg, any SD error codes, and halt.
// store messages in flash
#define errorExit(msg) errorHalt_P(PSTR(msg))
#define initError(msg) initErrorHalt_P(PSTR(msg))

// put sd card file name strings here:

const char *sd_ext_dir = "EXTERNAL";
const char *sd_int_dir = "INTERNAL";

const char *sd_ext_file = "Networks.BIN";
const char *sd_int_file = "Networks.BIN";

//const char *sd_ext_file2 = "Instructions.BIN";      //not implemented
//const char *sd_int_file2 = "Instructions.BIN";
//
//const char *sd_ext_file3 = "bitmap.BIN";
//const char *sd_int_file3 = "bitmap.BIN";

// configure lrd tool
bool use_button = false;    //in configuring ldr, accept button as input when true, only config over serial otherwise


char text_str[150] = "this is a test4 this is a test5 this is a test6";

//int x_pos = 0;      //actual coordinate
//int y_pos = 0;



//pos frame variables

byte x_pos_LSB = 0;   
byte x_pos_MSB = 0;
byte y_pos_LSB = 0;
byte y_pos_MSB = 0;
byte x_pos_dir = 129;   //direction and speed of the cursor
byte y_pos_dir = 127;
byte comms_delay = 0;
byte pos_update_freq = 5;
byte pos_frame_length = 13;   //length of frame to transmit to update pos



//write these to actual locations
byte current1 = 2;
byte current2 = 3;
byte temp1 = 4;
byte temp2 = 5;
byte temp3 = 6;
byte light1 =7;
byte light2 =8;
byte fan_speed = 9;
byte led_strip_brightness = 10;
bool sd_card1_detected = true;
bool sd_card2_detected = true;
bool ethernet_connected = true;
bool wifi_connected = true;
byte screen_brightness = 255;
byte text_size = 11;
byte text_colour_r = 12;
byte text_colour_g = 13;
byte text_colour_b = 14;
byte text_colour_hue_MSB = 0;   //value between +/- 1536 (god knows why)
byte text_colour_hue_LSB = 0;
bool use_hue = true;

int  text_colour_hue = 1000;

// list of valid sensor prefix's for sending non string data to the megas.
// append this as required and add to switch statements in due and mega code
const byte to_mega_prefix_array[] = {10, 11, 20, 21, 22, 30, 31, 40, 50, 60, 61, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180};  



#endif

#endif
