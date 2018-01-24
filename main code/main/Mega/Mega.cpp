#ifndef MEGA_CPP
#define MEGA_CPP

#include "libraries.h"                //file containing all 3rd party libraries used
#include "Pins.h"                     //list of pins used in each board
#include "function_declarations.h"    //any functions that need prototypes
#include "Global_variables.h"         //all global variables defined here
#include "due_structs.h"              //structs for a variety of aspects of the sign (fans, encoder, text etc)

#include "Interrupts.h"               //interrupt functions, arduino doesnt like them in classes so created seperately 

#include "base_class.h"               //classes
#include "due_class.h"
#include "mega_class.h"

mega_class mega;
//RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE1, OE2, false, 64, screen_mode);
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE2, false, 64);

  
void mega_setup(){
// configure_address();
//  coms_serial.init_serial();
//  graphics.init_screen();

  
  }
  
  
void mega_loop(){
//   coms_serial.check_serial();
//   graphics.update_menus();    // specifically display the menus if they are needed
//   graphics.update_display();  // fill frame if something changed, derive area to fill based on menus
  }



void configure_address(){
  
  bool address_bit1;
  bool address_bit2;
  
  pinMode(HARDWARE_ADDRESS_1, INPUT);
  pinMode(HARDWARE_ADDRESS_2, INPUT);

  address_bit1 = digitalRead(HARDWARE_ADDRESS_1);
  address_bit2 = digitalRead(HARDWARE_ADDRESS_2);

  if (address_bit1 == LOW && address_bit2 == LOW)
    node_address = 0;

  if (address_bit1 == LOW && address_bit2 == HIGH)
    node_address = 1;

  if (address_bit1 == HIGH && address_bit2 == LOW)
    node_address = 2;

  if (address_bit1 == HIGH && address_bit2 == HIGH)
    node_address = 3;
}
 }
 #endif	//MEGA_CPP
