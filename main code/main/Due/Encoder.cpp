
#ifndef Encoder_CPP
#define Encoder_CPP
#include "Arduino.h"
#include "Encoder.h"
//#include "Due.h"

#ifdef ENABLE_ENCODER
bool enable_encoder_on_startup = true;
#else
bool enable_encoder_on_startup = false;
#endif

#ifdef ENABLE_BUTTON
bool enable_button_on_startup = true;
#else
bool enable_button_on_startup = false;
#endif

bool button_enabled = false;


Encoder_Struct encoder_parameters;     //create encoder struct
Button_Struct button_parameters;       //create button struct


void init_encoder_ISR() {


  pinMode(encoder_parameters.pinA, INPUT);
  pinMode(encoder_parameters.pinB, INPUT);
  attachInterrupt(encoder_parameters.pinA, update_encoder_ISR, CHANGE);
  encoder_parameters.is_attached = true;
  encoder_parameters.enabled = true;

}

void init_button_ISR() {

  pinMode(button_parameters.button_pin, INPUT_PULLUP);
  attachInterrupt(button_parameters.button_pin, update_button_ISR,RISING);
  button_parameters.is_attached = true;
  button_parameters.enabled = true;
}

void update_encoder_ISR () {
  if (encoder_parameters.enabled) {
    encoder_parameters.aVal = digitalRead(encoder_parameters.pinA);
    if (encoder_parameters.aVal != encoder_parameters.pinALast) { // Means the knob is rotating
      // if the knob is rotating, we need to determine direction
      // We do that by reading pin B.
      if (digitalRead(encoder_parameters.pinB) != encoder_parameters.aVal) {  // Means pin A Changed first - We're Rotating Clockwise
        encoder_parameters.PosCount ++;

      } else {// Otherwise B changed first and we're moving CCW

        encoder_parameters.PosCount--;

      }
      encoder_parameters.position = encoder_parameters.PosCount / 2;

    }

    encoder_parameters.pinALast = encoder_parameters.aVal;
    encoder_parameters.encoder_moved_ISR = true;
  }
}

void update_button_ISR() {
  if (button_parameters.enabled) {
    if (millis() - button_parameters.last_button_pressed > button_parameters.button_press_interval) {
      Sprintln(F("Button Pressed"));
      button_parameters.last_button_pressed = millis();
      button_parameters.button_pressed_ISR = true;
    }
  }
}


int get_text_encoder_position(int byte_number) {  //function to return the MSB or LSB of the current hue value to send

  if (byte_number == 1) { //looking for MSB
    if (encoder_parameters.position < 0)
      return (abs(encoder_parameters.position) / 256);    //get quotient of absolute value and 256 rounded down

    else
      return (abs(encoder_parameters.position) / 256 + 128); //add 128 to indicate positve number
  }
  else if (byte_number == 2) { //LSB
    return (abs(encoder_parameters.position) % 256);    //get modulo of value and 256;
  }
  else {
    Sprintln("Error, cant get hue MSB/LSB, invalid byte number presented");
    return (-1);
  }
}


void Encoder::init_encoder() {
  if (enable_encoder_on_startup)
    this -> enable_encoder();

}

void Encoder::init_button() {
  if (enable_button_on_startup)
    this -> enable_button();

}

void Encoder::enable_encoder() {

  if (!encoder_parameters.enabled)
    encoder_parameters.enabled = true;

  if (!encoder_parameters.is_attached)
    init_encoder_ISR();

}

void Encoder::disable_encoder() {
  if (encoder_parameters.enabled)
    encoder_parameters.enabled = false;
}

void Encoder::enable_button() {
  if (!button_parameters.enabled)
    button_parameters.enabled = true;

  if (!button_parameters.is_attached)
    init_button_ISR();

}

void Encoder::disable_button() {
  if (button_parameters.enabled)
    button_parameters.enabled = false;
}

void Encoder::handle_interupts() {   // function to repond to an ISR.

  //ISR is short ideally meant to be a short piece of code, and can arrive at any point in the loop
  // this code forces all code that responds to an ISR based input to do so for one complete loop only, regardless of where in the loop the ISR may occur

  if (encoder_parameters.encoder_moved_ISR || encoder_parameters.encoder_moved) {   //if interrupt just happened or happened one loop ago

    if (encoder_parameters.encoder_moved_ISR) { //interrupt just happened
      encoder_parameters.encoder_moved_ISR = false;         //acknowlege interrupt just happened
      encoder_parameters.encoder_moved = true;             //set clean loop to true to allow funtions to execute next loop

      Sprintln(F("encoder interrupt detected"));
    }

    else if (encoder_parameters.encoder_moved) {
      encoder_parameters.encoder_moved = false;              // functions executed, set clean loop false until next ISR

      Sprintln(F("encoder interrupt handler completed"));
    }
  }


  if (button_parameters.button_pressed_ISR || button_parameters.button_pressed) {

    if (button_parameters.button_pressed_ISR) {
      button_parameters.button_pressed_ISR = false;
      button_parameters.button_pressed = true;

      Sprintln(F("button interrupt detected"));
    }
    else if (button_parameters.button_pressed) {
      button_parameters.button_pressed = false;

      Sprintln(F("button interrupt handler completed"));
    }
  }
}

#endif // Encoder_Cpp
