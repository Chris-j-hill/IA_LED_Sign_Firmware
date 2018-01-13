

#include "libraries.h"                //file containing all 3rd party libraries used
#include "Pins.h"                     //list of pins used in each board
#include "function_declarations.h"    //any functions that need prototypes
#include "Global_variables.h"         //all global variables defined here
#include "due_structs.h"              //structs for a variety of aspects of the sign (fans, encoder, text etc)

#include "Interrupts.h"               //interrupt functions, arduino doesnt like them in classes so created seperately 

#include "base_class.h"               //classes
#include "due_class.h"
#include "mega_class.h"


due_class due; 	// class constructor      

void due_setup() {
  if (!due_enabled){    //stop code imidiately if due disabled
    while(1){}
  }
  
  int fail = due.init_due();   //init everything enabled in global variables file
  if (fail!=0)
  due.stop();
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println(F("Init sucessfull"));

  //housekeeping, calc and set any variables
  
  //Serial.println(F("Calc the delay"));
  //due.calc_delay();
  
  
  Serial.println(F("due ready, init megas"));
  //due.startup_handshake();
  //Serial.println(F("megas ready"));

  //due.stop();

  //i2c ready, send string
  due.send_all_text();    //method to send the text to all the megas
  Serial.println(F("sent frames"));
  
  pinMode(13, OUTPUT);
  Serial.println(F("Attaching the timer interrupts"));
  //attach timer based interrupts
  
  

  attach_timer_interrupts();
  Serial.println(F("Attach interrupts complete"));
  delay(100);
  due.print_frame();
  
}


void due_loop() {
  text_cursor.x = 0;
  text_cursor.y = 0; 
  int mega_last_sent_time = millis();
  while (1) {
  due.send_all_pos();   //timer based function to send pos

  due.send_all_text();    //send text periodically

  
    //Serial.println(F("Loop"));
    //  delay(1000);
    //due.send_disp_string_frame(1);
    //due.send_disp_string_frame(4);
     //x_pos_dir = 1;
     if (text_cursor.x <= -text.text_width*strlen(text_str)*2)
     text_cursor.x=0;
//     y_pos++;
//     Serial.println(text_str);
//    Serial.println(x_pos);
//    Serial.println(y_pos);
//    send_text_frame_123 = true;
//    Serial.println();

  
    
 
  }

}


  
