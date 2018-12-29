 
//structures used for most non trivial peripheral components and protocol elements


#ifndef Struct_H
#define Struct_H

#include "Arduino.h"
#ifndef PINS_H              //declaration in pins to avoid double declarations
#include "Pins.h"
#endif

#include "libraries.h"





struct Colour {
  int red = 7;
  int blue = 7;
  int green = 7;
  int hue = 0;
  bool use_hue = false;     //if true, use hue value, otherwise colour using rgb
};

struct Text {
  int text_width = 6;
  int text_height = 9;
  Colour colour;
};




//structures for the layout of the types of frames sent
struct Test_Frame{
  
};
Test_Frame test_frame;


#ifdef _SAM3XA_

Text text;    //generate text structure for due


/*    --------------------------------------------------------------------------------
 *     communications struct variables
 *    --------------------------------------------------------------------------------
 */

struct Frame{             //frame details for the due, seperate one for the mega below

byte frame_buffer[32];
byte frame_length = 0;
byte frame_type = 1;
byte checksum = 0;
byte num_frames = 0;
byte this_frame = 0;
};
Frame frame;



struct Nack_details{
  byte mega1_failed_frame_type = 0;   // the type of frame last sent, to recalculate
  byte mega2_failed_frame_type = 0;
  byte mega3_failed_frame_type = 0;
  byte mega4_failed_frame_type = 0;
  
  byte mega1_failed_frame_number = 0;   // the frame number that failed 
  byte mega2_failed_frame_number = 0;
  byte mega3_failed_frame_number = 0;
  byte mega4_failed_frame_number = 0;

};
Nack_details nack;


/*    --------------------------------------------------------------------------------
 *     graphics struct variables
 *    --------------------------------------------------------------------------------
 */


struct Text_cursor {        //xy coordinates of cursor position
  int x = 0;
  int y = 0;

  byte x_min = 0;
  
  bool mega1_updated = false;   //is the mega up to date with current positions
  bool mega2_updated = false;
  bool mega3_updated = false;
  bool mega4_updated = false;
};
Text_cursor text_cursor;



struct Mega_variables {           //data related to each meags current state

  int address = 0;                // set as zero, define on instanciation, different on each instance

  int delay = 0;                  // time between recieving data and updating disply
  int brightness = 255;           // current brightness of this screen
  int cursor_x = 0;               // current x coordinate
  int cursor_y = 0;               // current y coordinate
  String SD_str = "Default: no string written from sd cards";     //current string  from sd cards
  String Web_str = "Default: no string written from web page";    // current string from a webpage
  String Serial_str = "Default: no string written from serial port"; //user specified string recieved from serial
  int displayed_string = 1;       // defines which string is displayed (1:SD_str, 2:Web_str, 3: Serial_str)
  Colour colour;                  // structure of text colours
  int text_size = 2;              // text size based off adaruit gfx library standard

};
Mega_variables mega_var1;    //create four structs of mega variables
Mega_variables mega_var2;
Mega_variables mega_var3;
Mega_variables mega_var4;



/*    --------------------------------------------------------------------------------
 *     sensor and peripherals struct variables
 *    --------------------------------------------------------------------------------
 */


struct Current_Meter{

  int pin1 = CURRENT_METER_1;
  int pin2 = CURRENT_METER_2;

  int raw_reading_1 = 0;          //raw data from sensor
  int raw_reading_2 = 0;
  
  float current_reading_1 = 0;    //current reading from sensor in amps
  float current_reading_2 = 0;    

  float total = 0;                //total current in amps
  
};
Current_Meter current;


struct LDR_struct{

  int pin1 = LDR1;
  int pin2 = LDR2;

  bool LDR1_enable = true;    //enable to begin with, st false to ignore value, (eg if reading rail voltage, probably not connected)
  bool LDR2_enable = true;

  int ambient = 800; //??? set this later, used to throttle screen brightness if needed
  int large_disparity = 200;   // large enough value to assume one reading is wrong (eg sensor covered) 
  int max_reading1 = 1023;     // maximum value read on analog pin when bright light on LDR
  int min_reading1 = 0;        // minimum reading when LDR covered

  
  int max_reading2 = 1023;
  int min_reading2 = 0;  
};
LDR_struct LDR;


struct Temp_sensor{

  int pin1 = TEMP1;
  int pin2 = TEMP2;
  int pin3 = temp3;

  bool temp1_enable = true;
  bool temp2_enable = true;
  bool temp3_enable = true;

  byte dat1[5];         //data array for temp and humidity
  byte dat2[5];
  byte dat3[5];

  int temp1 = -100;   //current temperature as read by sensor, set to something rediculous to start
  int temp2 = -100;
  int temp3 = -100;

  int humidity1 = -100;   //current humidity as read by sensor
  int humidity2 = -100;
  int humidity3 = -100;
};
Temp_sensor temp;


struct Led_Strip_Struct {
  int pin = LED_STRIP;                  // attached to
  int target_brightness = 255;        // value to approach
  int current_brightness = 255;       // current value written to strip
  int change_increment = 5;           // value to increment by to approach target
  int change_interval = 40;           // interrupt period between incrementing value
  int led_stable_interval = 500;          // interrupt period when target=current brightness
  int minimum_on = 100;                  // minimum value where the leds are on
  bool led_on  = true;                    // allow led to be set on
  bool fast_interval = true;          // use change_interval if true as interrupt period, otherwise led_stable_interval
  bool sinusoidal = false;              //set true if using a sinusoidal method to change between 
  int sinusoidal_half_frequency = 1;         // time, in seconds, to go from one value to another, changing values will be a half sign wave 

};
Led_Strip_Struct led_strip;              //create led strip struct


struct Fan_Struct {
  int fan_pin = FAN_PIN;                  // pin num
  int fan_target_speed = 0 ;              // target value to approach, not necessicarily the current speed
  int fan_current_speed = 0;              // current value written to fan
  int fan_change_increment = 5;           // value to increment by to approach target
  int fan_change_interval = 40;           // milliseconds to wait between incrementing value
  int fan_minimum = 100;                  // minimum value where the leds are on
  bool fan_on  = true;                    // is fan enabled, if so, run functions

};
Fan_Struct fan;        //create fan struct


struct Encoder_Struct {   //structure for the encoder

  int pinA = PIN_A;  // Connected to CLK on KY-040
  int pinB = PIN_B;  // Connected to DT on KY-040


  volatile int PosCount = 0;       //number of pulses recieved
  volatile int position = 0;      //position = no. of pulses/2

  const int center = 0;         // center position, use as reference origin
  
  int pinALast;
  volatile int aVal;
  
  volatile bool encoder_moved = false;    // has the encoder moved, if so do functions
  bool clean_loop = false;                // interrupt will arrive mid loop, wait until end of loop check if encoder moved, 
                                          // set clean loop true, do functions on next loop then set both false when finished

};
Encoder_Struct encoder;     //create encoder struct



//data related to the button on the encoder
struct Button_Struct {

  int button_pin = BUTTON_PIN;              // pin num
  volatile bool button_pressed = false;     // has the button been pressed recently
  bool clean_loop = false;
  volatile int last_button_pressed = 0;     // when was the button last pressed
  int button_press_interval = 300;          // minimum period for button to be de-pressed to register 2 independant pressed

};
Button_Struct button;       //create button struct




/*    --------------------------------------------------------------------------------
 *     timer struct variables
 *    --------------------------------------------------------------------------------
 */

struct Timers{            //structer for all timers used and related variables
  

//Timer1 is led strip timer
// Timer2 is fan speed control timer

//Timer3 is to cycles between modes, eg display different strings peiodically, read instruction set when implented etc

bool led_strip_timer_attached = false;
bool fan_timer_attached = false;
bool pos_timer_attached = false;


};

Timers timers;



/*    --------------------------------------------------------------------------------
 *     misc struct variables
 *    --------------------------------------------------------------------------------
 */


struct SD_Strings{
  
String str_sd ="123456789 123456789 123456789 123456789 123456789 ";   //sample default string, needed to set the length correctly
String Network = "init network";    //store current network
String Password = "init password";   //store current password
 
};
SD_Strings SD_string;



#endif




#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)    // if mega        //structs for the megas

Text text;



/*    --------------------------------------------------------------------------------
 *     communications struct variables
 *    --------------------------------------------------------------------------------
 */

struct Frame{   //frame details for the mega

volatile bool frame_end = false;
char frame_str[150];
String disp_str = "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";   //<- 150 characters
volatile int str_len = 150;
volatile byte frame_length;
byte frame_type = 0;
byte frame_num = 0;
byte num_frames = 0;
byte checksum;
int offset = 0;
byte error;
int frame_len = 0;
};
Frame frame;



/*    --------------------------------------------------------------------------------
 *     graphics struct variables
 *    --------------------------------------------------------------------------------
 */


struct Line{            //structure to define verticies of arbitrary shape based on lines
  int start_x = 0;
  int start_y = 0;
  int end_x = 0;
  int end_y = 0;
};

struct Shapes{
  int type=0;
  int rect_pos_x = 0;   //position of top left corner of a rectangle
  int rect_pos_y = 0;
  int rect_width = 0;   //number of pixels to the right and down to draw the rectangle
  int rect_height = 0;
  int circle_pox_x = 0;
  int circle_pos_y =0;
  int circle_radius =0;
  Line line[8];           //array of line elements to generate arbitrary shape
  int shape_colour_r = 0;
  int shape_colour_g = 0;
  int shape_colour_b = 0;
};

struct Matrix_Objects{
  Shapes shape[10];       //array of ten shapes
  Text text;              //text structure, global structure available to both due and mega
};
Matrix_Objects matrix_objects;



#endif    //if mega




#endif // Struct_H
