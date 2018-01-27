

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Local_Config.h"
#include "Arduino.h"


#define ASCII_CHARACTER_BASIC_WIDTH 6
#define ASCII_CHARACTER_BASIC_HEIGHT 9

struct Text_Struct{
  byte string [MAX_TWEET_SIZE];
  byte text_size =1;
  byte colour_r = 7;
  byte colour_g = 7;
  byte colour_b = 7;
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



class Graphics{

private:

public:
    Graphics(){};
    int init_matrix();
    int init_matrix(int address);

    void set_text_min();
    void set_text_max();

//    int set_refresh_rate(int rate);  //refresh rate of matrix library ( modify library to access this value)
};

#endif // GRAPHICS_H
