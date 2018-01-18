#ifndef Coms_I2C_CPP
#define Coms_I2C_CPP 

//#include "Coms.h"
#include "Coms_I2C.h"
//#include "Due.h"
#include <Wire.h>


extern struct Frame frame;


#ifdef USE_I2C_TO_MEGAS
bool enable_i2c = true;    
#else
bool enable_i2c = false;
#endif


bool i2c_enabled = false;

#ifdef USE_I2C_TO_MEGAS       //only run test if i2c is enabled and run_i2c scanner_test is defined 
#ifdef RUN_I2C_SCANNER_TEST
bool test_i2c = true;
#else
bool test_i2c = false;
#endif
#else
bool test_i2c = false;
#endif





//methods for i2c coms class
#ifdef USE_I2C_TO_MEGAS
int Coms_i2c::init_i2c() { //if init does not specify address call with address 5(safe value), if due, value not used

  init_i2c_due();   //attach the i2c to wire1

}

int Coms_i2c::init_i2c(int address) {   //initialise i2c by calling specify routines for this chipset

//
//  int alpha = i2c_scanner();   //test all addresses
//  if (alpha != 0)
//    Serial.println(F("Failed to detect 4 megas on i2c"));
//  //alpha = init_i2c_due();
//
//  if (alpha < 0) {     //error if less than 0
//    
//      Sprintln("error setting up i2c, stopping program");
//      Sprintln("Error code: " + alpha);
//    
//    while (1) {}    //stop code
//  }

}

int Coms_i2c::init_i2c_due() {  //i2c init for the due specifically

}

int Coms_i2c::i2c_scanner() {           // scan all addresses and make sure decive responds

  // based on the i2c scanner methodology send dummy frame to mega
  // check endTransmission() return value, if non zero there is an error
  if (test_i2c) {
    int fail;
    for (int i = 1; i <= NUM_SCREENS; i++) {
      fail = i2c_scanner(i);
      if (fail != 0) {
          Sprint(F("Error connecting to i2c device:"));
          Sprintln(i);
        
        return (-1);
      }

    }
   
    Sprintln(F("i2c return success"));
    return (0);
  }
  else
    return (0);

}

int Coms_i2c::i2c_scanner(int address) { // test specific address

  Wire1.beginTransmission(address);
  //for(int i=0; i<strlen(dummy_frame);i++){
  //Wire1.write(dummy_frame[i]);
  //}
  int fail = Wire1.endTransmission();
  return fail;

}





#endif //USE_I2C_TO_MEGAS

#endif //Coms_I2C_CPP
