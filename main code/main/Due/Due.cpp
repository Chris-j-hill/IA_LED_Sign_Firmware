#ifndef Due_CPP
#define Due_CPP

#include "Due.h"
#include "Coms.h"
#include "Arduino.h"
#include "function_declarations.h"
#include "Due_Pins.h"
#include "Graphics.h"
#include "Current_Control.h"
#include "Fans.h"
#include "Internet.h"
#include "SD_Cards.h"
#include "Led_Strip.h"
#include "Encoder.h"
#include "Host.h"
// class constructors

Coms_Serial    coms_serial;
Coms_i2c       coms_i2c;
Card           card;
Menu           menu;
Internet       internet;
Fans           fans;
Encoder        encoder;
Led_Strip      led_strip;
Current_Meter  current_meter;
Light_Sensor   light_sensor;
Graphics       graphics;
Host           host;
Timers timers;


void due_setup() {
#ifndef DUE_ENABLED    //stop code imidiately if due disabled
  while (1) {}
#endif
  host.init_serial();
  Serial.println("host init done");
  fans.init_fans();
  Serial.println("fans init done");

  fans.init_temp_sensors();
  Serial.println("temp sensor init done");


}


void due_loop() {
  // //do these periodically based on config
  // cards.check_for_sd_card_inserted();
  // internet.check_connection();
  // fans.get_temperature();
  fans.set_fan_speed();
  // graphics.update_brightness();
  // encoder.update_encoder_postion();
  // encoder.pressed();
  // update_display();  //push additional data to screens as required
  //
  // //do this based speed variable
  // update_text_location(); //write location variable to be sent by interrupt


}

#endif // Due_CPP

