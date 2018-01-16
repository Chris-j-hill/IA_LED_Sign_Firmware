#ifndef Sign_coms_serial_CPP
#define Sign_coms_serial_CPP 

#include "Coms_Serial.h"
#include "Coms.h"
#include "Due.h"



//methods for led_strip class

#ifdef USE_SERIAL_TO_MEGAS

int Coms_Serial::init_software_serial_to_megas() {      // initialise serial at set baud rate speed of 115200

  //init four soft serial objects on due
  Serial_1.begin(
    RX_PIN_1,
    TX_PIN_1,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
  Serial_2.begin(
    RX_PIN_2,
    TX_PIN_2,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
  Serial_3.begin(
    RX_PIN_3,
    TX_PIN_3,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
  Serial_4.begin(
    RX_PIN_4,
    TX_PIN_4,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
  return 0;

}

int Coms_Serial::init_software_serial_to_megas(int speed) {   // initialise serial at specified speed, must be standardised speed 115200 or below, otherwise error thrown
  //ensure the speed is a standard baud rate
  if (speed != 300 && speed != 600 && speed != 1200 && speed != 2400 && speed != 4800 && speed != 14400 && speed != 9600 && speed != 14400 && speed != 19200 && speed != 28800 && speed != 38400 && speed != 57600 && speed != 115200)
    return (-2);
  //if (debug){      //if debug mode, init serial, if debug false dont bether with this
  int alpha = millis();

    Serial_1.begin(
    RX_PIN_1,
    TX_PIN_1,
    speed,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
  Serial_2.begin(
    RX_PIN_2,
    TX_PIN_2,
    speed,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
  Serial_3.begin(
    RX_PIN_3,
    TX_PIN_3,
    speed,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
  Serial_4.begin(
    RX_PIN_4,
    TX_PIN_4,
    speed,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
  return 0;

}

int Coms_Serial::get_serial() {                   //function to interpret serial data recieved without a user prompt

  // function to interpret any serial data recieved during the main loop (ie without a user prompt)
  // the function should work on a key word basis whereby any recieved data should be of the form 'keyword: data'
  // where 'keyword' defines the type of data recieved, such as displayable string, instruction, sensor get request etc
  // and 'data' is any relevant string for the specified keyword. The function should return an error message if the keyword isnt defined.


  // an example would be: 'disp_string: display this string'
  // this is defined by the disp_string keyword meaning the associated data should be pushed to the screen, in which case the data is the string to be pushed

  #error("function not implemented")
}

int coms::Serial_write_frame(int address) {   //function to actually send the frame to given address

  
  if (!mega_enabled[address - 1]) {
    
      Sprintln(F("Mega disabled, did not attempt transmission"));
      return (-1);
  }

  if (address == 1) {
    for (int i = 0; i < frame.frame_buffer[0]; i++) {
      Serial_1.write(frame.frame_buffer[i]);
 
    }
  }
  else if (address == 2) {
    for (int i = 0; i < frame.frame_buffer[0]; i++) {
      Serial_2.write(frame.frame_buffer[i]);
 
    }
  }
  else if (address == 3) {
    for (int i = 0; i < frame.frame_buffer[0]; i++) {
      Serial_3.write(frame.frame_buffer[i]);

    }
  }
  else  if (address == 4) {
    for (int i = 0; i < frame.frame_buffer[0]; i++) {
      Serial_4.write(frame.frame_buffer[i]);

    }
  }
  else {
    Sprintln(F("Address invalid"));
    return -1;
  }
  
  
  //clear frame from last iteration
  for (int beta = 0; beta < MEGA_SERIAL_BUFFER_LENGTH; beta++) {
    frame.frame_buffer[beta] = 0;
  }

  return (0);
}

#endif //USE_SERIAL_TO_MEGAS
#endif //Sign_coms_serial_CPP
