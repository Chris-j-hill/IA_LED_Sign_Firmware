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
HostNativeUSB  nativeUsb;
Timers timers;


void due_setup() {
#ifndef DUE_ENABLED    //stop code imidiately if due disabled
  while (1) {}
#endif

  host.init_serial();
  //  host.check_serial();
  //  host.print_messages();
  fans.init_fans();
  fans.init_temp_sensors();
  led_strip.init_led_strip();
  encoder.init_encoder();
  encoder.init_button();
  menu.init_menu_tree();
  nativeUsb.init_native_usb();
  light_sensor.init_LDR();
  card.init_sd_cards();
  current_meter.init_current_meter();
}


void due_loop() {
  while (1) {

    // //do these periodically based on config
    // cards.check_for_sd_card_inserted();
    // internet.check_connection();
    // graphics.update_brightness();

    fans.update_temperatures();
    fans.set_fan_speed();
    led_strip.led_strip_set_freq();
    light_sensor.get_readings();
    encoder.handle_interupts();
    menu.display_menu();
    current_meter.get_readings();

    // update_display();  //push additional data to screens as required
    //
    // //do this based speed variable
    // update_text_location(); //write location variable to be sent by interrupt
    host.check_serial();
    host.print_messages();

    card.check_for_sd_card();
    //card.update_data_log();
    
  }

}

#endif // Due_CPP

