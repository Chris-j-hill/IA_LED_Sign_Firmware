
#ifndef Internet_CPP
#define Internet_CPP
#include "Arduino.h"

#include "Internet.h"
//#include "Due.h"
#include "SD_Cards.h"



#ifdef ENABLE_ETHERNET
bool enable_ethernet = true;
#else
bool enable_ethernet = false;
#endif

#ifdef ENABLE_WIFI
bool enable_wifi = true;
#else
bool enable_wifi = false;
#endif

bool ethernet_enabled = false;
bool wifi_enabled = false;

bool ethernet_connected = true;   
bool wifi_connected = true;


extern bool sd_cards_enabled;


//methods for internet_class

int Internet::init_ethernet() {}     //not implented

int Internet::init_wifi() {       //not implented

  if (sd_cards_enabled) {        //make sure the sd cards have been enabled, otherwise due cant have access to network info

  }

  else {
      Sprintln(F("From init wifi: Sd cards not initialised, dont have network info"));
  }
}

#endif // Internet_CPP
