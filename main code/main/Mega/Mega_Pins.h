

#ifndef PINS_H
#define PINS_H      
#include "Arduino.h"

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

    
//    NOTE: Do not assign anything to digital pins 22 or 23 as these are used by the matrix. the colour assignments require the whole 8 bit port not just the six pins

    #define SERIAL_RX_PIN 19
    #define SERIAL_TX_PIN SERIAL_RX_PIN-1 



#endif // PINS_H
