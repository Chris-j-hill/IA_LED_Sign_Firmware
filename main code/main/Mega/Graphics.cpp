
#ifndef GRAPHICS_CPP
#define GRAPHICS_CPP

#include "Graphics.h"
#include "Mega_Pins.h"
#include "src/Timer3/TimerThree.h"
#include "src/Timer4/TimerFour.h"
#include "Menus.h"
#include "Local_Config.h"
#include "Coms_Serial.h"

#ifdef USE_CUSTOM_RGB_MATRIX_LIBRARY
#include "src/customRGBMatrixPanel/customRGBmatrixPanel.h"
#else
#include "RGBmatrixPanel.h"
#endif


Text_Struct text_parameters[MAX_NUM_OF_TEXT_OBJECTS];
Screen_Struct screen_parameters;
Cursor_Struct cursor_parameters[MAX_NUM_OF_TEXT_OBJECTS];
Object_Struct_Circles startup_ring;   //general purpose object parameters, so as not to overwrite text parameters when displaying several things at once

extern struct Menu_tree_items menu_items;
extern struct Menu_Tree_Item_Lengths menu_tree_item_lengths;
extern struct Menu_colour_struct    menu_option_colour;
extern struct Menu_colour_struct    title_colour;
extern struct Menu_Struct           menu_parameters;

extern Graphics graphics;
extern RGBmatrixPanel matrix;
extern Menu menu;
extern Coms_Serial coms_serial;

//volatile byte x_pos_ISR_counter = 0;
//volatile byte x_pos_isr_counter_overflow = 255;
//
//volatile byte y_pos_ISR_counter = 0;
//volatile byte y_pos_isr_counter_overflow = 255;

//volatile bool suppress_x_pos_update = false;
//volatile bool suppress_y_pos_update = false;

//extern byte menu_width;

extern bool menu_visible;

volatile bool interpolate_pos_flag = false;

const char plus_string[] PROGMEM = "+";
const char minus_string[] PROGMEM = "-";

/*
   define locations of data in array below, for
   abstraction away from non descript indexing in
   funcions later
*/

#define MAIN_MENU_LOC           0
#define RETURN_LOC              1
#define SCREEN_MODE_LOC         2
#define SCREEN_BRIGHTNESS_LOC   3
#define TEXT_SETTINGS_LOC       4
#define FAN_SETTINGS_LOC        5
#define INTERNET_CONFIG_LOC     6
#define SD_CARD_LOC             7

#define LED_STRIP_LOC           8
#define BOTH_ON_LOC             9
#define FRONT_ON_LOC            10
#define BACK_ON_LOC             11
#define BOTH_OFF_LOC            12

#define TEXT_SIZE_LOC           13
#define TEXT_COLOUR_LOC         14
#define SCROLL_SPEED            15
#define FLIP_DIR_LOC            16

#define FAN_SPEED_LOC           17
#define FAN_MIN_SPEED_LOC       18

//#define internet_config_menu    19
//#define select_network_manually 20
//#define ethernet_enable         21
//#define ethernet_disable        22
//#define wifi_enable             23
//#define wifi_disable            24

#define SD_FOLDERS_LOC          25

#define STRIP_BRIGHTNESS_LOC    26

#define RED_LOC                 27
#define GREEN_LOC               28
#define BLUE_LOC                29
#define HUE_LOC                 30
#define USE_HUE_LOC             31
#define USE_RGB_LOC             32

#define SCROLL_X_LOC            33
#define SCROLL_Y_LOC            34

#define ENABLE_LOC              35
#define DISABLE_LOC             36


#define EMPTY_STRING_LOC        37
#define DEFAULT_STRING_LOC      38
#define DEFAULT_TITLE_LOC       39



/*
   Need to define these as global variables not in struct as loading bytes from progmem
   doesnt seem to like struct formatting. no sram used so no big deal to double define,
   saves rewriting functions containg struct variables. code derived from array of strings
   examlple here: https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
*/


//level 1 menu items
const char main_menu[]                PROGMEM           = MAIN_MENU_STRING;
const char RETURN[]                   PROGMEM           = RETURN_STRING;
const char screen_mode[]              PROGMEM           = SCREEN_MODE_STRING;
const char brightness[]               PROGMEM           = SCREEN_BRIGHTNESS_STRING;
const char text_settings[]            PROGMEM           = TEXT_SETTINGS_STRING;
const char fan_settings[]             PROGMEM           = FAN_SETTINGS_STRING;
const char internet_settings[]        PROGMEM           = INTERNET_CONFIG_STRING;
const char sd_card_settings[]         PROGMEM           = SD_CARDS_STRING;
const char led_strip_settings[]       PROGMEM           = LED_STRIP_STRING;


//level 2 menu items
//change mode folder
const char screen_mode0[]             PROGMEM           = BOTH_ON_STRING;
const char screen_mode1[]             PROGMEM           = FRONT_SIDE_STRING;
const char screen_mode3[]             PROGMEM           = BACK_SIDE_STRING;
const char screen_mode2[]             PROGMEM           = BOTH_OFF_STRING;

//text adjustment folder
const char text_size_settings[]       PROGMEM           = TEXT_SIZE_STRING;
const char text_colour_settings[]     PROGMEM           = TEXT_COLOUR_STRING;
const char scroll_speed_settings[]    PROGMEM           = SCROLL_SPEED_STRING;
const char flip_dir_settings[]        PROGMEM           = FLIP_DIR_STRING;

//fan settings folder
const char fan_speed_settings[]       PROGMEM           = FAN_SPEED_STRING;
const char minimum_rotating_speed[]   PROGMEM           = FAN_MIN_SPEED_STRING;

// internet configuration folder
const char internet_config_menu[]     PROGMEM           = "Internet";
const char select_network_manually[]  PROGMEM           = "Connect To...";
const char ethernet_enable[]          PROGMEM           = "Enable Eth";
const char ethernet_disable[]         PROGMEM           = "Disable Eth";
const char wifi_enable[]              PROGMEM           = "Enable Wifi";
const char wifi_disable[]             PROGMEM           = "Disable Wifi";

// SD cards folder
const char sd_cards_menu[]            PROGMEM           = SD_CARDS_STRING;
const char sd_card_folders[]          PROGMEM           = READ_FOLDERS_STRING;

// Led Strip folder
const char led_strip_menu[]           PROGMEM           = LED_STRIP_STRING;
const char led_strip_brightness[]     PROGMEM           = STRIP_BRIGHTNESS_STRING;


//level 3 folders

const char text_colour_red[]          PROGMEM           = RED_STRING;
const char text_colour_green[]        PROGMEM           = GREEN_STRING;
const char text_colour_blue[]         PROGMEM           = BLUE_STRING;
const char text_colour_hue[]          PROGMEM           = HUE_STRING;
const char text_colour_use_hue[]      PROGMEM           = USE_HUE_STRING;
const char text_colour_use_rgb[]      PROGMEM           = USE_RGB_STRING;

// scroll speed
const char scroll_speed_x[]           PROGMEM           = X_DIR_STRING;
const char scroll_speed_y[]           PROGMEM           = Y_DIR_STRING;

const char enable[]                   PROGMEM           = ENABLE_STRING;
const char disable[]                  PROGMEM           = DISABLE_STRING;

//dummy string
const char null_string[]              PROGMEM           = EMPTY_STRING;
const char default_string[]           PROGMEM           = CONTENT_ERROR_STRING; // default in write_menu_option function
const char default_title[]            PROGMEM           = TITLE_ERROR_STRING; // default in write_menu_option function


const char* const menu_string_table[] PROGMEM = { //create array of const char arrays rather than struct
  /*0*/  main_menu,
  /*1*/  RETURN,
  /*2*/  screen_mode,
  /*3*/  brightness,
  /*4*/  text_settings,
  /*5*/  fan_settings,
  /*6*/  internet_settings,
  /*7*/  sd_card_settings,
  /*8*/  led_strip_settings,

  /*9*/  screen_mode0,
  /*10*/  screen_mode1,
  /*11*/  screen_mode3,
  /*12*/  screen_mode2,

  /*13*/  text_size_settings,
  /*14*/  text_colour_settings,
  /*15*/  scroll_speed_settings,
  /*16*/  flip_dir_settings,

  /*17*/  fan_speed_settings,
  /*18*/  minimum_rotating_speed,

  /*19*/  internet_config_menu,
  /*20*/  select_network_manually,
  /*21*/  ethernet_enable,
  /*22*/  ethernet_disable,
  /*23*/  wifi_enable,
  /*24*/  wifi_disable,

  /*25*/  sd_card_folders,

  /*26*/  led_strip_brightness,

  /*27*/  text_colour_red,
  /*28*/  text_colour_green,
  /*29*/  text_colour_blue,
  /*30*/  text_colour_hue,
  /*31*/  text_colour_use_hue,
  /*32*/  text_colour_use_rgb,

  /*33*/  scroll_speed_x,
  /*34*/  scroll_speed_y,

  /*35*/  enable,
  /*36*/  disable,

  /*37*/  null_string,
  /*38*/  default_string,
  /*39*/  default_title
};




void pos_update_ISR() {
  //  interpolate_pos_flag = true;
  graphics.interpolate_pos();
}


void Graphics::init_matrix() {
  matrix.begin();
  matrix.setTextWrap(false); // Allow text to run off right edge

  init_menu_title_colour();
  init_menu_option_colour();
}


void Graphics::init_update_display_isr() {

  uint32_t period = serial_check_ISR_period();

  Timer4.initialize(period);
  Timer4.attachInterrupt(serial_check_ISR);
  Timer4.start();
}


void serial_check_ISR() {
  /*
     fast running isr to check the serial port. a lot of the operations on the led panel require
     large amounts of data to be set. this is going to be slow, especially if your using a not
     insignificant part of the screen. on the other hand, we also want the serialport to be operating
     at as high a frequency as is possible. Thus simply polling the serial port now and again isnt
     enough, the internal buffers are configured to be small due to space restraints and thus will
     overflow if not attended to quickly after recieving a byte. thus we need an interrupt for the serial,
     which will check the serial port for recieved data very frequently to avoid a buffer overflow.
     However, in an ISR, all interrupts get disabled until we finish the ISR, but serial commands (and delays)
     require interrupts to work. Hmmm... solution is to detach this interrupt, to prevent it interrupting
     itself, along with all non critical interrupts, and reinable interrupts withing the ISR. this allows us
     to use serial functions without processor jamming. at end of function we must then disable interrupts, and
     reattach all the interrupts we disabled. initial testing seems to indicate all ok
  */

  //  TIMSK1 &= ~(1 << TOIE1); //turn off all active interrupts
  Timer3.stop();
  Timer4.stop();

  interrupts(); // <- aw geez rick, i dont know about this...

  if (Serial_1.available() != 0)
    coms_serial.read_buffer();


  noInterrupts(); //stop all interrupts

  Timer3.start(); //enable these, once out of isr interrupts will be enabled, then these can be occur if needed
  Timer4.start();
  //  TIMSK1 |= (1 << TOIE1);
}

void Graphics::update_display() {

  static uint32_t time_since_last_update = millis();

  if (screen_parameters.time_last_updated == 0)
    screen_parameters.time_last_updated = millis(); //if it was never set yet, set it here

  if ((millis() > time_since_last_update + MIN_DISPLAY_UPDATE_PERIOD) && (!screen_parameters.updated) && (millis() > screen_parameters.time_last_updated + SCREEN_UPDATE_BACKOFF_PERIOD)) {

    screen_parameters.updated = true;

    Timer3.stop();  //disable the interpolate pos timer
    set_display_mode();


    //if (menu.get_current_menu() == DEFAULT_MENU) menu.set_current_menu(MAIN_MENU);

    matrix.fillScreen(0);

    if (menu.get_current_menu() == DEFAULT_MENU) {

      for (byte i = 0; i < MAX_NUM_OF_TEXT_OBJECTS; i++) {

        //        Serial.print(F("Printing string "));
        //        Serial.print(i);
        //        Serial.print(F(" = "));
        //        for (byte j = 0; j < MAX_TWEET_SIZE; j++) {
        //          Serial.print((char)text_parameters[i].string[j]);
        //        }
        //
        //        Serial.println();

        if (text_parameters[i].object_used) {
          if (text_parameters[i].use_hue)
            graphics.set_text_colour(text_parameters[i].hue);
          else
            graphics.set_text_colour(text_parameters[i].colour_r, text_parameters[i].colour_g, text_parameters[i].colour_b);

          graphics.draw_text(i);
          //          Serial.println("draw");
        }
      }
    }

    else if (menu.get_current_menu() == STARTUP) {
      matrix.setCursor(2, 10);
      matrix.setTextColor(matrix.Color333(5, 4, 3));
      matrix.print("starting...");
    }

    //    else if((menu.get_current_menu() != DEFAULT_MENU && menu.get_current_menu() != STARTUP_MENU) && !screen_parameters.updated) //<- partial coverage of menu...

    else if (menu.get_current_menu() != DEFAULT_MENU) // draw menu
      menu.display_menu();  //update any menu if applicable

    matrix.swapBuffers(false);  //push the buffer we just wrote to front

    time_since_last_update = millis();
    //dealt with screen update

    Timer3.start();
  }
}

inline void Graphics::set_display_mode() {
  //only do this if mode was changed. mode method is quick so not much gian, but even so...
  if (screen_parameters.new_mode != screen_parameters.cur_mode) {
    screen_parameters.cur_mode = screen_parameters.new_mode;
    matrix.Mode(screen_parameters.new_mode);
  }
}

inline void Graphics::set_text_colour(byte new_r, byte new_g, byte new_b) {

  //limit colour range on off chance number is still wrong
  if (new_r > COLOUR_MAX_LEVEL)
    new_r = COLOUR_MAX_LEVEL;
  if (new_g > COLOUR_MAX_LEVEL)
    new_g = COLOUR_MAX_LEVEL;
  if (new_b > COLOUR_MAX_LEVEL)
    new_b = COLOUR_MAX_LEVEL;

  //scale colour value with brightness
  float multiplier = screen_parameters.brightness / 100;
  new_r = new_r * multiplier; //scale by this
  new_g = new_g * multiplier;
  new_b = new_b * multiplier;

  //Serial.print("red: ");
  //Serial.println(new_r);
  //Serial.print("green: ");
  //Serial.println(new_g);
  //Serial.print("blue: ");
  //Serial.println(new_b);

#if defined(USING_COLOUR_SET_888)
  matrix.setTextColor(matrix.Color888(new_r, new_g, new_b));
#elif defined(USING_COLOUR_SET_444)
  matrix.setTextColor(matrix.Color444(new_r, new_g, new_b));
#else
  matrix.setTextColor(matrix.Color333(new_r, new_g, new_b));
#endif
}

inline void Graphics::set_text_colour(int new_hue) {

  if (new_hue > HUE_MAX_LEVEL)
    new_hue = HUE_MAX_LEVEL;
  else if (new_hue < HUE_MIN_LEVEL)
    new_hue = HUE_MIN_LEVEL;

  //
  //Serial.print("hue: ");
  //Serial.println(new_hue);

  byte multiplier = map(screen_parameters.brightness, 0, 100, 0, 255);

  matrix.setTextColor(matrix.ColorHSV(new_hue, 255, multiplier, true));
}

inline void Graphics::draw_text(byte obj_num) {

  //
  //  uint32_t temp = millis();
  //  temp = millis() - temp;
  //  uint32_t start_time = millis();

  //  int cursorx = 1;
  //  int cursory = 1;
  //  if (obj_num == 1)
  //    cursory = 15;

  int cursorx = cursor_parameters[obj_num].local_x_pos;
  int cursory = cursor_parameters[obj_num].local_y_pos;

  //  Serial.print(F("obj num "));
  //  Serial.print(obj_num);
  //  Serial.print("\t");
  //  Serial.print("X = ");
  //  Serial.print(cursorx);
  //  Serial.print("\t");
  //  Serial.print("Y = ");
  //  Serial.println(cursory);

  //    byte text_size = text_parameters[obj_num].text_size;
  byte text_size = 1;
  matrix.setTextSize(text_size);
  //  matrix.setCursor(cursor_parameters[obj_num].local_x_pos, cursor_parameters[obj_num].local_y_pos);

  //reduce string down to only that which could be displayed
  byte num_chars = SINGLE_MATRIX_WIDTH / (text_size * ASCII_CHARACTER_BASIC_WIDTH) + 2;   // +2 for case of char not aligned with egdes and null char at end
  char sub_char_array[num_chars] = {'\0'};

  byte start_loc = 0;

  if (cursorx < (0 - ASCII_CHARACTER_BASIC_WIDTH)) {
    start_loc = abs(cursorx / ASCII_CHARACTER_BASIC_WIDTH);
    matrix.setCursor(cursorx % ASCII_CHARACTER_BASIC_WIDTH, cursory); //move cursor closer to x=0 and ignore earlier characters
  }
  else if (cursorx >= SINGLE_MATRIX_WIDTH) //dont print if fully off screen
    return;
  else
    matrix.setCursor(cursorx, cursory);

  //populate sub char array
  if (start_loc < MAX_TWEET_SIZE) { // only if could possibly be on screen
    for (byte i = 0; i < num_chars; i++) {
      if ((i + start_loc) == MAX_TWEET_SIZE) break;
      sub_char_array[i] = (char)text_parameters[obj_num].string[i + start_loc];
    }
  }

  //make array into a string
  String str((char*)sub_char_array);
  //  Serial.print(F("sub string = "));
  //  Serial.println(str);

  TIMSK1 &= ~(1 << TOIE1);  //disable timer 1 interrupt (timer for screen)
  for (byte i = 0; i < MAX_TWEET_SIZE; i++) {
    if (text_parameters[obj_num].string[i] == 0) break;
    matrix.print(str[i]);
  }

  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt

  //  Serial.print(F("time taken = "));
  //  Serial.println(millis() - start_time - temp);
}



void Graphics::attach_pos_ISR() {

  uint32_t period = pos_isr_period();
  //  pos_isr_counter_overflow();

  Timer3.initialize(period);
  Timer3.attachInterrupt(pos_update_ISR);
  Timer3.start();
}

inline void Graphics::set_pos_isr_period(uint32_t period) {
  Timer3.setPeriod(1000 * period); //period calculated as millis, function needs micros
  //  Serial.print("set_pos_isr : ");
  //  Serial.println(period);
  //  delay(10);

}

void Graphics::interpolate_pos() {


  //  if (interpolate_pos_flag) {

  interpolate_pos_flag = false;
  uint32_t cur_time = millis(); //only need to do this once, millis isnt that fast...

  if (menu.get_current_menu()  == DEFAULT_MENU)  //if displaying any menu, dont allow this to cause screen update
    screen_parameters.updated = false;
  else
    screen_parameters.updated = true;

  //  Serial.println(F("Interpolate pos"));
  for (byte i = 0; i < MAX_NUM_OF_TEXT_OBJECTS; i++) {

    if (text_parameters[i].object_used) {
      //check if period elapsed, if so it is time to update
      if ((cur_time - screen_parameters.last_updated[2 * i]) > screen_parameters.update_periods[2 * i]) {
        screen_parameters.last_updated[2 * i] = cur_time;

        //increment value, speed now irrelevant, since this interrupt should run the required num times per second, so just increment by one
        if (cursor_parameters[i].x_dir < 0)       cursor_parameters[i].local_x_pos--;
        else if (cursor_parameters[i].x_dir > 0)  cursor_parameters[i].local_x_pos++;

      }

      if ((cur_time - screen_parameters.last_updated[(2 * i) + 1]) > screen_parameters.update_periods[(2 * i) + 1]) {
        screen_parameters.last_updated[(2 * i) + 1] = cur_time;

        if (cursor_parameters[i].y_dir < 0)       cursor_parameters[i].local_y_pos--;
        else if (cursor_parameters[i].y_dir > 0)  cursor_parameters[i].local_y_pos++;

      }
    }
  }
  //  }
}

//void Graphics::delay_pos_ISR(int value, byte counter) {
//
//  if (counter == 1) { // increment x
//    if (x_pos_ISR_counter + value < 0 ) { //decreasing counter and near 0
//      value += x_pos_ISR_counter;
//      x_pos_ISR_counter = x_pos_isr_counter_overflow + value;
//      suppress_x_pos_update = true;
//    }
//    else if (x_pos_ISR_counter + value >= x_pos_isr_counter_overflow) { //account for overflow,also incrment cursor manually
//      value -= (x_pos_isr_counter_overflow - x_pos_ISR_counter);
//      x_pos_ISR_counter = value;
//      this -> increment_cursor_position(1);
//    }
//    else
//      x_pos_ISR_counter += value; // just move the
//  }
//
//  else if (counter == 2) { // increment y
//    if (y_pos_ISR_counter + value < 0 ) { //decreasing counter and near 0
//      value += y_pos_ISR_counter;
//      y_pos_ISR_counter = y_pos_isr_counter_overflow + value;
//      suppress_y_pos_update = true;
//    }
//    else if (y_pos_ISR_counter + value >= y_pos_isr_counter_overflow) { //account for overflow,also increment cursor manually
//      value -= (y_pos_isr_counter_overflow - y_pos_ISR_counter);
//      x_pos_ISR_counter = value;
//      this -> increment_cursor_position(2);
//    }
//    else
//      y_pos_ISR_counter += value; // just move the
//
//  }
//}

void Graphics::increment_cursor_position(byte axis, byte obj_num) {

  if (axis == 1) {
    if (cursor_parameters[obj_num].x_dir < 0) {
      cursor_parameters[obj_num].global_x_pos --;
      cursor_parameters[obj_num].local_x_pos --;
    }
    else if (cursor_parameters[obj_num].x_dir > 0) {
      cursor_parameters[obj_num].global_x_pos ++;
      cursor_parameters[obj_num].local_x_pos ++;
    }
  }

  else if (axis == 2) {
    if (cursor_parameters[obj_num].y_dir < 0) {
      cursor_parameters[obj_num].global_y_pos --;
      cursor_parameters[obj_num].local_y_pos --;
    }
    else if (cursor_parameters[obj_num].y_dir > 0) {
      cursor_parameters[obj_num].global_y_pos ++;
      cursor_parameters[obj_num].local_y_pos ++;
    }
  }
}

inline uint32_t Graphics::pos_isr_period() {

  return (1000000 / POS_ISR_FREQUENCY);// number of microseconds between functions calls

}

//void Graphics::pos_isr_counter_overflow() {
//
//  x_pos_isr_counter_overflow = abs(cursor_parameters[0].x_dir) * 2; // *2 doubles range to increase of overflow to improve achievable scroll speed
//  y_pos_isr_counter_overflow = abs(cursor_parameters[0].y_dir) * 2;
//
//}


void Graphics::draw_ring(byte x_center, byte y_center, uint16_t radius) {

  int16_t local_x;

  switch (screen_parameters.node_address) {
    case 0:   local_x = x_center; break;                    // global top left is this screens top left
    case 1:   local_x = x_center - SINGLE_MATRIX_WIDTH;     // global top left is -64
    case 2:   local_x = x_center - (2 << SINGLE_MATRIX_WIDTH_AS_POW_2); // global top left is -128
    case 3:   local_x = x_center - (3 << SINGLE_MATRIX_WIDTH_AS_POW_2);
  }


#if  defined(USING_COLOUR_SET_888)
  matrix.drawCircle(local_x, y_center, radius, matrix.Color888(startup_ring.red, startup_ring.green, startup_ring.blue));
#elif defined(USING_COLOUR_SET_444)
  matrix.drawCircle(local_x, y_center, radius, matrix.Color444(startup_ring.red, startup_ring.green, startup_ring.blue));
#else
  matrix.drawCircle(local_x, y_center, radius, matrix.Color333(startup_ring.red, startup_ring.green, startup_ring.blue));
#endif
}

void Graphics::set_object_colour(byte new_r, byte new_g, byte new_b) {
  startup_ring.red = new_r;
  startup_ring.blue = new_b;
  startup_ring.green = new_g;
}


void Graphics::init_menu_title_colour() {
  title_colour.red = MENU_TITLE_R;
  title_colour.green = MENU_TITLE_G;
  title_colour.blue = MENU_TITLE_B;
}

void Graphics::init_menu_option_colour() {
  menu_option_colour.red = MENU_OPTION_R;
  menu_option_colour.green = MENU_OPTION_G;
  menu_option_colour.blue = MENU_OPTION_B;

}


void Graphics::set_title_colour() {

#if defined(USING_COLOUR_SET_888)
  matrix.setTextColor(matrix.Color888(title_colour.red, title_colour.green, title_colour.blue));
#elif defined(USING_COLOUR_SET_444)
  matrix.setTextColor(matrix.Color444(title_colour.red, title_colour.green, title_colour.blue));
#else
  matrix.setTextColor(matrix.Color333(title_colour.red, title_colour.green, title_colour.blue));
#endif
}


void Graphics::set_menu_colour() {

  //Serial.print("red :");
  //Serial.println(menu_option_colour.red);
  //Serial.print("greeen :");
  //Serial.println(menu_option_colour.green);
  //Serial.print("blue :");
  //Serial.println(menu_option_colour.blue);

#if defined(USING_COLOUR_SET_888)
  matrix.setTextColor(matrix.Color888(menu_option_colour.red, menu_option_colour.green, menu_option_colour.blue));
#elif defined(USING_COLOUR_SET_444)
  matrix.setTextColor(matrix.Color444(menu_option_colour.red, menu_option_colour.green, menu_option_colour.blue));
#else
  matrix.setTextColor(matrix.Color333(menu_option_colour.red, menu_option_colour.green, menu_option_colour.blue));
#endif
}


void Graphics::set_menu_colour_highlighted() {

  byte delta = COLOUR_LEVEL_STEP_SIZE; //modify colour by this

#if defined(USING_COLOUR_SET_888)
  matrix.setTextColor(matrix.Color888(menu_option_colour.red + delta, menu_option_colour.green + delta, menu_option_colour.blue + delta));
#elif defined(USING_COLOUR_SET_444)
  matrix.setTextColor(matrix.Color444(menu_option_colour.red + delta, menu_option_colour.green + delta, menu_option_colour.blue + delta));
#else
  matrix.setTextColor(matrix.Color333(menu_option_colour.red + delta, menu_option_colour.green + delta, menu_option_colour.blue + delta));
#endif
}

byte Graphics::non_linear_startup_function(uint16_t x) {//<- i have no idea how i came up with this

  //  float a = 0.000000000000010658 //<- tiny effect, not included to improve speed
  float b = -0.151;
  float c = 0.0127;
  float d = -0.000066;
  return (byte) round(abs(b * x + c * (x * x) + d * (x * x * x))); // d*x^3 + c*x^2 + b*x + c

}


void Graphics::draw_background() {

  //  byte pixels_right_of_node = ((NUM_SCREENS - 1) - screen_parameters.node_address) * SINGLE_MATRIX_WIDTH; //number of pixels to the right of this screen

  if (DEFAULT_MENU_WIDTH - menu_pixels_right_of_node() < SINGLE_MATRIX_WIDTH) { // draw partial background
    this -> clear_area(SINGLE_MATRIX_WIDTH - (DEFAULT_MENU_WIDTH - menu_pixels_right_of_node()), 0, SINGLE_MATRIX_WIDTH, SINGLE_MATRIX_HEIGHT); // clear area for menu
    //    text_parameters[0].x_max = SINGLE_MATRIX_WIDTH - (DEFAULT_MENU_WIDTH - menu_pixels_right_of_node());
    //    text_parameters[0].hard_limit = false;
    //    text_parameters[0].limit_enabled = true;
  }
}

void Graphics::clear_area(byte top_left_x, byte top_left_y, byte bottom_right_x, byte bottom_right_y) {

  if (top_left_x == 0 && top_left_y == 0 && bottom_right_x == SINGLE_MATRIX_WIDTH && bottom_right_y == SINGLE_MATRIX_HEIGHT)
    matrix.fillScreen(0); //optimised method for filling the screen
  else

#if defined(USING_COLOUR_SET_888)
    matrix.fillRect(top_left_x, top_left_y, bottom_right_x, bottom_right_y, matrix.Color888(0, 0, 0));
#elif defined(USING_COLOUR_SET_444)
    matrix.fillRect(top_left_x, top_left_y, bottom_right_x, bottom_right_y, matrix.Color444(0, 0, 0));
#else
    matrix.fillRect(top_left_x, top_left_y, bottom_right_x, bottom_right_y, matrix.Color333(0, 0, 0));
#endif

}


void Graphics::cpy_pgm_string(char *dest, byte src) {
  strcpy_P(dest, (char*)pgm_read_word(&(menu_string_table[src])));
}

void Graphics::print_pgm_title(byte src, byte len, byte center) {
  char item[len];
  cpy_pgm_string(item, src);
  matrix.setCursor((center - ((len * ASCII_CHARACTER_BASIC_WIDTH) >> 1) + (ASCII_CHARACTER_BASIC_WIDTH >> 1)), 0);

  for (byte i = 0; i < len; i++) {
    //    Serial.print(item[i]);
    //    Serial.print(" ");
    matrix.print(item[i]);
  }
}

void Graphics::fill_title_background() {

#if defined(USING_COLOUR_SET_888)
  uint16_t c = matrix.Color888(MENU_TITLE_BACKGROUND_R, MENU_TITLE_BACKGROUND_G, MENU_TITLE_BACKGROUND_B);
#elif defined(USING_COLOUR_SET_444)
  uint16_t c = matrix.Color444(MENU_TITLE_BACKGROUND_R, MENU_TITLE_BACKGROUND_G, MENU_TITLE_BACKGROUND_B);
#else
  uint16_t c = matrix.Color333(MENU_TITLE_BACKGROUND_R, MENU_TITLE_BACKGROUND_G, MENU_TITLE_BACKGROUND_B);
#endif

  matrix.fillRect(0, 0, SINGLE_MATRIX_WIDTH, ASCII_CHARACTER_BASIC_HEIGHT, c);
}

void Graphics::write_title(byte title) {

  static int center_of_menu = SINGLE_MATRIX_WIDTH - ((DEFAULT_MENU_WIDTH >> 1) - menu_pixels_right_of_node());

  fill_title_background();
  set_title_colour();

  switch (title) {
    case MAIN_MENU:                 print_pgm_title(MAIN_MENU_LOC,          menu_tree_item_lengths.main_menu,               center_of_menu);         break;
    case SCREEN_MODE_MENU:          print_pgm_title(SCREEN_MODE_LOC,        menu_tree_item_lengths.screen_mode,             center_of_menu);         break;
    case BRIGHTNESS_MENU:           print_pgm_title(SCREEN_BRIGHTNESS_LOC,  menu_tree_item_lengths.brightness,              center_of_menu);         break;
    case TEXT_SETTINGS_MENU:        print_pgm_title(TEXT_SETTINGS_LOC,      menu_tree_item_lengths.text_settings,           center_of_menu);         break;
    case FAN_SETTINGS_MENU:         print_pgm_title(FAN_SETTINGS_LOC,       menu_tree_item_lengths.fan_settings,            center_of_menu);         break;
    case INTERNET_CONFIG_MENU:      print_pgm_title(INTERNET_CONFIG_LOC,    menu_tree_item_lengths.internet_settings,       center_of_menu);         break;
    case SD_CARD_MENU:              print_pgm_title(SD_CARD_LOC,            menu_tree_item_lengths.sd_card_settings,        center_of_menu);         break;
    case LED_STRIP_MENU:            print_pgm_title(LED_STRIP_LOC,          menu_tree_item_lengths.led_strip_settings,      center_of_menu);         break;
    case TEXT_SIZE_MENU:            print_pgm_title(TEXT_SIZE_LOC,          menu_tree_item_lengths.text_size_settings,      center_of_menu);         break;
    case TEXT_COLOUR_MENU:          print_pgm_title(TEXT_COLOUR_LOC,        menu_tree_item_lengths.text_colour_settings,    center_of_menu);         break;
    case TEXT_COLOUR_RED:           print_pgm_title(RED_LOC,                menu_tree_item_lengths.text_colour_red,         center_of_menu);         break;
    case TEXT_COLOUR_GREEN:         print_pgm_title(GREEN_LOC,              menu_tree_item_lengths.text_colour_green,       center_of_menu);         break;
    case TEXT_COLOUR_BLUE:          print_pgm_title(BLUE_LOC,               menu_tree_item_lengths.text_colour_blue,        center_of_menu);         break;
    case TEXT_COLOUR_HUE:           print_pgm_title(HUE_LOC,                menu_tree_item_lengths.text_colour_hue,         center_of_menu);         break;
    case SCROLL_SPEED_MENU:         print_pgm_title(SCROLL_SPEED,           menu_tree_item_lengths.scroll_speed_settings,   center_of_menu);         break;
    case MIN_FAN_SPEED_MENU:        print_pgm_title(FAN_MIN_SPEED_LOC,      menu_tree_item_lengths.minimum_rotating_speed,  center_of_menu);         break;
    case SD_FOLDERS_MENU:           print_pgm_title(SD_FOLDERS_LOC,         menu_tree_item_lengths.sd_card_folders,         center_of_menu);         break;
    case LED_STRIP_BRIGHTNESS_MENU: print_pgm_title(STRIP_BRIGHTNESS_LOC,   menu_tree_item_lengths.led_strip_brightness,    center_of_menu);         break;


    default:                        print_pgm_title(DEFAULT_TITLE_LOC,      menu_tree_item_lengths.default_title,           center_of_menu);         break;


  }
}


inline void Graphics::draw_menu_pointer() {

#if defined(USING_COLOUR_SET_888)
  uint16_t c = matrix.Color888(MENU_POINTER_COLOUR_R, MENU_POINTER_COLOUR_G, MENU_POINTER_COLOUR_B);
#elif defined(USING_COLOUR_SET_444)
  uint16_t c = matrix.Color444(MENU_POINTER_COLOUR_R, MENU_POINTER_COLOUR_G, MENU_POINTER_COLOUR_B);
#else
  uint16_t c = matrix.Color333(MENU_POINTER_COLOUR_R, MENU_POINTER_COLOUR_G, MENU_POINTER_COLOUR_B);
#endif

  matrix.fillTriangle(MENU_POINTER_X0, MENU_POINTER_Y0, MENU_POINTER_X1, MENU_POINTER_Y1, MENU_POINTER_X2, MENU_POINTER_Y2, c);
}

void Graphics::set_menu_item_cursor(byte row) {

  int x_loc = SINGLE_MATRIX_WIDTH - (DEFAULT_MENU_WIDTH - menu_pixels_right_of_node());
  int y_loc = 0;
  if (row == 1) {
    y_loc = ASCII_CHARACTER_BASIC_HEIGHT;
  }
  else if (row == 2) {
    y_loc = 2 * ASCII_CHARACTER_BASIC_HEIGHT;
    x_loc += 6;
  }
  else if (row == 3) {
    y_loc = 3 * ASCII_CHARACTER_BASIC_HEIGHT;
  }
  matrix.setCursor(x_loc, y_loc);
}

void Graphics::set_menu_item_cursor_scrolling(byte row) { //offset positions to between standard positions

  int x_loc = SINGLE_MATRIX_WIDTH - (DEFAULT_MENU_WIDTH - menu_pixels_right_of_node());
  int y_loc = 0;

  byte y_offset = ASCII_CHARACTER_BASIC_HEIGHT / 2;

  if (row == 1) {
    y_loc = y_offset;
  }

  else if (row == 2) {

    y_loc = ASCII_CHARACTER_BASIC_HEIGHT + y_offset;
    x_loc += 4;
  }
  else if (row == 3) {
    y_loc = (2 * ASCII_CHARACTER_BASIC_HEIGHT) + y_offset;
    x_loc += 4;
  }

  else if (row == 4) {
    y_loc = (3 * ASCII_CHARACTER_BASIC_HEIGHT) + y_offset;
  }

  matrix.setCursor(x_loc, y_loc);
}


void Graphics::print_pgm_menu_item(byte src, byte len, byte row) {
  char item[len];
  set_menu_item_cursor(row);
  cpy_pgm_string(item, src);

  for (byte i = 0; i < len; i++) {
    matrix.print(item[i]);
  }
}


void Graphics::print_pgm_menu_item_scrolling(byte src, byte len, byte row) {
  char item[len];
  set_menu_item_cursor_scrolling(row);  //intermediary cursor positions for animation
  cpy_pgm_string(item, src);

  for (byte i = 0; i < len; i++) {
    matrix.print(item[i]);
  }
}


void Graphics::print_highlight_pgm_menu_item(byte src, byte len, byte row) {

  if (src == SCREEN_MODE_0 && screen_parameters.cur_mode == 0)     set_menu_colour_highlighted();
  else if (src == SCREEN_MODE_1 && screen_parameters.cur_mode == 1)     set_menu_colour_highlighted();
  else if (src == SCREEN_MODE_2 && screen_parameters.cur_mode == 2)     set_menu_colour_highlighted();
  else if (src == SCREEN_MODE_3 && screen_parameters.cur_mode == 3)     set_menu_colour_highlighted();


  print_pgm_menu_item(src, len, row);

}



void Graphics::write_menu_option(byte first, byte second, byte third) {

  byte line_item = 255;

  draw_menu_pointer();
  set_menu_colour();

  for (byte i = 1; i <= 3; i++) { //loop through three lines

    if (i == 1)       line_item = first;  //select which argument were printing this loop
    else if (i == 2)  line_item = second;
    else              line_item = third;

    //    matrix.setTextColor(matrix.Color333(0,7,7));

    //match menu item passed to index in array, and expected stirng length
    switch (line_item) {
      case RETURN_MENU:                 print_pgm_menu_item(RETURN_LOC,             menu_tree_item_lengths.RETURN,                i);    break;
      case SCREEN_MODE_MENU:            print_pgm_menu_item(SCREEN_MODE_LOC,        menu_tree_item_lengths.screen_mode,           i);    break;
      case BRIGHTNESS_MENU:             print_pgm_menu_item(SCREEN_BRIGHTNESS_LOC,  menu_tree_item_lengths.brightness,            i);    break;
      case TEXT_SETTINGS_MENU:          print_pgm_menu_item(TEXT_SETTINGS_LOC,      menu_tree_item_lengths.text_settings,         i);    break;
      case FAN_SETTINGS_MENU:           print_pgm_menu_item(FAN_SETTINGS_LOC,       menu_tree_item_lengths.fan_settings,          i);    break;
      case INTERNET_CONFIG_MENU:        print_pgm_menu_item(INTERNET_CONFIG_LOC,    menu_tree_item_lengths.internet_settings,     i);    break;
      case SD_CARD_MENU:                print_pgm_menu_item(SD_CARD_LOC,            menu_tree_item_lengths.sd_card_settings,      i);    break;
      case LED_STRIP_MENU:              print_pgm_menu_item(LED_STRIP_LOC,          menu_tree_item_lengths.led_strip_settings,    i);    break;

      case SCREEN_MODE_0:               print_highlight_pgm_menu_item(BOTH_ON_LOC,  menu_tree_item_lengths.screen_mode0,          i);    break;
      case SCREEN_MODE_1:               print_highlight_pgm_menu_item(FRONT_ON_LOC, menu_tree_item_lengths.screen_mode1,          i);    break;
      case SCREEN_MODE_2:               print_highlight_pgm_menu_item(BACK_ON_LOC,  menu_tree_item_lengths.screen_mode2,          i);    break;
      case SCREEN_MODE_3:               print_highlight_pgm_menu_item(BOTH_OFF_LOC, menu_tree_item_lengths.screen_mode3,          i);    break;


      case TEXT_SIZE_MENU:              print_pgm_menu_item(TEXT_SIZE_LOC,          menu_tree_item_lengths.text_size_settings,    i);    break;
      case TEXT_COLOUR_MENU:            print_pgm_menu_item(TEXT_COLOUR_LOC,        menu_tree_item_lengths.text_colour_settings,  i);    break;
      case SCROLL_SPEED_MENU:           print_pgm_menu_item(SCROLL_SPEED,           menu_tree_item_lengths.scroll_speed_settings, i);    break;
      case FAN_SPEED_MENU:              print_pgm_menu_item(FAN_SPEED_LOC,          menu_tree_item_lengths.fan_speed_settings,    i);    break;
      case ENABLE_FAN:                  print_pgm_menu_item(ENABLE_LOC,             menu_tree_item_lengths.fan_enable,            i);    break;
      case DISABLE_FAN:                 print_pgm_menu_item(DISABLE_LOC,            menu_tree_item_lengths.fan_disable,           i);    break;
      case MIN_FAN_SPEED_MENU:          print_pgm_menu_item(FAN_MIN_SPEED_LOC,      menu_tree_item_lengths.minimum_rotating_speed, i);   break;

      //case SELECT_NETWORK_MANUALLY:     print_pgm_menu_item(LED_STRIP_LOC,          menu_tree_item_lengths.led_strip_settings,    i);    break;

      case SD_FOLDERS_MENU:             print_pgm_menu_item(SD_FOLDERS_LOC,          menu_tree_item_lengths.sd_card_folders,      i);    break;
      case LED_STRIP_BRIGHTNESS_MENU:   print_pgm_menu_item(STRIP_BRIGHTNESS_LOC,    menu_tree_item_lengths.led_strip_brightness, i);    break;
      case ENABLE_LED_STRIP:            print_pgm_menu_item(ENABLE_LOC,              menu_tree_item_lengths.led_strip_enable,     i);    break;
      case DISABLE_LED_STRIP:           print_pgm_menu_item(DISABLE_LOC,             menu_tree_item_lengths.led_strip_disable,    i);    break;

      case TEXT_COLOUR_RED:             print_pgm_menu_item(RED_LOC,                 menu_tree_item_lengths.text_colour_red,      i);    break;
      case TEXT_COLOUR_GREEN:           print_pgm_menu_item(GREEN_LOC,               menu_tree_item_lengths.text_colour_green,    i);    break;
      case TEXT_COLOUR_BLUE:            print_pgm_menu_item(BLUE_LOC,                menu_tree_item_lengths.text_colour_blue,     i);    break;
      case TEXT_COLOUR_HUE:             print_pgm_menu_item(HUE_LOC,                 menu_tree_item_lengths.text_colour_hue,      i);    break;


      case NULL_STRING:                 print_pgm_menu_item(EMPTY_STRING_LOC,        menu_tree_item_lengths.null_string,          i);    break;
      default:                          print_pgm_menu_item(DEFAULT_STRING_LOC,      menu_tree_item_lengths.default_string,       i);    break;


    }
  }
}


void Graphics::write_adjustment_menu(byte item, byte obj_num) {

  static int center_of_menu = SINGLE_MATRIX_WIDTH - (DEFAULT_MENU_WIDTH >> 1 - menu_pixels_right_of_node());
  char buf[4] = {' '}; //to store converted byte
  byte val;
  switch (item) {
    case BRIGHTNESS_MENU:           print_pgm_adjustment_menu(screen_parameters.brightness);              break;
    case TEXT_SIZE_MENU:            print_pgm_adjustment_menu(text_parameters[obj_num].text_size);        break;
    case SCROLL_SPEED_MENU:         print_pgm_adjustment_menu(cursor_parameters[obj_num].x_dir);          break;
    case FAN_SPEED_MENU:            print_pgm_adjustment_menu(menu_parameters.fan_speed);                 break;
    case MIN_FAN_SPEED_MENU:        print_pgm_adjustment_menu(menu_parameters.min_fan_speed);             break;
    case LED_STRIP_BRIGHTNESS_MENU: print_pgm_adjustment_menu(menu_parameters.led_strip_brightness);      break;
    case TEXT_COLOUR_RED:           print_pgm_adjustment_menu(text_parameters[obj_num].colour_r);         break;
    case TEXT_COLOUR_GREEN:         print_pgm_adjustment_menu(text_parameters[obj_num].colour_g);         break;
    case TEXT_COLOUR_BLUE:          print_pgm_adjustment_menu(text_parameters[obj_num].colour_b);         break;
    case TEXT_COLOUR_HUE:           print_pgm_adjustment_menu(text_parameters[obj_num].hue);              break;
     
  }
}


void Graphics::print_pgm_adjustment_menu(int value) {

  static int center_of_menu = SINGLE_MATRIX_WIDTH - ((DEFAULT_MENU_WIDTH >> 1) - menu_pixels_right_of_node());
  byte num_chars = 0;
  int temp = value;
  while (temp != 0) {
    temp /= 10;
    num_chars++;
  }
  if (value < 0) num_chars++;//minus symbol
  num_chars += NUM_EXTRA_ADJ_MENU_CHARS;  //counter additional chars (+,- etc)
  matrix.setCursor(center_of_menu - ((num_chars * ASCII_CHARACTER_BASIC_WIDTH) / 2), 12);
  //  char item[1];
  //  cpy_pgm_string(item, minus_string);
  matrix.print((char)pgm_read_byte_near(minus_string));
  matrix.print("(");
  //  Serial.print((char)pgm_read_byte_near(minus_string));
  matrix.print(menu_parameters.min_fan_speed);
    matrix.print(")");
  matrix.print((char)pgm_read_byte_near(plus_string));
  //  cpy_pgm_string(item, plus_string);
  //  matrix.print(item);

  //  matrix.print(minus_string + (String)buf + plus_string);
}

void Graphics::write_enable_menu_item(byte item) {}



byte Graphics::menu_pixels_right_of_node() {
  return ((NUM_SCREENS - 1) - screen_parameters.node_address) * SINGLE_MATRIX_WIDTH; //number of pixels to the right of this screen

}

void Graphics::update_pos_isr_period(byte obj_num, uint32_t new_x_val, uint32_t new_y_val) {

  //save value to array
  screen_parameters.update_periods[(2 * obj_num)] = new_x_val;
  screen_parameters.update_periods[(2 * obj_num) + 1] = new_y_val;

  //find smallest value
  screen_parameters.cur_update_period = screen_parameters.update_periods[0];

  for (byte i = 1 ; i < (2 * MAX_NUM_OF_TEXT_OBJECTS) ; i++ )
  {
    if ( screen_parameters.update_periods[i] < screen_parameters.cur_update_period && screen_parameters.update_periods[i] != 0 )
    {
      screen_parameters.cur_update_period = screen_parameters.update_periods[i];
    }
  }

  set_pos_isr_period(screen_parameters.cur_update_period);

}



void Graphics::write_menu_scrolling(byte first, byte second, byte third, byte fourth, byte fifth, byte dir) {

  byte line_item = 255;
  byte start_loc = 0;
  byte end_loc = 5;
  byte row = start_loc;
  if (dir == SCROLL_UP)start_loc++;
  else end_loc--;


  draw_menu_pointer();
  set_menu_colour();

  for (byte i = start_loc; i <= end_loc; i++) { //loop through four lines lines, start and end defined by dir

    if (i == 1)       line_item = first;  //select which argument were printing this loop
    else if (i == 2)  line_item = second;
    else if (i == 3)  line_item = third;
    else if (i == 4)  line_item = fourth;
    else              line_item = fifth;

    //Serial.println(line_item);

    if (dir == SCROLL_UP)row = i - 1; //will go to 5 in this case, apply correction for set_menu_item_cursor_scolling function
    else row = i;

    //match menu item passed to index in array, and expected stirng length
    switch (line_item) {
      case RETURN_MENU:                 print_pgm_menu_item_scrolling(RETURN_LOC,             menu_tree_item_lengths.RETURN,                row);    break;
      case SCREEN_MODE_MENU:            print_pgm_menu_item_scrolling(SCREEN_MODE_LOC,        menu_tree_item_lengths.screen_mode,           row);    break;
      case BRIGHTNESS_MENU:             print_pgm_menu_item_scrolling(SCREEN_BRIGHTNESS_LOC,  menu_tree_item_lengths.brightness,            row);    break;
      case TEXT_SETTINGS_MENU:          print_pgm_menu_item_scrolling(TEXT_SETTINGS_LOC,      menu_tree_item_lengths.text_settings,         row);    break;
      case FAN_SETTINGS_MENU:           print_pgm_menu_item_scrolling(FAN_SETTINGS_LOC,       menu_tree_item_lengths.fan_settings,          row);    break;
      case INTERNET_CONFIG_MENU:        print_pgm_menu_item_scrolling(INTERNET_CONFIG_LOC,    menu_tree_item_lengths.internet_settings,     row);    break;
      case SD_CARD_MENU:                print_pgm_menu_item_scrolling(SD_CARD_LOC,            menu_tree_item_lengths.sd_card_settings,      row);    break;
      case LED_STRIP_MENU:              print_pgm_menu_item_scrolling(LED_STRIP_LOC,          menu_tree_item_lengths.led_strip_settings,    row);    break;

      case SCREEN_MODE_0:               print_highlight_pgm_menu_item_scrolling(BOTH_ON_LOC,  menu_tree_item_lengths.screen_mode0,          row);    break;
      case SCREEN_MODE_1:               print_highlight_pgm_menu_item_scrolling(FRONT_ON_LOC, menu_tree_item_lengths.screen_mode1,          row);    break;
      case SCREEN_MODE_2:               print_highlight_pgm_menu_item_scrolling(BACK_ON_LOC,  menu_tree_item_lengths.screen_mode2,          row);    break;
      case SCREEN_MODE_3:               print_highlight_pgm_menu_item_scrolling(BOTH_OFF_LOC, menu_tree_item_lengths.screen_mode3,          row);    break;


      case TEXT_SIZE_MENU:              print_pgm_menu_item_scrolling(TEXT_SIZE_LOC,          menu_tree_item_lengths.text_size_settings,    row);    break;
      case TEXT_COLOUR_MENU:            print_pgm_menu_item_scrolling(TEXT_COLOUR_LOC,        menu_tree_item_lengths.text_colour_settings,  row);    break;
      case SCROLL_SPEED_MENU:           print_pgm_menu_item_scrolling(SCROLL_SPEED,           menu_tree_item_lengths.scroll_speed_settings, row);    break;
      case FAN_SPEED_MENU:              print_pgm_menu_item_scrolling(FAN_SPEED_LOC,          menu_tree_item_lengths.fan_speed_settings,    row);    break;
      case ENABLE_FAN:                  print_pgm_menu_item_scrolling(ENABLE_LOC,             menu_tree_item_lengths.fan_enable,            row);    break;
      case DISABLE_FAN:                 print_pgm_menu_item_scrolling(DISABLE_LOC,            menu_tree_item_lengths.fan_disable,           row);    break;
      case MIN_FAN_SPEED_MENU:          print_pgm_menu_item_scrolling(FAN_MIN_SPEED_LOC,      menu_tree_item_lengths.minimum_rotating_speed, row);   break;

      //case SELECT_NETWORK_MANUALLY:     print_pgm_menu_item_scrolling(LED_STRIP_LOC,          menu_tree_item_lengths.led_strip_settings,    row);    break;

      case SD_FOLDERS_MENU:             print_pgm_menu_item_scrolling(SD_FOLDERS_LOC,          menu_tree_item_lengths.sd_card_folders,      row);    break;
      case LED_STRIP_BRIGHTNESS_MENU:   print_pgm_menu_item_scrolling(STRIP_BRIGHTNESS_LOC,    menu_tree_item_lengths.led_strip_brightness, row);    break;
      case TEXT_COLOUR_RED:             print_pgm_menu_item_scrolling(RED_LOC,                 menu_tree_item_lengths.text_colour_red,      row);    break;
      case TEXT_COLOUR_GREEN:           print_pgm_menu_item_scrolling(GREEN_LOC,               menu_tree_item_lengths.text_colour_green,    row);    break;
      case TEXT_COLOUR_BLUE:            print_pgm_menu_item_scrolling(BLUE_LOC,                menu_tree_item_lengths.text_colour_blue,     row);    break;
      case TEXT_COLOUR_HUE:             print_pgm_menu_item_scrolling(HUE_LOC,                 menu_tree_item_lengths.text_colour_hue,      row);    break;

      case NULL_STRING:                 print_pgm_menu_item_scrolling(EMPTY_STRING_LOC,        menu_tree_item_lengths.null_string,          row);    break;
      default:                          print_pgm_menu_item_scrolling(DEFAULT_STRING_LOC,      menu_tree_item_lengths.default_string,       row);    break;

    }
  }
}
#endif //GRAPHICS_CPP
