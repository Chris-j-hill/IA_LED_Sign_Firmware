#ifndef MEGA_CPP
#define MEGA_CPP

#include "Mega.h"
#include "Mega_Pins.h"                     //list of pins used in each board
#include "Graphics.h"
#include "Host.h"
#include "Coms_Serial.h"

extern struct Screen_Struct screen_parameters;

//mega_class mega;
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE1, OE2, false, 64, DEFAULT_SCREEN_MODE);
//RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE2, false, 64);
Menu menu;
Graphics graphics;
Coms_Serial coms_serial;
Host host;


void mega_setup() {
  
  configure_address();
  host.init_serial();    //enable printing to monitor
  Serial.print(F("address: "));
  Serial.println(screen_parameters.node_address);
  coms_serial.init_serial(); //enable coms port to due
  graphics.init_matrix();
  graphics.attach_pos_ISR();  //pos isr to interpolate cursor positions between frames based on timer
  Serial.println(F("done init"));

}


void mega_loop() {
  //byte i=0;
  while (1) {
    coms_serial.read_buffer();  //deal with any serial recieved reently and send nack if needed
    graphics.update_display();  // fill frame if something changed, derive area to fill based on menus
    graphics.interpolate_pos(); //this is reasonably slow so only set flag in interrupt and do heavy lifting at time to suit
  }
}



void configure_address() {

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
