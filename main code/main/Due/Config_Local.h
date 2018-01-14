
#ifndef Configuration_H
#ifndef Config_Local_h
#define Config_Local_h
#include "Arduino.h"

#define USE_SERIAL_TO_MEGAS
#define USE_I2C_TO_MEGAS
#define ENABLE_ERROR_CHECKING

#define TEXT_TRANSMIT_PERIOD 2000

#define single_matrix_width = 64;
#define single_matrix_height = 32;
#define num_screens = 4;    //per side
#define total_width = single_matrix_width * num_screens;

#define HEADER_LENGTH = 3;
#define TRAILER_LENGTH = 2;
#define DATA_IDENTIFIER_BYTE = 2;
#define FRAME_LENGTH_BYTE = 1;

//byte frame_type;             // frame type defines what is to be done with info, ie where string should go or other operation
//byte frame_length = 150;     // length of current frame
#define max_frame_size = 150;    // max frame size 150, ie max length of a tweet plus some frame info
// byte frame[150];             //frame array



#define COMS_SPEED 38400         //speed of coms between due and megas when using serial

#define SOFT_UART_BIT_RATE COMS_SPEED // 57600 38400 1200 19200 9600 115200 300
#define RX_BUF_LENGTH 256 // software serial port's reception buffer length 
#define TX_BUF_LENGTH 256 // software serial port's transmision buffer length




#define DEBUG       //comment this out to dissable debugging

#ifdef DEBUG
#define Sprintln(a) (Serial.println(a))   // use to print debug errors
#define Sprint(a) (Serial.print(a))

#else

#define Sprintln(a)    // prints nothing if debug not defined
#define Sprint(a)
#endif
#endif // Config_Local_h
#endif // Configuration_H
