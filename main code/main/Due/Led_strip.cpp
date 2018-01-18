#ifndef Led_Strip_Class_CPP
#define Led_Strip_Class_CPP 


#include "Led_Strip.h"
#include "Arduino.h"
#include "DueTimer.h"
#include "function_declarations.h"


Led_Strip_Struct led_strip_parameters;              //create led strip struct

extern struct Timers timers;
#ifdef ENABLE_LED_STRIP
bool enable_led_strip = true;
#else
bool enable_led_strip = false;
#endif

bool led_strip_enabled = false;

byte led_strip_brightness = LED_STRIP_DEFUALT_BRIGHTNESS;




// ______  non class functions _______

int attach_timer_led_strip(){
   
  //attach led strip interrupt
    if (!timers.led_strip_timer_attached && led_strip_enabled){
      timers.led_strip_timer_attached = true;       //indicate the timer is attached
      
      Timer1.attachInterrupt(fade_led_strip);   //attach ISR
      int fail = led_strip_set_freq();          // set the freq to based on the programmed interval
      
      if (fail !=0){
        Sprintln(F("Failed to attach led strip timer"));
        timers.led_strip_timer_attached = false;      //failed to attach
        return(-1);     //stop code   
      }
      
      timers.led_strip_timer_attached = true;       //indicate the timer is attached
      Timer1.start();
      Sprintln(F("Attached led strip timer")); 
    }
}

int led_strip_set_freq() {      //function to set the frequency of the led strip interrupt, use at the end of a main loop iteration


    if (!timers.led_strip_timer_attached){
      Sprintln(F("From 'led_strip_set_freq': trying to set frequency but timer not attached"));
      return (-1);
    }
    else{
  if (led_strip_parameters.target_brightness == led_strip_parameters.current_brightness) {
    if (led_strip_parameters.fast_interval)  //if values the same and using fast interval, set to slow interval
      Timer1.setPeriod(led_strip_parameters.led_stable_interval*1000);   //period in microseconds
    return (0);
  }

  if (led_strip_parameters.target_brightness != led_strip_parameters.current_brightness) {
    if (!led_strip_parameters.fast_interval)  //if values not the same and using slow interval, set to fast interval
      Timer1.setPeriod(led_strip_parameters.change_interval*1000);
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

  if (led_strip_parameters.target_brightness != led_strip_parameters.current_brightness) {
    if (!led_strip_parameters.sinusoidal) {   //linear fade
      if (led_strip_parameters.current_brightness + led_strip_parameters.change_increment < led_strip_parameters.target_brightness) { //if after adding increment is less than target, increase current value and write to pin
        led_strip_parameters.current_brightness = led_strip_parameters.current_brightness + led_strip_parameters.change_increment;
        analogWrite(led_strip_parameters.pin, led_strip_parameters.current_brightness);
      }

      else if (led_strip_parameters.current_brightness - led_strip_parameters.change_increment > led_strip_parameters.target_brightness) { //if after subtracting incrementing is grater than target, decrease current value and write to pin
          led_strip_parameters.current_brightness = led_strip_parameters.current_brightness - led_strip_parameters.change_increment;
          analogWrite(led_strip_parameters.pin, led_strip_parameters.current_brightness);
        }

        else {      //otherwise we must be close to target, set equal
          led_strip_parameters.current_brightness = led_strip_parameters.target_brightness;
          analogWrite(led_strip_parameters.pin, led_strip_parameters.current_brightness);
        }
    }

    else { //implement sinusoidal changes, useful for pulsing the led strip to look awesome
      Sprintln(F("Trying to implement led strip sinusoidal fade, function not implemented"));
      
      }     
  }
}




// ______  non class functions _______

int Led_Strip::init_led_strip() {     // initialise the led strip and set it to starting value

  pinMode(led_strip_parameters.pin, OUTPUT);
  
  Sprintln(F("\t Set led strip to value..."));
  int fail = writeLedStrips(led_strip_parameters.target_brightness);  // start leds and fade to target brightness

  if (fail != 0) {
    Sprintln(F("Failed to write initial led strip brightness"));
    return (-1);
  }
  else
    return (0);
}


int Led_Strip::writeLedStrips(int newValue) {     //function to fade in led strip from current value to new value

  // function will set new value, and attach interrupt if not attached already
  // when target value is reached, isr will set ISR_attached to false and isr will be detached at the end of the loop


}


#endif //Led_Strip_Class_CPP