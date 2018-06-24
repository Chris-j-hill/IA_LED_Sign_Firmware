
#ifndef Host_CPP
#define Host_CPP

#include "Arduino.h"
#include "Host.h"
#include "Config_local.h"

void Host::init_serial(){
  Serial.begin(HOST_SERIAL_SPEED);
  }





#endif // Host_CPP
