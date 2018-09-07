

#ifndef PINS_H
#define PINS_H      
#include "Arduino.h"

    #define due_ready_pin       42
    #define mega_ready_pin      43
    #define HARDWARE_ADDRESS_1  44
    #define HARDWARE_ADDRESS_2  45

    // Colours D24-D29  -> Defined in matrix library
    #define OE1   12
    #define OE2   13
    #define LAT   10
    #define CLK   11
    #define A     A0
    #define B     A1
    #define C     A2
    #define D     A3

    #define DELAY_FEEDBACK_X_PIN   46
    #define DELAY_FEEDBACK_Y_PIN   47

    #define serial_handshake_pin  17

    
//    NOTE: Do not assign anything to digital pins 22 or 23 as these are used by the matrix. the colour assignments require the whole 8 bit port not just the six pins


#endif // PINS_H
