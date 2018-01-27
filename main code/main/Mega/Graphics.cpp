

#ifndef GRAPHICS_CPP
#define GRAPHICS_CPP

#include "Graphics.h"
#include "Mega_Pins.h"
#include "libs/Timer3/TimerThree.h"
Text_Struct text_parameters;
Screen_Struct screen_parameters;
Cursor_Struct cursor_parameters;
extern Graphics graphics;


volatile byte x_pos_ISR_counter = 0;
volatile byte x_pos_isr_counter_overflow = 255;

volatile byte y_pos_ISR_counter = 0;
volatile byte y_pos_isr_counter_overflow = 255;

volatile bool suppress_x_pos_update = false;
volatile bool suppress_y_pos_update = false;

void pos_update_ISR() {   //ISR for updating the cursor position
  // this ISR runs fast and updates two counters. Once a counter has
  // reached an overflow value it is reset, a pulse is sent back to
  // the due and the position is incremented. this allows the cursor
  // speed to be anywhere between POS_ISR_FREQUENCY pixels/second and
  // POS_ISR_FREQUENCY/128 pixels/second


  x_pos_ISR_counter++;
  y_pos_ISR_counter++;

  if (x_pos_ISR_counter == x_pos_isr_counter_overflow && !suppress_x_pos_update) {
    x_pos_ISR_counter = 0;

#ifdef DELAY_FEEBDACK_PINS
    pinMode (DELAY_FEEDBACK_X_PIN, OUTPUT);   //drive line low on position update
    digitalWrite(DELAY_FEEDBACK_X_PIN, LOW);  //direct port manipulation?
#endif

    graphics.increment_cursor_position(1);

  }
  else if (x_pos_ISR_counter == x_pos_isr_counter_overflow && suppress_x_pos_update)
    suppress_x_pos_update = false;



  if (y_pos_ISR_counter == y_pos_isr_counter_overflow && !suppress_y_pos_update) {
    y_pos_ISR_counter = 0;

#ifdef DELAY_FEEBDACK_PINS
    pinMode (DELAY_FEEDBACK_X_PIN, OUTPUT);
    digitalWrite(DELAY_FEEDBACK_X_PIN, LOW);
#endif

    graphics.increment_cursor_position(2);


  }
  else if (y_pos_ISR_counter == y_pos_isr_counter_overflow && suppress_y_pos_update)
    suppress_y_pos_update = false;



#ifdef DELAY_FEEBDACK_PINS
  // set these as inputs to allow due to pull line high after pos update
  if (x_pos_ISR_counter == 1)
    pinMode(DELAY_FEEDBACK_X_PIN, INPUT);
  if (y_pos_ISR_counter == 1)
    pinMode(DELAY_FEEDBACK_Y_PIN, INPUT);
#endif
}


void Graphics::attach_pos_ISR() {
  int period = pos_isr_period();
  this -> pos_isr_counter_overflow();

  Timer3.attachInterrupt(pos_update_ISR, period);
  Timer3.start();
}

void Graphics::delay_pos_ISR(int value, byte counter) {

  if (counter == 1) { // increment x
    if (x_pos_ISR_counter + value < 0 ) { //decreasing counter and near 0
      value += x_pos_ISR_counter;
      x_pos_ISR_counter = x_pos_isr_counter_overflow + value;
      suppress_x_pos_update = true;
    }
    else if (x_pos_ISR_counter + value >= x_pos_isr_counter_overflow) { //account for overflow,also incrment cursor manually
      value -= (x_pos_isr_counter_overflow - x_pos_ISR_counter);
      x_pos_ISR_counter = value;
      this -> increment_cursor_position(1);
    }
    else
      x_pos_ISR_counter += value; // just move the
  }

  else if (counter == 2) { // increment y
    if (y_pos_ISR_counter + value < 0 ) { //decreasing counter and near 0
      value += y_pos_ISR_counter;
      y_pos_ISR_counter = y_pos_isr_counter_overflow + value;
      suppress_y_pos_update = true;
    }
    else if (y_pos_ISR_counter + value >= y_pos_isr_counter_overflow) { //account for overflow,also increment cursor manually
      value -= (y_pos_isr_counter_overflow - y_pos_ISR_counter);
      x_pos_ISR_counter = value;
      this -> increment_cursor_position(2);
    }
    else
      y_pos_ISR_counter += value; // just move the

  }
}

void Graphics::increment_cursor_position(byte axis) {

  if (axis == 1) {
    if (cursor_parameters.x_dir < 0) {
      cursor_parameters.global_x_pos --;
      cursor_parameters.local_x_pos --;
    }
    else if (cursor_parameters.x_dir > 0) {
      cursor_parameters.global_x_pos ++;
      cursor_parameters.local_x_pos ++;
    }
  }

  else if (axis == 2) {
    if (cursor_parameters.y_dir < 0) {
      cursor_parameters.global_y_pos --;
      cursor_parameters.local_y_pos --;
    }
    else if (cursor_parameters.y_dir > 0) {
      cursor_parameters.global_y_pos ++;
      cursor_parameters.local_y_pos ++;
    }
  }
}


int Graphics::pos_isr_period(){

  return(1000/POS_ISR_FREQUENCY);
  
}

void Graphics::pos_isr_counter_overflow(){
  
  x_pos_isr_counter_overflow = abs(cursor_parameters.x_dir)*2;  // *2 doubles range to increase of overflow to improve achievable scroll speed
  y_pos_isr_counter_overflow = abs(cursor_parameters.y_dir)*2;  
  
}

void Graphics::set_text_min() {
  //how far to the left will the text scroll
  cursor_parameters.local_min_x_pos = -1 * (screen_parameters.node_address * SINGLE_MATRIX_WIDTH) * (text_parameters.text_size * ASCII_CHARACTER_BASIC_WIDTH);
}

void Graphics::set_text_max() {
  //how far to the left will the text scroll
  cursor_parameters.local_max_x_pos = screen_parameters.node_address * SINGLE_MATRIX_WIDTH;
}

#endif //GRAPHICS_CPP
