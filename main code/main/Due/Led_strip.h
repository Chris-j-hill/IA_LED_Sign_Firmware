#ifndef Led_Strip_Class_H
#define Led_Strip_Class_H

//#include "Due_Libraries.h"
#include "Config_Local.h"
#include "Due_Pins.h"


#define LED_STRIP_DEFUALT_BRIGHTNESS 255   


struct Led_Strip_Struct {
  int pin = LED_STRIP;                  // attached to
  int target_brightness = LED_STRIP_DEFUALT_BRIGHTNESS;        // value to approach
  int current_brightness = 0;       // current value written to strip
  int change_increment = 5;           // value to increment by to approach target
  int change_interval = 40;           // interrupt period between incrementing value
  int led_stable_interval = 500;          // interrupt period when target=current brightness
  int minimum_on = 100;                  // minimum value where the leds are on
  bool enabled = false;                   
  bool fast_interval = true;          // use change_interval if true as interrupt period, otherwise led_stable_interval
  bool sinusoidal = false;              //set true if using a sinusoidal method to change between 
  int sinusoidal_half_frequency = 1;         // time, in seconds, to go from one value to another, changing values will be a half sign wave 

};

int attach_timer_led_strip();


int led_strip_set_freq();     //function to set the frequency of the led strip interrupt, use at the end of a main loop iteration

void fade_led_strip();          //ISR functon to fade led strip between current and target value by the increment value



class Led_Strip{

  private:



  public:


    Led_Strip(){}
    int init_led_strip();           //initialisations of individual items, set pins, test if working if applicable, etc
    int writeLedStrips(int newValue);       // change value of led strips, attach timers to fade in an out over period
    void enable();
    void disable(); 
     
};

#endif //Led_Strip_Class_H
