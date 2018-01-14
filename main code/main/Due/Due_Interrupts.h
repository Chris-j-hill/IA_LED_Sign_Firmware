
// File to contain all interrupt related functions, including inits.
// arduino doesnt handle interrupts as class methods well, keep them as seperate functions




#ifndef Due_Interrupts_H
#define Due_Interrupts_H

#include "Arduino.h"

#include "due_structs.h"

#include "function_declarations.h"

#include "Due_Libraries.h"


// ___________ DUE INTERRUPTS ______________

// interrupt for encoder
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


// ___________ button functions _____________

//interrrupt service routine for button press
void update_button_ISR() {

  //button pressed and been a while since last press
  if (digitalRead(button.button_pin) == false && millis() - button.last_button_pressed >= button.button_press_interval) {
    //if (debug)
    Serial.println("Button Pressed");
    button.last_button_pressed = millis();
    button.button_pressed = true;
  }
}


//initialise button
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


//timer interrupts
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



void fade_fans() {          // interrupt to change the current value of the fans to approach the target value

}

int fans_set_freq(){}      //interrupt to set the frequency the fans are adjusted


void send_pos_interrupt(){     // interrupt to send pos data to all megas
    

    text_cursor.x++;      //increment for now. need wa to dynamically set base on cursor dir variables
    send_pos_now = true;

    
    digitalWrite(13,!digitalRead(13));    //visual feedback


}



int set_pos_speed(int x_speed, int y_speed){            //function to set the speed (pixels per second) the cursor postion is moving at

x_pos_dir = x_speed+128;    //shift up to allow negatives to be sent as bytes, make sure to shift down on recieve end
y_pos_dir = y_speed+128;
}

int set_pos_update_frequency(int freq){
  
  if (!timers.pos_timer_attached){
      Sprintln(F("From 'set_pos_update_frequency': trying to set frequency but timer not attached"));
      return (-1);
    }
    
  
    else{   //all good, set freq
      
    pos_update_freq = freq;   //variable for frame
    
    Timer3.setFrequency(freq);   //set interval
    
    return(0);
    }
  
}

int attach_timer_interrupts(){            //attach all timer interrupts here, call in setup, after due_init()

  // code to attach all the timers. this should be called soon after the program begins
  // all timers need to be called seperately and attached individually, maybe possible to implent as a loop 
  // but for now this is clearer
  
  
  /*
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
      
  //attach fan interrupt
    if (!timers.fan_timer_attached & fans_enabled){
      timers.fan_timer_attached = true;       //indicate the timer is attached
      
      Timer2.attachInterrupt(fade_fans);   //attach ISR
      int fail = fans_set_freq();          // set the freq to based on the programmed interval
      
      if (fail !=0){
        if (debug)
            Serial.print(F("Failed to attach fan timer"));
        timers.fan_timer_attached = false;       
        return(-1);     //stop code   
      }
      
      
      Timer2.start();
      if (debug){
        Serial.println(F("Attached fan timer"));
      }
    }
    
    */
    
    //attach pos update interrupt
    if (!timers.pos_timer_attached){
      timers.pos_timer_attached = true;       //indicate the timer is attached
      
      Timer3.attachInterrupt(send_pos_interrupt);   //attach ISR
      int fail = set_pos_update_frequency(pos_update_freq);         // set the freq
      
      if (fail !=0){
        Sprintln(F("Failed to attach pos timer"));
        timers.pos_timer_attached = false;       
        return(-1);     //stop code   
      }
      
      
      Timer3.start();
      Sprintln(F("Attached pos timer"));
      
      
    }
    
    return(0);    //once finished ever, return 0
  
}


//code to send the xy position frames. copyied from due class as cant access it from here

//int wire_write_frame(int address) {   //function to actually send the frame to given address
//
//  if (!mega_enabled[address - 1]) {
//    if (debug)
//      //Serial.println(F("Mega disabled, did not attempt transmission"));
//      return (-1);
//  }
//
//  if (address == 1) {
//    for (int i = 0; i < frame.frame_buffer[0]; i++) {
//      Serial_1.write(frame.frame_buffer[i]);
//      // if (debug){
//      //   Serial.print((byte)frame.frame_buffer[i]);
//      //   Serial.print(" ");
//      // }
//    }
//  }
//  else if (address == 2) {
//    for (int i = 0; i < frame.frame_buffer[0]; i++) {
//      Serial_2.write(frame.frame_buffer[i]);
//      // if (debug){
//      //   Serial.print((byte)frame.frame_buffer[i]);
//      //   Serial.print(" ");
//      // }
//    }
//  }
//  else if (address == 3) {
//    for (int i = 0; i < frame.frame_buffer[0]; i++) {
//      Serial_3.write(frame.frame_buffer[i]);
//      // if (debug){
//      //   Serial.print((byte)frame.frame_buffer[i]);
//      //   Serial.print(" ");
//      // }
//    }
//  }
//  else  if (address == 4) {
//    for (int i = 0; i < frame.frame_buffer[0]; i++) {
//      Serial_4.write(frame.frame_buffer[i]);
//      // if (debug){
//      //   Serial.print((byte)frame.frame_buffer[i]);
//      //   Serial.print(" ");
//      // }
//    }
//  }
//  else {
//    return -1;
//  }
//  
//  // if(debug)
//  // Sprintln("");
//
//  // int error = Wire1.endTransmission();
//  // if (error != 0 && debug)    //error sending
//  // {
//  //   Serial.print("Error from 'endTransmission', error: ");
//  //   Serial.println(error);
//  // }
//
//  //clear frame from last iteration
//  for (int beta = 0; beta < 32; beta++) {
//    frame.frame_buffer[beta] = 0;
//  }
//
//  return (0);
//}



//int send_pos_frame(int address) {
//
//  int type = 2;
//  Serial.print("Sending cursor positions to address ");
//  Serial.println(address);
//
//  frame.frame_buffer[0] = (byte) pos_frame_length;   //build frame
//  frame.frame_buffer[1] = (byte) type;
//  frame.frame_buffer[2] = (byte) 1;
//  frame.frame_buffer[3] = (byte) 1;
//
//  pack_xy_coordinates();        //seperate function to bit shift values to correct order.
//
//  frame.frame_buffer[8] = (byte) x_pos_dir;
//  frame.frame_buffer[9] = (byte) x_pos_dir;
//  frame.frame_buffer[10] = (byte) comms_delay;
//  frame.frame_buffer[11] = (byte) pos_update_freq;
//
//  frame.frame_buffer[pos_frame_length - 1] = 0; //zero checksum
//  for (int alpha = 0; alpha < pos_frame_length - 2; alpha++) { //sum of elements
//    frame.frame_buffer[pos_frame_length - 1] = frame.frame_buffer[pos_frame_length - 1] + frame.frame_buffer[alpha];
//  }
//  frame.frame_buffer[pos_frame_length - 1] = (byte) 256 - (frame.frame_buffer[pos_frame_length - 1] % 256); //calc checksum
//
//  wire_write_frame(address);    //send frame
//
//  return (0);
//}

//int pack_xy_coordinates() {       //function to pack the 4 bytes to send the x and y positions of the text cursor
//
//  // Wire.write can only handle bytes (0-256) whereas we will need to use positive and negaitve values potientially significantly larger than 256
//  // to accomplish this two sucessive bytes are sent to repersent one number. to deal with positive and negative numbers, the coordinate is split into
//  // two bytes, leasat significant 8 bits are the second byte and the most significant 7 bits are in the first byte sent. if the coordinate is negaitve the
//  // remaining bit is set 0 and 1 otherwise.
//
//  // NOTE: current implementation will overflow if an out of bounds coordinate is presented (+/-32738 is usable)
//  //       I cant see a reason this would be an issue so not fixing it for now
//
//  if (abs(x_pos) > 32738 || abs(y_pos) > 32738) //print warning that coordinate will be wrong
//    Serial.println("WARNING: failed to send correct coordinate, out of bounds, overflow likely to occur");
//
//  if (x_pos > 0) {
//    x_pos_MSB = x_pos / 256; //take the multiples of 256 and set as MS byte
//    x_pos_MSB = x_pos_MSB + 128; //greater than zero, set MSB to 1
//    x_pos_LSB = x_pos % 256; //take the modulo to get the LS byte
//  }
//  else {
//    x_pos_MSB = x_pos / 256; //take the multiples 256 and set as MS byte
//    x_pos_LSB = x_pos % 256; //take the modulo to get the LS byte
//  }
//
//  if (y_pos > 0) {
//    y_pos_MSB = y_pos / 256; //take the multiples of 256 and set as MS byte
//    y_pos_MSB = y_pos_MSB + 128; //greater than zero, set MSB to 1
//    y_pos_LSB = y_pos % 256; //take the modulo to get the LS byte
//  }
//  else {
//    y_pos_MSB = y_pos / 256; //take the multiples 256 and set as MS byte
//    y_pos_LSB = y_pos % 256; //take the modulo to get the LS byte
//  }
//
//
//  frame.frame_buffer[4] = x_pos_MSB;   //write new values to frame
//  frame.frame_buffer[5] = x_pos_LSB;
//  frame.frame_buffer[6] = y_pos_MSB;
//  frame.frame_buffer[7] = y_pos_LSB;
//
//  return (0);
//}



//int send_disp_string_frame(int address) {   //function to send strings to display on screen
//
//  // function calculates the number of frames required to send the string, then loops,
//  // generates a frame hader and fills up to 27 bytes of the string and calculates the checksum
//  // it also calls the send frame function to send it on to the specified address when frame complete
//  //noInterrupts();
//  int type = 1;
//  // if(debug){
//  // Sprint("Sending displaying string to address ");
//  // Sprintln(address);
//  // }
//
//  frame.num_frames = 1 + (strlen(text_str) / 27); //send this many frames
//  frame.this_frame = 1;
//
//  do {    //loop to send multiple frames if string is long
//
//    if (frame.num_frames != frame.this_frame)
//      frame.frame_buffer[0]  = 32;  //if there are more than one frame to send, this
//
//    else
//      frame.frame_buffer[0]  = strlen(text_str) - ((frame.num_frames - 1) * 27) + 5; //remaining frame is string length-offset+5 bytes overhead
//
//
//    frame.frame_buffer[1] = (byte) type;
//    frame.frame_buffer[2] = (byte) frame.num_frames;
//    frame.frame_buffer[3] = (byte) frame.this_frame;
//    frame.checksum = frame.frame_buffer[0] + frame.frame_buffer[1] + frame.frame_buffer[2] + frame.frame_buffer[3] ;
//
//    pack_disp_string_frame(type, frame.this_frame);//function to pack the frame with which ever data is relevant
//    frame.frame_buffer[frame.frame_buffer[0] - 1] = (byte)256 - frame.checksum;
//    wire_write_frame(address);
//
//    frame.this_frame++;   //increment this_frame after sending, will prepare for next loop or break
//    delayMicroseconds(100000);       //small delay, want reciever to read through its buffer, otherwise the buffer may overload when we send next frame
//
//  } while (frame.this_frame <= frame.num_frames);
//  //interrupts();
//  return (0);
//}

//int pack_disp_string_frame(int frame_type, int frame_offset) {   //function to pack a frame of text to display
//
//  // function to pack a frame based on a given offset (ie this frames number)
//  // maybe generalise later to accept calls from multiple frame building methods
//
//  frame_offset = ((frame_offset - 1) * 27); //if this frame is 1 offset in data set is 0, if 2 offset 27, etc
//
//  if (frame_type == 1) { //send str variable
//    for (int i = 0; i < strlen(text_str) - frame_offset; i++) {
//      frame.frame_buffer[i + 4] = (byte)text_str[frame_offset + i];
//      frame.checksum = frame.checksum + (byte)frame.frame_buffer[i + 4];
//      if (i == 27) break;     //copy string untilend or 28 bytes copied
//    }
//  }
//
//  // else{
//  //   if (debug)
//  //     Sprintln(F("Error from pack_disp_string_frame: can only pack type 1 frames"));
//  // }
//
//  return (0);
//}


#endif // Interupts_H
