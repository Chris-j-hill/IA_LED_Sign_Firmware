
#ifndef Internet_CPP
#define Internet_CPP
#include "Arduino.h"

#include "Internet.h"
#include "Due.h"



//methods for internet_class

int internet::init_ethernet() {}     //not implented

int internet::init_wifi() {       //not implented

  if (sd_cards_enabled) {        //make sure the sd cards have been enabled, otherwise due cant have access to network info

  }

  else {
    if (debug)
      Sprintln(F("From init wifi: Sd cards not initialised, dont have network info"));
  }
}

#endif // Internet_CPP
