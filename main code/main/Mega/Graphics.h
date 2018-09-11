
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Local_Config.h"
#include "Arduino.h"


#define ASCII_CHARACTER_BASIC_WIDTH 6
#define ASCII_CHARACTER_BASIC_HEIGHT 8

#define POS_ISR_FREQUENCY 1    // 100 gives maximum speed 50 pixels/second, minimum 0.2 pixels/second (100/128)/2
// Note: There may be severe stability issues with this at high speeds if delay
// correction is used due to overflows and amount the counter is incremented

#define SERIAL_CHECK_FREQ 1000  // check for serial.available() this many times a second, cant allow for buffer overflow due to slower functions...

#define MIN_DISPLAY_UPDATE_PERIOD 20  //20ms period = 50hz

#define SCREEN_UPDATE_BACKOFF_PERIOD 50// after recieving frame requiring a screen update, wait a bit, to see if others are coming 

#if SCREEN_UPDATE_BACKOFF_PERIOD > (1000/POS_UPDATE_ISR_FREQ)
#error "SCREEN_UPDATE_BACKOFF_PERIOD cannot be larger than period of POS_UPDATE_ISR_FREQ, ensure significantly smaller"
#endif

// Similar to F(), but for PROGMEM string pointers rather than literals
#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr


struct Text_Struct {
  byte string [MAX_TWEET_SIZE] = "No due found";
  byte text_size = DEFAULT_TEXT_SIZE;
  byte colour_r = DEFAULT_TEXT_RED_BRIGHTNESS;
  byte colour_g = DEFAULT_TEXT_GREEN_BRIGHTNESS;
  byte colour_b = DEFAULT_TEXT_BLUE_BRIGHTNESS;
  int hue = 0;
  bool use_hue = false;

  //  byte x_min = 0; //region to allow text to be displayed
  //  byte x_max = SINGLE_MATRIX_WIDTH;
  //  bool hard_limit = false;    //limit with regards to text diaplayed while menu active
  //  bool limit_enabled = false; //is above limit enabled

  bool object_used = false;
};

struct Cursor_Struct {
  int global_x_pos = 0;   // position as recieved in frame
  int local_x_pos = 0;    // relative position for this matrix

  int global_y_pos = 0;
  int local_y_pos = 0;

  int8_t x_dir = 0;         // direction and speed of text in x and y direction
  int8_t y_dir = 0;

};

struct Screen_Struct {
  byte brightness = 100;
  byte new_mode = DEFAULT_SCREEN_MODE;
  byte cur_mode = 255;  //force set on startup
  byte node_address = 0;
  bool updated = false;
  uint32_t time_last_updated = 0; //do not update screen immediately if screen update arrives, usually many frames arriving at once, wait until we suspect all arrived
  uint16_t update_periods[2 * MAX_NUM_OF_TEXT_OBJECTS] = {1000}; //array of maximum update periods for each object in both axis
  uint32_t cur_update_period  = 1000; // smallest value in above array
  uint32_t last_updated[2 * MAX_NUM_OF_TEXT_OBJECTS] = {0};

};

struct Object_Struct_Polygons {  //colours of thing that isnt text

  byte red = 0;
  byte green = 0;
  byte blue = 0;
  bool enabled = false;
  int16_t x_pos = 0;      //coordinates of object
  int16_t y_pos = 0;
  byte size1 = 0;
  byte size2 = 0;
};

struct Object_Struct_Circles {  //colours of thing that isnt text

  byte red = 0;
  byte green = 0;
  byte blue = 0;
  bool enabled = false;
  int16_t x_pos = 0;      //coordinates of object
  int16_t y_pos = 0;
  byte radius = 0;

};

void pos_update_ISR();    //ISR for updating the cursor position
// this ISR runs fast and updates a counter. Once the counter has
// reached an overflow value it is reset, a pulse is send back to
// the due and the position is incremented by the value x_dir and y_dir

void serial_check_ISR();


class Graphics {

  private:

    inline uint32_t pos_isr_period();
    //void pos_isr_counter_overflow();
    byte menu_pixels_right_of_node();

    void set_title_colour();  //set the colour as the title colour
    void set_menu_colour();
    void set_menu_colour_highlighted();

    //void update_menus();
    inline void set_text_colour(byte new_r, byte new_g, byte new_b);
    inline void set_text_colour(int new_hue);
    inline void draw_text(byte obj_num);
    inline void set_display_mode();


    inline uint32_t serial_check_ISR_period() {
      return 1000000 / SERIAL_CHECK_FREQ; //microseconds between check serial
    }

    void cpy_pgm_string(char *dest, byte src);
    void print_pgm_title(byte src, byte len, byte center);
    void print_pgm_menu_item(byte src, byte len, byte row);
    void set_menu_item_cursor(byte row);

    void print_pgm_menu_item_scrolling(byte src, byte len, byte row);
    void set_menu_item_cursor_scrolling(byte row);

    
    inline void fill_title_background();
    inline void draw_menu_pointer();
    
    inline void print_highlight_pgm_menu_item(byte src, byte len, byte row);
    inline void print_highlight_pgm_menu_item_scrolling(byte src, byte len, byte row){};
  public:
    Graphics() {};
    void init_matrix();
    void init_update_display_isr(); //need to check serial very frequently or else buffer overflow will occur
    void update_display();

    //cursor functions
    void increment_cursor_position(byte axis, byte obj_num = 0);
    void attach_pos_ISR();

    inline void set_pos_isr_period(uint32_t period);

    //void delay_pos_ISR(int value, byte counter); // advance or delay the counter based on value from due
    void interpolate_pos();

    // text functions
    void set_object_colour(byte new_r, byte new_g, byte new_b);

    void init_menu_title_colour();    //since common struct cant define defualt there, needs seperate function
    void init_menu_option_colour();

    void write_title(byte title);
    void write_menu_option(byte first, byte second, byte third);
    void write_adjustment_menu(byte item, byte obj_num = 0);
    void write_enable_menu_item(byte item);   // animation to confirm click on enable/disable menu items
    void write_menu_scrolling(byte first, byte second, byte third, byte fourth, byte fifth, byte dir);


    void draw_text_restricted_width(byte x_min, byte x_max, bool hard_limit);   // draw text from min to max. hard_limit is used to prevent anything being written beyond limits if true
    // full characters will be completed beyond limits if hard_limit is false


    // object primative functions
    void draw_objects(); // draw all enabled objects
    void draw_ring(byte x_center, byte y_center, uint16_t radius);         //global x/y coordinates



    byte non_linear_startup_function(uint16_t x);

    void draw_background();   //if menu visible, draw partial background if screen not entirely covered
    void clear_area(byte top_left_x, byte top_left_y, byte bottom_right_x, byte bottom_right_y);  // clear any data in this part of the matrix, useful for clearing menu but not background etc
    //    int set_refresh_rate(int rate);  //refresh rate of matrix library ( modify library to access this value)


    void update_pos_isr_period(byte obj_num, uint32_t new_x_val, uint32_t new_y_val);
};

#endif // GRAPHICS_H
