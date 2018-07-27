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

#define LED_STRIP_TIMER Timer1              //make sure these two lines match, see DueTimer.cpp
#define LED_STRIP_TIMER_INTERRUPT TC1_IRQn

// ______  non class functions _______

int attach_timer_led_strip() {

  //attach led strip interrupt
  if (!timers.led_strip_timer_attached && led_strip_parameters.enabled) {
    timers.led_strip_timer_attached = true;       //indicate the timer is attached

    LED_STRIP_TIMER.attachInterrupt(fade_led_strip);   //attach ISR
    byte fail = led_strip_init_freq();          // set the freq to based on the programmed interval

    if (fail != 0) {
      timers.led_strip_timer_attached = false;      //failed to attach
      return (1);    //stop code
    }

    timers.led_strip_timer_attached = true;       //indicate the timer is attached
    LED_STRIP_TIMER.start();
    NVIC_SetPriority (LED_STRIP_TIMER_INTERRUPT, LED_STRIP_PRIORITY);  //set priority of interrupt, see priority definitions for details and links

  }
  return (0);
}

byte led_strip_init_freq() {
  LED_STRIP_TIMER.setPeriod(led_strip_parameters.change_interval * 1000);    //initially set fast, and slow later if needed
  return (LED_STRIP_TIMER.getPeriod() != led_strip_parameters.change_interval * 1000);
}

byte Led_Strip::led_strip_set_freq() {      //function to set the frequency of the led strip interrupt, use at the end of a main loop iteration

  static int led_strip_period_update_timer = millis();

  if (millis() > led_strip_period_update_timer + LED_STRIP_PERIOD_UPDATE_INTERVAL) {
    if (!timers.led_strip_timer_attached) {
      Sprintln(F("From 'led_strip_set_freq': trying to set frequency but timer not attached"));
      return (1);
    }
    else {
      if (led_strip_parameters.target_brightness == led_strip_parameters.current_brightness) {
        if (led_strip_parameters.fast_interval) { //if values the same and using fast interval, set to slow interval
          LED_STRIP_TIMER.setPeriod(led_strip_parameters.led_stable_interval * 1000); //period in microseconds
          LED_STRIP_TIMER.start();
        }
        led_strip_parameters.fast_interval = false;
        return (0);
      }

      if (led_strip_parameters.target_brightness != led_strip_parameters.current_brightness) {
        if (!led_strip_parameters.fast_interval) { //if values not the same and using slow interval, set to fast interval
          LED_STRIP_TIMER.setPeriod(led_strip_parameters.change_interval * 1000);
          LED_STRIP_TIMER.start();
        }
        led_strip_parameters.fast_interval = true;
        return (0);
      }
    }
  }
}

void fade_led_strip() {         //ISR functon to fade led strip between current and target value by the increment value

  // timer based function to fade to the target value from the current value.
  // function should have the option of fading in a sinusoidal pattern or linearly.
  // when changing led brightness function should set interrupt based on "change_interval"
  // variable in led strip struct, when at target value, interrupt based on "ledstable_interval"
  // led_fast_interrupt dictates which is used, this is changed in function

  if (led_strip_parameters.enabled) { //only write values if enabled

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
}


int Led_Strip::init_led_strip() {     // initialise the led strip and set it to starting value

  if (enable_led_strip)
    this -> enable();
  else
    this -> disable();

}


int Led_Strip::writeLedStrips(int newValue) {     //function to fade in led strip from current value to new value

  led_strip_parameters.target_brightness = newValue;

}


void Led_Strip::enable() {

  pinMode(led_strip_parameters.pin, OUTPUT);
  if (!timers.led_strip_timer_attached) {
    attach_timer_led_strip();
  }
  led_strip_parameters.enabled = true;
}


void Led_Strip::disable() {
  led_strip_parameters.enabled = false;       //disable interrupt analogWrite
  pinMode(led_strip_parameters.pin, INPUT_PULLUP);   //disable pin
}

#endif //Led_Strip_Class_CPP
