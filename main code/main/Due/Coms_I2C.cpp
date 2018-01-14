#ifndef Coms_I2C_CPP
#define Coms_I2C_CPP 

#include "Coms.h"
#include "Coms_I2C.h"
#include "Due.h"



//methods for i2c coms class
#ifdef USE_I2C_TO_MEGAS
int coms::init_i2c() { //if init does not specify address call with address 5(safe value), if due, value not used

  init_i2c_due();   //attach the i2c to wire1
  //Serial.println(F("Pause for megas to initialise"));
  //delay(500);
  //init_i2c(0);      //check all i2c devices activated

}

int coms::init_i2c(int address) {   //initialise i2c by calling specify routines for this chipset


  int alpha = i2c_scanner();   //test all addresses
  if (alpha != 0)
    Serial.println(F("Failed to detect 4 megas on i2c"));
  //alpha = init_i2c_due();

  if (alpha < 0) {     //error if less than 0
    
      Sprintln("error setting up i2c, stopping program");
      Sprintln("Error code: " + alpha);
    
    while (1) {}    //stop code
  }

}

int coms::init_i2c_due() {  //i2c init for the due specifically

  //ensure the device running this is a due

  Sprintln(F("Error, runinng due i2c init but using mega chipset"));
  return (-1);

}

int coms::i2c_scanner() {           // scan all addresses and make sure decive responds

  // based on the i2c scanner methodology send dummy frame to mega
  // check endTransmission() return value, if non zero there is an error
  if (test_i2c) {
    int fail;
    for (int i = 1; i <= num_screens; i++) {
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

int coms::i2c_scanner(int address) { // test specific address

  Wire1.beginTransmission(address);
  //for(int i=0; i<strlen(dummy_frame);i++){
  //Wire1.write(dummy_frame[i]);
  //}
  int fail = Wire1.endTransmission();
  return fail;

}





#endif //USE_I2C_TO_MEGAS

#endif //Coms_I2C_CPP
