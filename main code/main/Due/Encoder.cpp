
#ifndef Encoder_CPP
#define Encoder_CPP     
#include "Arduino.h"
#include "Encoder.h"
#include "Due.h"




int init_encoder() {
  if (enable_encoder && !encoder_enabled) {

    pinMode(encoder.pinA, INPUT);
    pinMode(encoder.pinB, INPUT);

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    if (encoder.pinA != 2 && encoder.pinA != 3 && encoder.pinA != 18 && encoder.pinA != 19 && encoder.pinA != 20 && encoder.pinA != 21) {
      Sprintln("error, encoder pin not an interrupt");
      return (-1);
    }
    else
      attachInterrupt(digitalPinToInterrupt(encoder.pinA), update_encoder_ISR, CHANGE);
#else
    attachInterrupt(encoder.pinA, update_encoder_ISR, CHANGE);
#endif

    return (0);
  }

  else {
    Sprintln(F("Conflict with enabling encoder: make sure only enabled once and 'enable_encoder' is true"));
    return (-1);
  }

}

int init_button() {

  pinMode(button.button_pin, INPUT);


#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  if (button.button_pin != 2 && button.button_pin != 3 && button.button_pin != 18 && button.button_pin != 19  && button.button_pin != 20 && button.button_pin != 21) {
   
      Sprintln("error, button pin not an interrupt");
      return(-1);
   
    
    }
  else
    attachInterrupt(digitalPinToInterrupt(button), update_button_ISR, FALLING);
#else
  attachInterrupt(button.button_pin, update_button_ISR, CHANGE);
#endif


Serial.println(F("Button Initialised"));
return(0);
}

void update_encoder_ISR () {
  encoder.aVal = digitalRead(encoder.pinA);
  if (encoder.aVal != encoder.pinALast) { // Means the knob is rotating
    // if the knob is rotating, we need to determine direction
    // We do that by reading pin B.
    if (digitalRead(encoder.pinB) != encoder.aVal) {  // Means pin A Changed first - We're Rotating Clockwise
      encoder.PosCount ++;

    } else {// Otherwise B changed first and we're moving CCW

      encoder.PosCount--;

    }
    encoder.position = encoder.PosCount / 2;

  }

  encoder.pinALast = encoder.aVal;
  encoder.encoder_moved = true;
}

void update_button_ISR() {

  if (digitalRead(button.button_pin) == false && millis() - button.last_button_pressed >= button.button_press_interval) {
    Sprintln(F("Button Pressed"));
    button.last_button_pressed = millis();
    button.button_pressed = true;
  }
}

#endif // Encoder_Cpp
