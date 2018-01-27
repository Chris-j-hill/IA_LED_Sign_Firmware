

#ifndef GRAPHICS_CPP
#define GRAPHICS_CPP

#include "Graphics.h"

Text_Struct text_parameters;
Screen_Struct screen_parameters;
Cursor_Struct cursor_parameters;

int text_min = 0;   //Local_minimum 

void Graphics::set_text_min(){
  //how far to the left will the text scroll
  cursor_parameters.local_min_x_pos= -1*(screen_parameters.node_address*SINGLE_MATRIX_WIDTH)*(text_parameters.text_size*ASCII_CHARACTER_BASIC_WIDTH);
}

void Graphics::set_text_max(){
  //how far to the left will the text scroll
  cursor_parameters.local_max_x_pos= screen_parameters.node_address*SINGLE_MATRIX_WIDTH;
}

#endif //GRAPHICS_CPP
