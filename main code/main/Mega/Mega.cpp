#ifndef MEGA_CPP
#define MEGA_CPP

#include "Mega.h"
#include "Mega_Pins.h"                     //list of pins used in each board
#include "Graphics.h"
#include "Host.h"
#include "Coms_Serial.h"


#if SERIAL_RX_BUFFER_SIZE >64 || SERIAL_TX_BUFFER_SIZE >16
#error "serial buf size error"
/* serial buffer size is defined under hardware\arduino\avr\cores\arduino\HardwareSerial.h
   there are 2 buffers (rx and tx) allocated in ram per serial port initialised (we use two ports)
   so an excssively large buffer size will be a problem here as were very tight on ram. change
   the value to 32 for rx (size of data frame) and 16 for tx (transmitting less data usually)
*/
#endif

extern struct Screen_Struct screen_parameters;

//mega_class mega;
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE1, OE2, true, 64, DEFAULT_SCREEN_MODE);
//RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE2, false, 64);
Menu menu;
Graphics graphics;
Coms_Serial coms_serial;
Host host;


extern struct Text_Struct text_parameters[MAX_NUM_OF_TEXT_OBJECTS];

void mega_setup() {

  configure_address();
  host.init_serial();    //enable printing to monitor
  Serial.print(F("address: "));
  Serial.println(screen_parameters.node_address);
  coms_serial.init_serial(); //enable coms port to due
  graphics.init_matrix();
  graphics.attach_pos_ISR();  //pos isr to interpolate cursor positions between frames based on timer
  Serial.println(F("done init"));


  text_parameters[0].object_used = true;
  text_parameters[0].text_size = 1;

}


void mega_loop() {
  // byte i=0;
  //  uint32_t start_time = millis();
  //  uint32_t delay_time = 500;
  while (1) {
    //    if (millis() > delay_time + start_time){
    //      coms_serial.request_frame_retransmission(TEXT_FRAME_TYPE, 1, 0);
    //      Serial.println("request");
    //      start_time = millis();
    //  }
    coms_serial.read_buffer();  //deal with any serial recieved reently and send nack if needed
    //graphics.update_display();  // fill frame if something changed, derive area to fill based on menus
    //graphics.interpolate_pos(); //this is reasonably slow so only set flag in interrupt and do heavy lifting at time to suit
    //host.check_serial();
    //host.print_messages();
    //    i++;
    //    if (i==0)
    //    Serial.println(F("loop"));
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
