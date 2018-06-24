
// File to contain all interrupt related functions, including inits.
// arduino doesnt handle interrupts as class methods well, keep them as seperate functions



//
//#ifndef Due_Interrupts_H
//#define Due_Interrupts_H
//
//#include "Arduino.h"
//
//#include "due_structs.h"
//
//#include "function_declarations.h"
//
//#include "Due_Libraries.h"


// ___________ DUE INTERRUPTS ______________

// interrupt for encoder




// ___________ button functions _____________
//
////interrrupt service routine for button press
//void update_button_ISR() {
//
//  //button pressed and been a while since last press
//  if (digitalRead(button.button_pin) == false && millis() - button.last_button_pressed >= button.button_press_interval) {
//    //if (debug)
//    Serial.println("Button Pressed");
//    button.last_button_pressed = millis();
//    button.button_pressed = true;
//  }
//}
//
//
////initialise button
//int init_button() {
//
//  pinMode(button.button_pin, INPUT);
//
//
//#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
//  if (button.button_pin != 2 && button.button_pin != 3 && button.button_pin != 18 && button.button_pin != 19  && button.button_pin != 20 && button.button_pin != 21) {
//   
//      Sprintln("error, button pin not an interrupt");
//      return(-1);
//   
//    
//    }
//  else
//    attachInterrupt(digitalPinToInterrupt(button), update_button_ISR, FALLING);
//#else
//  attachInterrupt(button.button_pin, update_button_ISR, CHANGE);
//#endif
//
//
//Serial.println(F("Button Initialised"));
//return(0);
//}
//

//timer interrupts
//void fade_led_strip() {         //ISR functon to fade led strip between current and target value by the increment value
//
//  // timer based function to fade to the target value from the current value.
//  // function should have the option of fading in a sinusoidal pattern or linearly.
//  // when changing led brightness function should set interrupt based on "change_interval"
//  // variable in led strip struct, when at target value, interrupt based on "ledstable_interval"
//  // led_fast_interrupt dictates which is used, this is changed in function
//
//  //if theres a difference, do something, ohterwise skip to end
//
//  if (led_strip.target_brightness != led_strip.current_brightness) {
//    if (!led_strip.sinusoidal) {
//      if (led_strip.current_brightness + led_strip.change_increment < led_strip.target_brightness) { //if after adding increment is less than target, increase current value and write to pin
//        led_strip.current_brightness = led_strip.current_brightness + led_strip.change_increment;
//        digitalWrite(led_strip.pin, led_strip.current_brightness);
//      }
//
//      else if (led_strip.current_brightness - led_strip.change_increment > led_strip.target_brightness) { //if after subtracting incrementing is grater than target, decrease current value and write to pin
//          led_strip.current_brightness = led_strip.current_brightness - led_strip.change_increment;
//          digitalWrite(led_strip.pin, led_strip.current_brightness);
//        }
//
//        else {      //otherwise we must be close to target, set equal
//          led_strip.current_brightness = led_strip.target_brightness;
//          digitalWrite(led_strip.pin, led_strip.current_brightness);
//        }
//    }
//
//    else {}     //implement sinusoidal changes, useful for pulsing the led strip to look awesome
//  }
//}

//int led_strip_set_freq() {      //function to set the frequency of the led strip interrupt, use at the end of a main loop iteration
//
//
//    if (!timers.led_strip_timer_attached){
//      Sprintln(F("From 'led_strip_set_freq': trying to set frequency but timer not attached"));
//      return (-1);
//    }
//    else{
//  if (led_strip.target_brightness == led_strip.current_brightness) {
//    if (led_strip.fast_interval)  //if values the same and using fast interval, set to slow interval
//      Timer1.setPeriod(led_strip.led_stable_interval*1000);   //period in microseconds
//    return (0);
//  }
//
//  if (led_strip.target_brightness != led_strip.current_brightness) {
//    if (!led_strip.fast_interval)  //if values not the same and using slow interval, set to fast interval
//      Timer1.setPeriod(led_strip.change_interval*1000);
//    return (0);
//  }
//}
//}


//
//void fade_fans() {          // interrupt to change the current value of the fans to approach the target value
//
//}
//
//int fans_set_freq(){}      //interrupt to set the frequency the fans are adjusted
//
//
//void send_pos_interrupt(){     // interrupt to send pos data to all megas
//    
//
//    text_cursor.x++;      //increment for now. need way to dynamically set base on cursor dir variables
//    send_pos_now = true;
//
//    
//    digitalWrite(13,!digitalRead(13));    //visual feedback
//
//
//}

//
//
//int set_pos_speed(int x_speed, int y_speed){            //function to set the speed (pixels per second) the cursor postion is moving at
//
//x_pos_dir = x_speed+128;    //shift up to allow negatives to be sent as bytes, make sure to shift down on recieve end
//y_pos_dir = y_speed+128;
//}
//
//int set_pos_update_frequency(int freq){
//  
//  if (!timers.pos_timer_attached){
//      Sprintln(F("From 'set_pos_update_frequency': trying to set frequency but timer not attached"));
//      return (-1);
//  }
//  else{   //all good, set freq
//    
//    pos_update_freq = freq;   //variable for frame
//    
//    Timer3.setFrequency(freq);   //set interval
//    
//    return(0);
//  }
//
//}

//int attach_timer_led_strip(){
//   
//  //attach led strip interrupt
//    if (!timers.led_strip_timer_attached && led_strip_enabled){
//      timers.led_strip_timer_attached = true;       //indicate the timer is attached
//      
//      Timer1.attachInterrupt(fade_led_strip);   //attach ISR
//      int fail = led_strip_set_freq();          // set the freq to based on the programmed interval
//      
//      if (fail !=0){
//        if (debug)
//            Serial.print(F("Failed to attach led strip timer"));
//        timers.led_strip_timer_attached = false;      //failed to attach
//        return(-1);     //stop code   
//      }
//      
//      timers.led_strip_timer_attached = true;       //indicate the timer is attached
//      Timer1.start();
//      if (debug){
//         Serial.println(F("Attached led strip timer"));
//      }
//    }
// 
//}
//int attach_timer_fan(){
//  //attach fan interrupt
//    if (!timers.fan_timer_attached & fans_enabled){
//      timers.fan_timer_attached = true;       //indicate the timer is attached
//      
//      Timer2.attachInterrupt(fade_fans);   //attach ISR
//      int fail = fans_set_freq();          // set the freq to based on the programmed interval
//      
//      if (fail !=0){
//        if (debug)
//            Serial.print(F("Failed to attach fan timer"));
//        timers.fan_timer_attached = false;       
//        return(-1);     //stop code   
//      }
//      
//      
//      Timer2.start();
//      if (debug){
//        Serial.println(F("Attached fan timer"));
//      }
//    }
//    
//    */
//  
//}
//int attach_timer_pos_update(){
//  //attach pos update interrupt
//    if (!timers.pos_timer_attached){
//      timers.pos_timer_attached = true;       //indicate the timer is attached
//      
//      Timer3.attachInterrupt(send_pos_interrupt);   //attach ISR
//      int fail = set_pos_update_frequency(pos_update_freq);         // set the freq
//      
//      if (fail !=0){
//        Sprintln(F("Failed to attach pos timer"));
//        timers.pos_timer_attached = false;       
//        return(-1);     //stop code   
//      }
//     
//      Timer3.start();
//      Sprintln(F("Attached pos timer"));
//    }
//}
//
//int attach_timer_interrupts(){            //attach all timer interrupts here, call in setup, after due_init()
//
//  // code to attach all the timers. this should be called soon after the program begins
//  // all timers need to be called seperately and attached individually, maybe possible to implent as a loop 
//  // but for now this is clearer
//  
//  
//  /*
//  //attach led strip interrupt
//    if (!timers.led_strip_timer_attached && led_strip_enabled){
//      timers.led_strip_timer_attached = true;       //indicate the timer is attached
//      
//      Timer1.attachInterrupt(fade_led_strip);   //attach ISR
//      int fail = led_strip_set_freq();          // set the freq to based on the programmed interval
//      
//      if (fail !=0){
//        if (debug)
//            Serial.print(F("Failed to attach led strip timer"));
//        timers.led_strip_timer_attached = false;      //failed to attach
//        return(-1);     //stop code   
//      }
//      
//      timers.led_strip_timer_attached = true;       //indicate the timer is attached
//      Timer1.start();
//      if (debug){
//         Serial.println(F("Attached led strip timer"));
//      }
//    }
//      
//  //attach fan interrupt
//    if (!timers.fan_timer_attached & fans_enabled){
//      timers.fan_timer_attached = true;       //indicate the timer is attached
//      
//      Timer2.attachInterrupt(fade_fans);   //attach ISR
//      int fail = fans_set_freq();          // set the freq to based on the programmed interval
//      
//      if (fail !=0){
//        if (debug)
//            Serial.print(F("Failed to attach fan timer"));
//        timers.fan_timer_attached = false;       
//        return(-1);     //stop code   
//      }
//      
//      
//      Timer2.start();
//      if (debug){
//        Serial.println(F("Attached fan timer"));
//      }
//    }
//    
//    */
//    
//    //attach pos update interrupt
//    if (!timers.pos_timer_attached){
//      timers.pos_timer_attached = true;       //indicate the timer is attached
//      
//      Timer3.attachInterrupt(send_pos_interrupt);   //attach ISR
//      int fail = set_pos_update_frequency(pos_update_freq);         // set the freq
//      
//      if (fail !=0){
//        Sprintln(F("Failed to attach pos timer"));
//        timers.pos_timer_attached = false;       
//        return(-1);     //stop code   
//      }
//      
//      
//      Timer3.start();
//      Sprintln(F("Attached pos timer"));
//      
//      
//    }
//    
//    return(0);    //once finished ever, return 0
//  
//}


#endif // Interupts_H
