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
//Coms_i2c       coms_i2c;
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
HostNativeUSB  nativeUsb;


Timers timers; //timers struct

void due_setup() {
#ifndef DUE_ENABLED    //stop code imidiately if due disabled
  while (1) {}
#endif

  host.init_serial();

  card.init_sd_cards();

  encoder.init_encoder();
  encoder.init_button();
  menu.init_menu_tree();
  
  light_sensor.init_LDR();
  current_meter.init_current_meter();
  graphics.init_cursor();
  fans.init_fans();
  fans.init_temp_sensors();
  led_strip.init_led_strip();
  nativeUsb.init_native_usb();
  coms_serial.init_serial();

}


void due_loop() {
  while (1) {
    // update screen brightness based on ldr and current meter readings
    current_meter.get_readings();
    light_sensor.get_readings();
    graphics.update_brightness();

    // get temperature readings and update fan speeds
    fans.update_temperatures();
    fans.set_fan_speed();

    // update led strip brightness frequency of interrupt
    led_strip.led_strip_set_freq();

    // check the encoder
    encoder.handle_interupts();

    // push menu updates to screens 
    menu.display_menu();

    //check for user serial input and print requested data
    host.check_serial();
    host.print_messages();
    
    // check if sd card mounted and log data if possible
    card.check_for_sd_card();
    //card.update_data_log();


    //TO DO:
    // internet.check_connection();
    //graphics.get_next_string_config_profile();
    
  }

}

#endif // Due_CPP

