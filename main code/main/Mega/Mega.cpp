#ifndef MEGA_CPP
#define MEGA_CPP

#include "Mega.h"
#include "Mega_Pins.h"                     //list of pins used in each board
#include "Graphics.h"


extern struct Screen_Struct screen_parameters;

//mega_class mega;
//RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE1, OE2, false, 64, screen_mode);
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE2, false, 64);
Menu menu;
Graphics graphics;
  
void mega_setup(){
// configure_address();
//  coms_serial.init_serial();
//  graphics.init_screen();

  
  }
  
  
void mega_loop(){
//   coms_serial.check_serial();  //deal with any serial recieved reently and send nack if needed
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
    screen_parameters.node_address = 0;

  if (address_bit1 == LOW && address_bit2 == HIGH)
    screen_parameters.node_address = 1;

  if (address_bit1 == HIGH && address_bit2 == LOW)
    screen_parameters.node_address = 2;

  if (address_bit1 == HIGH && address_bit2 == HIGH)
    screen_parameters.node_address = 3;
}
 
 #endif	//MEGA_CPP
