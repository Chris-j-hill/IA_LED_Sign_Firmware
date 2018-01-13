

#ifndef PINS_H
#define PINS_H      //define to stop double declarations
#include "Arduino.h"

    //      mega pins

    #ifdef __AVR__

        #define due_ready_pin       42
        #define mega_ready_pin      43
        #define HAREWARE_ADDRESS_1  44
        #define HAREWARE_ADDRESS_2  45

        // Colours D24-D29  -> Defined in matrix library
        #define OE1  12
        #define OE2  13
        #define LAT 10
        #define CLK 11
        #define A   A0
        #define B   A1
        #define C   A2
        #define D   A3

        

    #endif // __AVR__




#endif // PINS_H
