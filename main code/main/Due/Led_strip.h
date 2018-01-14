#ifndef Led_Strip_Class_H
#define Led_Strip_Class_H

#include "Due_Libraries.h"


class led_strip{

  private:



  public:
    led_strip(){}
    int init_led_strip();           //initialisations of individual items, set pins, test if working if applicable, etc
    int writeLedStrips(int newValue);       // change value of led strips, attach timers to fade in an out over period
       

    //TODO
    int toggle_led();                   // toggle the on/off bool, detach interrupt if off
    int is_led_on();                    //useful in menus
    int return_led_brightness();
    int led_gradual_pulse(int minValue, int maxValue);  //code to fade the leds in and out in sinusoidal pattern
    
     
};

#endif //Led_Strip_Class_H
