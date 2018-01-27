

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Local_Config.h"
#include "Arduino.h"


#define ASCII_CHARACTER_BASIC_WIDTH 6
#define ASCII_CHARACTER_BASIC_HEIGHT 9

#define POS_ISR_FREQUENCY 100    // 100 gives maximum speed 50 pixels/second, minimum 0.2 pixels/second (100/128)/2
                                 // Note: There may be severe stability issues with this at high speeds if delay 
                                 // correction is used due to overflows and amount the couner is incremented




struct Text_Struct{
  byte string [MAX_TWEET_SIZE];
  byte text_size = DEFAULT_TEXT_SIZE;
  byte colour_r = DEFAULT_TEXT_RED_BRIGHTNESS;
  byte colour_g = DEFAULT_TEXT_RED_BRIGHTNESS;
  byte colour_b = DEFAULT_TEXT_RED_BRIGHTNESS;
  byte hue = 0;
  bool use_hue = false;
};

struct Cursor_Struct{
  int global_x_pos = 0;   // position as recieved in frame
  int local_x_pos =0;     // relative position for this matrix
  int global_y_pos = 0; 
  int local_y_pos =0;     
  int x_dir = 0;         // direction and speed of text in x and y direction
  int y_dir = 0;   
  int local_min_x_pos = 0;  //how far the text will scroll off to the left 
  int local_max_x_pos = 0;  //how far the text will start to the right
  int local_min_y_pos = 0;  //how far the text will scroll up
  int local_max_y_pos = 0;  //how far the text will start down    
};

struct Screen_Struct{
  byte brightness =0;
  byte mode = 0;
  byte node_address =0;
};


void pos_update_ISR();    //ISR for updating the cursor position
// this ISR runs fast and updates a counter. Once the counter has 
// reached an overflow value it is reset, a pulse is send back to 
// the due and the position is incremented by the value x_dir and y_dir


class Graphics{

private:

    int pos_isr_period();
    void pos_isr_counter_overflow();
public:
    Graphics(){};
    int init_matrix();
    int init_matrix(int address);
    void increment_cursor_position(byte axis);
    void attach_pos_ISR();  
    void delay_pos_ISR(int value, byte counter); // advance or delay the counter based on value from due  
    void set_text_min();
    void set_text_max();

//    int set_refresh_rate(int rate);  //refresh rate of matrix library ( modify library to access this value)
};

#endif // GRAPHICS_H
