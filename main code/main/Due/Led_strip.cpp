#ifndef Led_Strip_Class_CPP
#define Led_Strip_Class_CPP 

#include "base_class.h"
#include "Due_Class.h"
#include "Due.h"



//methods for led_strip class


int led_strip::init_led_strip() {     // initialise the led strip and set it to starting value

  pinMode(led_strip.pin, OUTPUT);
  
  Sprintln(F("\t Set led strip to value..."));
  int fail = writeLedStrips(led_strip.target_brightness);  // start leds and fade to target brightness

  if (fail != 0) {
    Sprintln(F("Failed to write initial led strip brightness"));
    return (-1);
  }
  else
    return (0);
}


int led_strip::writeLedStrips(int newValue) {     //function to fade in led strip from current value to new value

  // function will set new value, and attach interrupt if not attached already
  // when target value is reached, isr will set ISR_attached to false and isr will be detached at the end of the loop


}


#endif //Led_Strip_Class_CPP
