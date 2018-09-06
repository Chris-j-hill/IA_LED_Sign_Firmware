

#ifndef GRAPHICS_CPP
#define GRAPHICS_CPP

#include "Graphics.h"
#include "Mega_Pins.h"
#include "src/Timer3/TimerThree.h"
#include "src/Timer4/TimerFour.h"
#include "Menus.h"
#include "Local_Config.h"


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
 * define locations of data in array below, for
 * abstraction away from non descript indexing in 
 * funcions later
 */

#define MAIN_MENU_LOC           0
#define RETURN_LOC              1
#define SCREEN_MODE_LOC         2
#define TEXT_SETTINGS_LOC       3
#define FAN_SETTINGS_LOC        4
#define INTERNET_CONFIG_LOC     5
#define SD_CARD_LOC             6

#define LED_STRIP_LOC           7
#define RETURN_LOC              8
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

#define SD_FOLDERS_LOC          19

#define STRIP_BRIGHTNESS_LOC    20

#define RED_LOC                 21
#define GREED_LOC               22
#define BLUE_LOC                23
#define HUE_LOC                 24
#define USE_HUE_LOC             25
#define USE_RGB_LOC             26

#define SCROLL_X_LOC            27
#define SCROLL_Y_LOC            28

#define ENABLE_LOC              29
#define DISABLE_LOC             30
#define EMPTY_STRING_LOC        31
#define DEFAULT_STRING_LOC      32
#define DEFAULT_TITLE_LOC       33



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
  main_menu,
  RETURN,
  screen_mode,
  brightness,
  text_settings,
  fan_settings,
  internet_settings,
  sd_card_settings,
  led_strip_settings,

  screen_mode0,
  screen_mode1,
  screen_mode3,
  screen_mode2,

  text_size_settings,
  text_colour_settings,
  scroll_speed_settings,
  flip_dir_settings,

  fan_speed_settings,
  minimum_rotating_speed,

  internet_config_menu,
  select_network_manually,
  ethernet_enable,
  ethernet_disable,
  wifi_enable,
  wifi_disable,

  sd_card_folders,

  led_strip_brightness,

  text_colour_red,
  text_colour_green,
  text_colour_blue,
  text_colour_hue,
  text_colour_use_hue,
  text_colour_use_rgb,

  scroll_speed_x,
  scroll_speed_y,

  enable,
  disable,

  null_string,
  default_string,
  default_title
};




void pos_update_ISR() {
  interpolate_pos_flag = true;
}


void Graphics::init_matrix() {
  matrix.begin();
  matrix.setTextWrap(false); // Allow text to run off right edge

  set_title_colour();
  set_menu_colour();
}


void Graphics::init_update_display_isr() {

  uint32_t period = serial_check_ISR_period();

  Timer4.initialize(period);
  Timer4.attachInterrupt(serial_check_ISR);
  Timer4.start();
}


void serial_check_ISR() {
  //
  //  if (Serial_1.available() != 0){  //check for serial arrived very often
  //
  //    Timer3.stop();  //turn off all active interrupts
  //    Timer4.stop();
  //
  //    //enable interrupts to allow serial and delays to work
  //    coms_serial.read_buffer();
  //
  //    Timer3.start();
  //    Tiemr4.start();
  //
  //  }
}

void Graphics::update_display() {

  static uint32_t time_since_last_update = millis();

  if (screen_parameters.time_last_updated == 0)
    screen_parameters.time_last_updated = millis(); //if it was never set yet, set it here

  if ((millis() > time_since_last_update + MIN_DISPLAY_UPDATE_PERIOD) && (!screen_parameters.updated) && (millis() > screen_parameters.time_last_updated + SCREEN_UPDATE_BACKOFF_PERIOD)) {

    Timer3.stop();  //disable this timer to speed things up
    set_display_mode();
    //    Serial.println();
    //    Serial.println("update display");
    //    Serial.println(menu.get_current_menu());
    //    Serial.println(screen_parameters.updated);
    //    Serial.println();

    if (menu.get_current_menu() == DEFAULT_MENU) menu.set_current_menu(MAIN_MENU);


    if (menu.get_current_menu() == DEFAULT_MENU) {

      for (byte i = 0; i < MAX_NUM_OF_TEXT_OBJECTS; i++) {
        if (text_parameters[i].object_used) {
          if (text_parameters[i].use_hue)
            graphics.set_text_colour(text_parameters[i].hue);
          else
            graphics.set_text_colour(text_parameters[i].colour_r, text_parameters[i].colour_g, text_parameters[i].colour_b);
          //delay(100);
          graphics.draw_text(i);
          Serial.println("draw");
        }
      }
    }

    //    else if((menu.get_current_menu() != DEFAULT_MENU && menu.get_current_menu() != STARTUP_MENU) && !screen_parameters.updated) //<- partial coverage of menu...

    else if (menu.get_current_menu() != DEFAULT_MENU) // draw menu
      menu.display_menu();  //update any menu if applicable

    matrix.swapBuffers(false);  //push the buffer we just wrote to front

    time_since_last_update = millis();
    //dealt with screen update
    screen_parameters.updated = true;

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

  byte multiplier = map(screen_parameters.brightness, 0, 100, 0, 255);

  matrix.setTextColor(matrix.ColorHSV(new_hue, 255, multiplier, true));
}

//inline void Graphics::draw_text(byte obj_num) {
//
//  matrix.setTextSize(text_parameters[obj_num].text_size);
//  //  matrix.setCursor(cursor_parameters[obj_num].local_x_pos, cursor_parameters[obj_num].local_y_pos);
//  matrix.setCursor(1, 1);
//
//  uint32_t temp = millis();
//  temp = millis()-temp;
//  uint32_t start_time = millis();
//
//  TIMSK1 &= ~(1 << TOIE1);  //disable timer 1 interrupt (timer for screen)
//  for (byte i = 0; i < MAX_TWEET_SIZE; i++) {
//    if (text_parameters[obj_num].string[i] == 0) break;
//    matrix.print((char)text_parameters[obj_num].string[i]);
//  }
//  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
//
//  Serial.print(F("time taken = "));
//  Serial.println(millis() - start_time-temp);
//}



inline void Graphics::draw_text(byte obj_num) {

  static int colour = -1500;

  uint32_t temp = millis();
  temp = millis() - temp;
  uint32_t start_time = millis();

  int cursorx = 1;
  int cursory = 1;
  //  byte text_size = text_parameters[obj_num].text_size;
  byte text_size = 1;
  matrix.setTextSize(text_size);
  //  matrix.setCursor(cursor_parameters[obj_num].local_x_pos, cursor_parameters[obj_num].local_y_pos);

  byte num_chars = SINGLE_MATRIX_WIDTH / (text_size * ASCII_CHARACTER_BASIC_WIDTH) + 2;   // +2 for case of char not aligned with egdes and null char at end
  char sub_char_array[num_chars] = {'\0'};

  byte start_loc = 0;

  if (cursorx < (0 - ASCII_CHARACTER_BASIC_WIDTH)) {
    start_loc = abs(cursorx / ASCII_CHARACTER_BASIC_WIDTH);
    matrix.setCursor(cursorx / ASCII_CHARACTER_BASIC_WIDTH, cursory); //move cursor closer to x=0 and ignore earlier characters
  }
  else if (cursorx >= SINGLE_MATRIX_WIDTH) //dont print if off screen
    return;
  else
    matrix.setCursor(cursorx, cursory);

  if (start_loc < MAX_TWEET_SIZE) { // only if could possibly be on screen
    for (byte i = 0; i < num_chars; i++) {
      if ((i + start_loc) == MAX_TWEET_SIZE) break;
      sub_char_array[i] = (char)text_parameters[obj_num].string[i + start_loc];
    }
  }
  String str((char*)sub_char_array);
  Serial.print(F("sub string = "));
  Serial.println(str);


  //  for (byte i = 0; i < MAX_TWEET_SIZE; i++) {
  //if (text_parameters[obj_num].string[i] == 0) break;
  TIMSK1 &= ~(1 << TOIE1);  //disable timer 1 interrupt (timer for screen)
  matrix.fillScreen(0);
  //  matrix.print(str[1]);
  //  matrix.fillScreen(matrix.ColorHSV(colour, 255, 255, true));
  //matrix.drawPixel(1,3,matrix.ColorHSV(colour, 255, 255, true));
  colour += 50;
  if (colour > 1500) colour = -1500;
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt

  //  }

  Serial.print(F("time taken = "));
  Serial.println(millis() - start_time - temp);
}



void Graphics::attach_pos_ISR() {

  uint16_t period = pos_isr_period();
  //  pos_isr_counter_overflow();

  Timer3.attachInterrupt(pos_update_ISR, period);
  Timer3.start();
}

void Graphics::interpolate_pos() {

  /* this function executes the required interpolation of the cursor
     based on the flag set inside the pos_update_isr funtion. this
     is reasonably slow with many divides so run outside of interrupts
     whenever processor isnt doing anything else
  */
  if (interpolate_pos_flag) {
    interpolate_pos_flag = false;
    uint32_t cur_time = millis(); //only need to do this once, millis isnt that fast...

    for (byte i = 0; i < MAX_NUM_OF_TEXT_OBJECTS; i++) {

      //check if period elapsed it is time to update
      if ((cur_time - cursor_parameters[i].isr_last_update_x_time) > cursor_parameters[i].time_between_increments_x) {
        cursor_parameters[i].isr_last_update_x_time = cur_time;

        //increment value, speed now irrelevant, since this interrupt should run the required num times per second, so just increment by one
        if (cursor_parameters[i].x_dir < 0)
          cursor_parameters[i].local_x_pos--;
        else if (cursor_parameters[i].x_dir > 0)
          cursor_parameters[i].local_x_pos++;
      }

      if ((cur_time - cursor_parameters[i].isr_last_update_y_time) > cursor_parameters[i].time_between_increments_y) {
        cursor_parameters[i].isr_last_update_y_time = cur_time;

        if (cursor_parameters[i].y_dir < 0)
          cursor_parameters[i].local_y_pos--;
        else if (cursor_parameters[i].y_dir > 0)
          cursor_parameters[i].local_y_pos++;
      }
    }
  }
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

inline uint16_t Graphics::pos_isr_period() {

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

void Graphics::write_title(byte title) {
  int center_of_menu = SINGLE_MATRIX_WIDTH - ((DEFAULT_MENU_WIDTH >> 1) - menu_pixels_right_of_node());
  set_title_colour();
  switch (title) {
    case MAIN_MENU:
      {
        matrix.setCursor((center_of_menu - ((menu_tree_item_lengths.main_menu * ASCII_CHARACTER_BASIC_WIDTH) >> 1) + (ASCII_CHARACTER_BASIC_WIDTH >> 1)), 1);
        char item[menu_tree_item_lengths.main_menu];
        strcpy_P(item, (char*)pgm_read_word(&(menu_string_table[0])));
        Serial.print(F("cursor x : "));
        Serial.println(center_of_menu - ((menu_tree_item_lengths.main_menu * ASCII_CHARACTER_BASIC_WIDTH) >> 1));

        for (byte i = 0; i < menu_tree_item_lengths.main_menu; i++) {
          Serial.print(item[i]);
          Serial.print(" ");
          matrix.print(item[i]);
          //          Serial.print((char)pgm_read_byte_near(menu_items.main_menu + i));
          //          Serial.print(" ");
          //          matrix.print((char)pgm_read_byte_near(menu_items.main_menu + i));
        }
        Serial.println();
        //        matrix.println(F2(menu_items.main_menu));
        break;
      }

    case SCREEN_MODE_MENU:
      {
        matrix.setCursor(1, center_of_menu - (sizeof(menu_items.screen_mode_menu)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.println(F2(menu_items.screen_mode_menu));
        break;
      }

    case BRIGHTNESS_MENU:
      {
        matrix.setCursor(1, center_of_menu - (sizeof(menu_items.brightness_menu)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.println(F2(menu_items.brightness_menu));
        break;
      }

    case TEXT_SETTINGS_MENU:
      {
        matrix.setCursor(1, center_of_menu - (sizeof(menu_items.text_settings_menu)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.println(F2(menu_items.text_settings_menu));
        break;
      }

    case FAN_SETTINGS_MENU:
      {
        matrix.setCursor(1, center_of_menu - (sizeof(menu_items.fan_settings_menu)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.println(F2(menu_items.fan_settings_menu));
        break;
      }

    case INTERNET_CONFIG_MENU:
      {
        matrix.setCursor(1, center_of_menu - (sizeof(menu_items.internet_config_menu)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.println(F2(menu_items.internet_config_menu));
        break;
      }

    case SD_CARD_MENU:
      {
        matrix.setCursor(1, center_of_menu - (sizeof(menu_items.sd_cards_menu)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.println(F2(menu_items.sd_cards_menu));
        break;
      }

    case LED_STRIP_MENU:
      {
        matrix.setCursor(1, center_of_menu - (sizeof(menu_items.led_strip_menu)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.println(F2(menu_items.led_strip_menu));
        break;
      }

    case TEXT_SIZE_MENU:
      {
        matrix.setCursor(1, center_of_menu - (sizeof(menu_items.text_size_menu)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.println(F2(menu_items.text_size_menu));
        break;
      }

    case TEXT_COLOUR_MENU:
      {
        matrix.setCursor(1, center_of_menu - (sizeof(menu_items.text_colour_menu)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.println(F2(menu_items.text_colour_menu));
        break;
      }

    case TEXT_COLOUR_RED:
      {
        matrix.setCursor(1, center_of_menu - (sizeof(menu_items.text_colour_red)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.println(F2(menu_items.text_colour_red));
        break;
      }

    case TEXT_COLOUR_GREEN:
      {
        matrix.setCursor(1, center_of_menu - (sizeof(menu_items.text_colour_green)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.println(F2(menu_items.text_colour_green));
        break;
      }

    case TEXT_COLOUR_BLUE:
      {
        matrix.setCursor(1, center_of_menu - (sizeof(menu_items.text_colour_blue)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.println(F2(menu_items.text_colour_blue));
        break;
      }

    case TEXT_COLOUR_HUE:
      {
        matrix.setCursor(1, center_of_menu - (sizeof(menu_items.text_colour_hue)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.println(F2(menu_items.text_colour_hue));
        break;
      }

    case SCROLL_SPEED_MENU:
      {
        matrix.setCursor(1, center_of_menu - (sizeof(menu_items.fan_speed_settings)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.println(F2(menu_items.fan_speed_settings));
        break;
      }

    case MIN_FAN_SPEED_MENU:
      {
        matrix.setCursor(1, center_of_menu - (sizeof(menu_items.minimum_fan_speed_menu)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.println(F2(menu_items.minimum_fan_speed_menu));
        break;
      }

    case SD_FOLDERS_MENU:
      {
        matrix.setCursor(1, center_of_menu - (sizeof(menu_items.sd_card_folders)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.println(F2(menu_items.sd_card_folders));
        break;
      }

    case LED_STRIP_BRIGHTNESS_MENU:
      {
        matrix.setCursor(1, center_of_menu - (sizeof(menu_items.led_strip_brightness)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.println(F2(menu_items.led_strip_brightness));
        break;
      }
    default:
      {
        matrix.setCursor(1, center_of_menu - (sizeof(menu_items.default_title)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.println(F2(menu_items.default_title));
        break;
      }
  }
}

void Graphics::write_menu_option(byte first, byte second, byte third, byte line_config) {  //NB: line_config = 1 top line blank -> = 2 all filled -> = 3 bottom blank
  byte line_item = 255;
  for (byte i = 1; i < 4; i++) { //loop through three lines
    if (i == 1) {
      line_item = first;
      matrix.setCursor(SINGLE_MATRIX_WIDTH - (DEFAULT_MENU_WIDTH - menu_pixels_right_of_node()), ASCII_CHARACTER_BASIC_HEIGHT + 1);
    }
    else if (i == 2) {
      line_item = second;
      matrix.fillTriangle(MENU_POINTER_X0, MENU_POINTER_Y0, MENU_POINTER_X1, MENU_POINTER_Y1, MENU_POINTER_X2, MENU_POINTER_Y2, matrix.Color333(MENU_POINTER_COLOUR_R, MENU_POINTER_COLOUR_G, MENU_POINTER_COLOUR_B));
      matrix.setCursor(SINGLE_MATRIX_WIDTH - (DEFAULT_MENU_WIDTH - menu_pixels_right_of_node()) + 6, (ASCII_CHARACTER_BASIC_HEIGHT << 1) + 1);
    }
    else if (i == 3) {
      line_item = third;
      matrix.setCursor(SINGLE_MATRIX_WIDTH - (DEFAULT_MENU_WIDTH - menu_pixels_right_of_node()), 3 * ASCII_CHARACTER_BASIC_HEIGHT + 1);
    }

    set_menu_colour();

    switch (line_item) {
      case RETURN_MENU:                 matrix.println(F2(menu_items.RETURN));                      break;
      case SCREEN_MODE_MENU:            matrix.println(F2(menu_items.screen_mode));                 break;
      case BRIGHTNESS_MENU:             matrix.println(F2(menu_items.brightness));                  break;
      case TEXT_SETTINGS_MENU:          matrix.println(F2(menu_items.text_settings));               break;
      case FAN_SETTINGS_MENU:           matrix.println(F2(menu_items.fan_settings));                break;
      case INTERNET_CONFIG_MENU:        matrix.println(F2(menu_items.internet_settings));           break;
      case SD_CARD_MENU:                matrix.println(F2(menu_items.sd_card_settings));            break;
      case LED_STRIP_MENU:              matrix.println(F2(menu_items.led_strip_settings));          break;

      case SCREEN_MODE_0:
        if (screen_parameters.cur_mode == 0) //if the item being displayed is the current mode, change its colour a bit to identify it
          set_menu_colour_highlighted();
        matrix.println(F2(menu_items.screen_mode0));
        break;

      case SCREEN_MODE_1:
        if (screen_parameters.cur_mode == 1) //if the item being displayed is the current mode, change its colour a bit to identify it
          set_menu_colour_highlighted();
        matrix.println(F2(menu_items.screen_mode1));
        break;

      case SCREEN_MODE_2:
        if (screen_parameters.cur_mode == 2) //if the item being displayed is the current mode, change its colour a bit to identify it
          set_menu_colour_highlighted();
        matrix.println(F2(menu_items.screen_mode2));
        break;

      case SCREEN_MODE_3:
        if (screen_parameters.cur_mode == 3) //if the item being displayed is the current mode, change its colour a bit to identify it
          set_menu_colour_highlighted();
        matrix.println(F2(menu_items.screen_mode3));
        break;

      case TEXT_SIZE_MENU:              matrix.println(F2(menu_items.text_size_settings));          break;
      case TEXT_COLOUR_MENU:            matrix.println(F2(menu_items.text_colour_settings));        break;
      case SCROLL_SPEED_MENU:           matrix.println(F2(menu_items.scroll_speed_settings));       break;

      case FAN_SPEED_MENU:              matrix.println(F2(menu_items.fan_speed_settings));          break;
      case ENABLE_FAN:                  matrix.println(F2(menu_items.fan_enable));                  break;
      case DISABLE_FAN:                 matrix.println(F2(menu_items.fan_disable));                 break;
      case MIN_FAN_SPEED_MENU:          matrix.println(F2(menu_items.minimum_rotating_speed));      break;

      case SELECT_NETWORK_MANUALLY:     matrix.println(F2(menu_items.select_network_manually));     break;

      case SD_FOLDERS_MENU:             matrix.println(F2(menu_items.sd_card_folders));             break;

      case LED_STRIP_BRIGHTNESS_MENU:   matrix.println(F2(menu_items.led_strip_brightness));        break;

      case TEXT_COLOUR_RED:             matrix.println(F2(menu_items.text_colour_red));             break;
      case TEXT_COLOUR_GREEN:           matrix.println(F2(menu_items.text_colour_green));           break;
      case TEXT_COLOUR_BLUE:            matrix.println(F2(menu_items.text_colour_blue));            break;
      case TEXT_COLOUR_HUE:             matrix.println(F2(menu_items.text_colour_hue));             break;






      case NULL_STRING:                 matrix.println(F2(menu_items.null_string));                 break;
      default:                          matrix.println(F2(menu_items.default_string));              break;

    }
  }
}

void Graphics::write_adjustment_menu(byte item, byte obj_num) {

  int center_of_menu = SINGLE_MATRIX_WIDTH - (DEFAULT_MENU_WIDTH >> 1 - menu_pixels_right_of_node());
  char buf[3] = {' '}; //to store converted byte
  byte val;
  switch (item) {
    case BRIGHTNESS_MENU:
      {
        itoa(screen_parameters.brightness, buf, 10);
        matrix.setCursor(12, center_of_menu - (sizeof(buf)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.print("-" + (String)buf + "+");
        break;
      }

    case TEXT_SIZE_MENU:
      {
        itoa(text_parameters[obj_num].text_size, buf, 10);
        matrix.setCursor(12, center_of_menu - (sizeof(buf)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.print("-" + (String)buf + "+");
        break;
      }

    case SCROLL_SPEED_MENU:
      {
        itoa(cursor_parameters[obj_num].x_dir, buf, 10);
        matrix.setCursor(12, center_of_menu - (sizeof(buf)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.print(minus_string + (String)buf + plus_string);
        break;
      }

    case FAN_SPEED_MENU:
      {
        itoa(menu_parameters.fan_speed, buf, 10);
        matrix.setCursor(12, center_of_menu - (sizeof(buf)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.print(minus_string + (String)buf + plus_string);
        break;
      }

    case MIN_FAN_SPEED_MENU:
      {
        itoa(menu_parameters.min_fan_speed, buf, 10);
        matrix.setCursor(12, center_of_menu - (sizeof(buf)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.print(minus_string + (String)buf + plus_string);
        break;
      }

    case LED_STRIP_BRIGHTNESS_MENU:
      {
        itoa(menu_parameters.led_strip_brightness, buf, 10);
        matrix.setCursor(12, center_of_menu - (sizeof(buf)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.print(minus_string + (String)buf + plus_string);
        break;
      }

    case TEXT_COLOUR_RED:
      {
        itoa(text_parameters[obj_num].colour_r, buf, 10);
        matrix.setCursor(12, center_of_menu - (sizeof(buf)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.print(minus_string + (String)buf + plus_string);
        break;
      }

    case TEXT_COLOUR_GREEN:
      {
        itoa(text_parameters[obj_num].colour_g, buf, 10);
        matrix.setCursor(12, center_of_menu - (sizeof(buf)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.print(minus_string + (String)buf + plus_string);
        break;
      }

    case TEXT_COLOUR_BLUE:
      {
        itoa(text_parameters[obj_num].colour_b, buf, 10);
        matrix.setCursor(12, center_of_menu - (sizeof(buf)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.print(minus_string + (String)buf + plus_string);
        break;
      }

    case TEXT_COLOUR_HUE:
      {
        itoa(text_parameters[obj_num].hue, buf, 10);
        matrix.setCursor(12, center_of_menu - (sizeof(buf)*ASCII_CHARACTER_BASIC_WIDTH) / 2);
        matrix.print(minus_string + (String)buf + plus_string);
        break;
      }
  }
}

void Graphics::write_enable_menu_item(byte item) {}



byte Graphics::menu_pixels_right_of_node() {
  return ((NUM_SCREENS - 1) - screen_parameters.node_address) * SINGLE_MATRIX_WIDTH; //number of pixels to the right of this screen

}
#endif //GRAPHICS_CPP
