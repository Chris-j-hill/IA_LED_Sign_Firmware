#ifndef Due_H
#define Due_H

#include "Due_Pins.h"                     //list of pins used in each board
#include "Config_Local.h"
#include "function_declarations.h"    //any functions that need prototypes
#include "Due_Libraries.h"                //file containing all class files and 3rd party libraries used

// class constructors      
Coms           coms;
Coms_Serial    coms_serial;
Coms_i2c       coms_i2c;
Card           card;
Menu           menu;
Internet       internet;
Fans           fans;
Encoder        encoder;
Led_Strip      led_strip;
Current_Meter  current_meter;
Light_Sensor   light_sensor;

Timers timers;
/*    --------------------------------------------------------------------------------
 *     timer struct variables, fjnd somewhere better for this
 *    --------------------------------------------------------------------------------
 */

//bool debug = DEBUG;



void due_setup() {
//  if (!DUE_ENABLED){    //stop code imidiately if due disabled
//    while(1){}
//  }
//  
//  int fail = due.init_due();   //init everything enabled in global variables file
//  if (fail!=0)
//  due.stop();
//  Serial.begin(115200);
//  delay(200);
//  Serial.println();
//  Serial.println(F("Init sucessfull"));
//
//  //housekeeping, calc and set any variables
//  
//  //Serial.println(F("Calc the delay"));
//  //due.calc_delay();
//  
//  
//  Serial.println(F("due ready, init megas"));
//  //due.startup_handshake();
//  //Serial.println(F("megas ready"));
//
//  //due.stop();
//
//  //i2c ready, send string
//  due.send_all_text();    //method to send the text to all the megas
//  Serial.println(F("sent frames"));
//  
//  pinMode(13, OUTPUT);
//  Serial.println(F("Attaching the timer interrupts"));
//  //attach timer based interrupts
//  
//  
//
//  attach_timer_interrupts();
//  Serial.println(F("Attach interrupts complete"));
//  delay(100);
//  due.print_frame();
  
}


void due_loop() {
// //do these periodically based on config
// cards.check_for_sd_card_inserted();   
// internet.check_connection();       
// fans.get_temperature();            
// fans.set_fan_speed();
// disp.update_brightness();
// encoder.update_encoder_postion();
// encoder.pressed();
// update_display();  //push additional data to screens as required
//
// //do this based speed variable
// update_text_location(); //write location variable to be sent by interrupt

 
}

#endif // Due_H

