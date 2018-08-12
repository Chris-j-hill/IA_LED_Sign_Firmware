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

struct Timers timers; //timers struct



extern struct Frame text_frame;
extern struct Frame sensor_data_frame;

extern Frame menu_frame;
extern Frame pos_frame;

extern struct Text text_parameters[MAX_NUM_OF_TEXT_OBJECTS];
extern struct Text_cursor text_cursor[MAX_NUM_OF_TEXT_OBJECTS];


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

  graphics.push_string_data();
  encoder.set_encoder_position(1000);
  coms_serial.send_menu_frame(12);
  graphics.reset_position(3);
  Serial.print("x loc");
  Serial.println(text_cursor[3].x);
  Serial.print("y loc");
  Serial.println(text_cursor[3].y);
  coms_serial.send_pos_frame(3);

  for (byte i = 0; i < text_frame.frame_length; i++) {

    Serial.print(text_frame.frame_buffer[i]);
    Serial.print("\t");
    host.println_bits(text_frame.frame_buffer[i], 8, BIN);
  }
Serial.println();
Serial.println();

  for (byte i = 0; i < sensor_data_frame.frame_length; i++) {

    Serial.print(sensor_data_frame.frame_buffer[i]);
    Serial.print("\t");
    host.println_bits(sensor_data_frame.frame_buffer[i], 8, BIN);
  }
Serial.println();
Serial.println();

  for (byte i = 0; i < menu_frame.frame_length; i++) {

    Serial.print(menu_frame.frame_buffer[i]);
    Serial.print("\t");
    host.println_bits(menu_frame.frame_buffer[i], 8, BIN);
  }
Serial.println();
Serial.println();

  for (byte i = 0; i < pos_frame.frame_length; i++) {

    Serial.print(pos_frame.frame_buffer[i]);
    Serial.print("\t");
    host.println_bits(pos_frame.frame_buffer[i], 8, BIN);
  }
Serial.println();
Serial.println();
  //  while (1) {
  //        Serial3.println();
  //    Serial3.write(text_frame.frame_buffer, text_frame.frame_length);
  //    Serial3.println();
  //    delay(500);
  //  }
}


void due_loop() {
  while (1) {

    //push string and related data if new data retrieved
    graphics.push_string_data();


    //check for requests from megas (eg transmission errors)
    //coms_serial.check_megas();

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

    //check for user seria  l input and print requested data
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

