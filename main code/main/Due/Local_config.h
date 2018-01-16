
#ifndef Configuration_H
#ifndef Local_config_H
#define Local_config_H      
#include "Arduino.h"

#define USE_SERIAL_TO_MEGAS
#define USE_I2C_TO_MEGAS




#define DEBUG       //comment this out to dissable debugging

#ifdef DEBUG
  #define Sprintln(a) (Serial.println(a))   // use to print debug errors
  #define Sprint(a) (Serial.print(a))

#else

  #define Sprintln(a)    // prints nothing if debug not defined
  #define Sprint(a) 
#endif
#endif // Local_config_H
#endif // Configuration_H
