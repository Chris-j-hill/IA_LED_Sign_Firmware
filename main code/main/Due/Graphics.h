#ifndef Graphics_H
#define Graphics_H




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

Text text;    

struct Text_cursor {        //xy coordinates of cursor position
  int x = 0;
  int y = 0;

  byte x_min = 0;
  
  bool mega1_updated = false;   //is the mega up to date with current positions, useful if not sending text displaying menu option
  bool mega2_updated = false;
  bool mega3_updated = false;
  bool mega4_updated = false;
};
Text_cursor text_cursor;












#endif  // Graphics_H
