#ifndef Led_Strip_Class_CPP
#define Led_Strip_Class_CPP 

#include "base_class.h"
#include "Due_Class.h"
#include "Due.h"
#include "Arduino.h"

// ______  non class functions _______

int attach_timer_led_strip(){
   
  //attach led strip interrupt
    if (!timers.led_strip_timer_attached && led_strip_enabled){
      timers.led_strip_timer_attached = true;       //indicate the timer is attached
      
      Timer1.attachInterrupt(fade_led_strip);   //attach ISR
      int fail = led_strip_set_freq();          // set the freq to based on the programmed interval
      
      if (fail !=0){
        if (debug)
            Serial.print(F("Failed to attach led strip timer"));
        timers.led_strip_timer_attached = false;      //failed to attach
        return(-1);     //stop code   
      }
      
      timers.led_strip_timer_attached = true;       //indicate the timer is attached
      Timer1.start();
      if (debug){
         Serial.println(F("Attached led strip timer"));
      }
    }
 
}

int led_strip_set_freq() {      //function to set the frequency of the led strip interrupt, use at the end of a main loop iteration


    if (!timers.led_strip_timer_attached){
      Sprintln(F("From 'led_strip_set_freq': trying to set frequency but timer not attached"));
      return (-1);
    }
    else{
  if (led_strip.target_brightness == led_strip.current_brightness) {
    if (led_strip.fast_interval)  //if values the same and using fast interval, set to slow interval
      Timer1.setPeriod(led_strip.led_stable_interval*1000);   //period in microseconds
    return (0);
  }

  if (led_strip.target_brightness != led_strip.current_brightness) {
    if (!led_strip.fast_interval)  //if values not the same and using slow interval, set to fast interval
      Timer1.setPeriod(led_strip.change_interval*1000);
    return (0);
  }
}
}

void fade_led_strip() {         //ISR functon to fade led strip between current and target value by the increment value

  // timer based function to fade to the target value from the current value.
  // function should have the option of fading in a sinusoidal pattern or linearly.
  // when changing led brightness function should set interrupt based on "change_interval"
  // variable in led strip struct, when at target value, interrupt based on "ledstable_interval"
  // led_fast_interrupt dictates which is used, this is changed in function

  //if theres a difference, do something, ohterwise skip to end

  if (led_strip.target_brightness != led_strip.current_brightness) {
    if (!led_strip.sinusoidal) {
      if (led_strip.current_brightness + led_strip.change_increment < led_strip.target_brightness) { //if after adding increment is less than target, increase current value and write to pin
        led_strip.current_brightness = led_strip.current_brightness + led_strip.change_increment;
        digitalWrite(led_strip.pin, led_strip.current_brightness);
      }

      else if (led_strip.current_brightness - led_strip.change_increment > led_strip.target_brightness) { //if after subtracting incrementing is grater than target, decrease current value and write to pin
          led_strip.current_brightness = led_strip.current_brightness - led_strip.change_increment;
          digitalWrite(led_strip.pin, led_strip.current_brightness);
        }

        else {      //otherwise we must be close to target, set equal
          led_strip.current_brightness = led_strip.target_brightness;
          digitalWrite(led_strip.pin, led_strip.current_brightness);
        }
    }

    else {}     //implement sinusoidal changes, useful for pulsing the led strip to look awesome
  }
}




// ______  non class functions _______

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
