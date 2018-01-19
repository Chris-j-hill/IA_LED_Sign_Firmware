#ifndef Coms_serial_CPP
#define Coms_serial_CPP 

#include "Coms_Serial.h"
#include "Coms.h"
//#include "Due.h"
#include "Due_Pins.h"
#include "Graphics.h"

extern struct Frame text_frame;
extern struct Frame sensor_data_frame;
extern struct Frame menu_frame;
extern struct Frame pos_frame;
extern struct Text_cursor text_cursor;
extern struct Text text;

extern char text_str[150];
extern const byte to_mega_prefix_array[] = {10, 11, 20, 21, 22, 30, 31, 40, 50, 60, 61, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180};  

extern byte time_since_last_sent_text_frame;

#ifdef USE_SERIAL_TO_MEGAS
bool enable_serial = true;   
#else
bool enable_serial = false;   
#endif

bool serial_enabled = false;


serial_tc5_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);
auto& Serial_1=serial_tc5;
serial_tc6_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);
auto& Serial_2=serial_tc6;
serial_tc7_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);
auto& Serial_3=serial_tc7;
serial_tc8_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);
auto& Serial_4=serial_tc8;



//methods for Coms_Serial class

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

  //#error("function not implemented")
}

int Coms_Serial::send_all_text_frames() {   // send the text frame to all megas

  if (millis()>time_since_last_sent_text_frame+TEXT_TRANSMIT_PERIOD){
    time_since_last_sent_text_frame = millis();
    for (int alpha = 1; alpha <= NUM_SCREENS; alpha++) {
     
        Sprint(F("Sending text frame to address "));
        Sprintln(alpha);
      
      int fail = send_text_frame(alpha);
      if (fail != 0) {
        Sprint(F("Failed to send string to mega"));
        Sprintln(alpha);
        return (-1);
      }
    }
  }
  return (0);
}

int Coms_Serial::send_text_frame(int address) {   //function to send strings to display on screen

  // function calculates the number of frames required to send the string, then loops,
  // generates a frame hader and fills up to 27 bytes of the string and calculates the checksum
  // it also calls the send frame function to send it on to the specified address when frame complete
  
  

  text_cursor.x_min = -text.text_width*strlen(text_str)*2; // set this based on size of string being sent, will update if string changed

  text_frame.num_frames = 1 + (strlen(text_str) / (FRAME_DATA_LENGTH)); //send this many frames
  text_frame.this_frame = 1;

  do {    //loop to send multiple frames if string is long

    if (text_frame.num_frames != text_frame.this_frame)
      text_frame.frame_buffer[0]  = MEGA_SERIAL_BUFFER_LENGTH;  //if there are more than one frame left to send, this frame is max size

    else
      text_frame.frame_buffer[0]  = strlen(text_str) - ((text_frame.num_frames - 1) * (FRAME_DATA_LENGTH)) + (HEADER_LENGTH + TRAILER_LENGTH); //remaining frame is string length-text offset+5 bytes overhead


    text_frame.frame_buffer[1] = (byte) text_frame.frame_type;
    text_frame.frame_buffer[2] = (byte) text_frame.num_frames;
    text_frame.frame_buffer[3] = (byte) text_frame.this_frame;
    text_frame.checksum = text_frame.frame_buffer[0] + text_frame.frame_buffer[1] + text_frame.frame_buffer[2] + text_frame.frame_buffer[3] ;

    pack_disp_string_frame(text_frame.frame_type, text_frame.this_frame);//function to pack the frame with which ever data is relevant
    text_frame.frame_buffer[text_frame.frame_buffer[0] - 1] = (byte)256 - text_frame.checksum;

    write_text_frame();
    
    text_frame.this_frame++;   //increment this_frame after sending, will prepare for next loop or break
    delayMicroseconds(10000);       //small delay, want reciever to read through its buffer, otherwise the buffer may overload when we send next frame

  } while (text_frame.this_frame <= text_frame.num_frames);
  
  return (0);
}


int Coms_Serial::Serial_write_frame(int address) {   //function to actually send the frame to given address

  
  if (!mega_enabled[address - 1]) {
    
      Sprintln(F("Mega disabled, did not attempt transmission"));
      return (-1);
  }

  if (address == 1) {
    for (int i = 0; i < text_frame.frame_buffer[0]; i++) {
      Serial_1.write(text_frame.frame_buffer[i]);
 
    }
  }
  else if (address == 2) {
    for (int i = 0; i < text_frame.frame_buffer[0]; i++) {
      Serial_2.write(text_frame.frame_buffer[i]);
 
    }
  }
  else if (address == 3) {
    for (int i = 0; i < text_frame.frame_buffer[0]; i++) {
      Serial_3.write(text_frame.frame_buffer[i]);

    }
  }
  else  if (address == 4) {
    for (int i = 0; i < text_frame.frame_buffer[0]; i++) {
      Serial_4.write(text_frame.frame_buffer[i]);

    }
  }
  else {
    Sprintln(F("Address invalid"));
    return -1;
  }
  
  
  //clear frame from last iteration
  for (int beta = 0; beta < MEGA_SERIAL_BUFFER_LENGTH; beta++) {
    text_frame.frame_buffer[beta] = 0;
  }

  return (0);
}



int Coms_Serial::send_all_calibration_data(int address) {     //function to send all data

  //function to send all the sensor data. loop through all sensor values
  
  byte frameNum = 1;
  byte numFrames = ((sizeof(to_mega_prefix_array) * 2) / 26) + 1;
  int offset = 0;
  bool frame_to_be_sent = false;

  sensor_data_frame.frame_buffer[1] = sensor_data_frame.frame_type;        //set frame starting bytes
  sensor_data_frame.frame_buffer[2] = numFrames;
  sensor_data_frame.frame_buffer[3] = frameNum;

  for (int alpha = 0; alpha < sizeof(to_mega_prefix_array) + 1; alpha++) {

    if (alpha == sizeof(to_mega_prefix_array)) { //if last byte
      frame_to_be_sent = send_specific_calibration_data(to_mega_prefix_array[alpha],  address, false,  offset);   //indicate this is the last element

    }
    else
      frame_to_be_sent = send_specific_calibration_data(to_mega_prefix_array[alpha],  address, true, offset);    //pack byte and dont send

    if (!frame_to_be_sent)  //if the frame was sent (function returns 1), reset offset otherwise increment
      offset++;

    else if (frame_to_be_sent) {
      frameNum++;     //increment the frame number
      offset = 0;     //reset offset for new frame
      write_sensor_data_frame();
      sensor_data_frame.frame_buffer[1] = sensor_data_frame.frame_type;        //set frame starting bytes
      sensor_data_frame.frame_buffer[2] = numFrames;
      sensor_data_frame.frame_buffer[3] = frameNum;
    }

  }
  
  return (0);

}

void Coms_Serial::write_frame(int address){
#if defined(USE_SERIAL_TO_MEGAS)
Serial_write_frame(address);
#else
#error "I2C coms protocol not defined, cant send frame"
#endif
}


#endif //USE_SERIAL_TO_MEGAS
#endif //Sign_coms_serial_CPP
