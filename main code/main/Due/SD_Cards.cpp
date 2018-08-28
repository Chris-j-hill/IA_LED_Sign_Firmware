#ifndef SD_Cards_CPP
#define SD_Cards_CPP

#include "SD_Cards.h"

#include "Coms.h"
#include "Fans.h"
#include "Current_Control.h"
#include "Graphics.h"
#include "Led_Strip.h"
#include "Encoder.h"
#include "Coms_serial.h"


#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>


extern char _end;
extern "C" char *sbrk(int i);
char *ramstart = (char *)0x20070000;
char *ramend = (char *)0x20088000;





SD_Strings SD_string;

SdFile file1;                 // file handling objects, use two objects to address both open files at once
SdFile file2;                 // nb: if modifying files, make sure to close at the end of function
//sd card objects,
SdFat external_sd_card;            //external sd card
SdFat internal_sd_card;            //onboard sd card

#ifdef ENABLE_SD_CARDS
bool enable_sd_cards = true;
#else
bool enable_sd_cards = false;
#endif

bool sd_cards_enabled = false;


SD_Card card1;    //external card struct
SD_Card card2;    //internal card struct

extern char text_str[MAX_NUM_OF_TEXT_OBJECTS][MAX_TWEET_SIZE];

extern struct Fan_Struct fan_parameters;
extern struct Temp_sensor temp_parameters;
extern struct LDR_Struct light_sensor_parameters;
extern struct Current_Meter_Struct current_meter_parameters;
extern struct Text text_parameters[MAX_NUM_OF_TEXT_OBJECTS];
extern struct Text_cursor text_cursor[MAX_NUM_OF_TEXT_OBJECTS];
extern struct Led_Strip_Struct led_strip_parameters;
extern struct Encoder_Struct encoder_parameters;
extern struct Button_Struct button_parameters;


extern Graphics graphics;
extern Coms_Serial coms_serial;

extern byte screen_brightness;
extern byte screen_mode;


Card_LED card_led;

//byte CARD_LED_RED[3] = {255, 0, 0};
//byte CARD_LED_GREEN[3] = {0, 255, 0};
//byte CARD_LED_ORANGE[3] = {255, 120, 0};
//byte CARD_LED_PURPLE[3] = {128, 0, 190};
//byte CARD_LED_BLUE[3] = {0, 0, 255};

bool CARD_LED_RED[3] = {1, 0, 0};
bool CARD_LED_GREEN[3] = {0, 1, 0};
bool CARD_LED_ORANGE[3] = {1, 1, 0};
bool CARD_LED_PURPLE[3] = {1, 0, 1};
bool CARD_LED_BLUE[3] = {0, 0, 1};


//more useful colour names
bool CARD_LED_READING[3] = {0}; //mapping to these arrays in constructor
bool CARD_LED_MOUNTED[3] = {0};
bool CARD_LED_UNMOUNTED[3] = {0};
bool CARD_LED_CARD_NOT_FOUND[3] = {0};


char command [COMMAND_LENGTH] = {'\0'};
char config_name[CONFIG_PROFILE_NAME_LENGTH] = {'\0'};
char data_found[MAX_TWEET_SIZE] = {'\0'};

void ram_stats() {
  char *heapend = sbrk(0);
  register char * stack_ptr asm ("sp");
  struct mallinfo mi = mallinfo();
  delay(1);
  printf("\nDynamic ram used: %d\n", mi.uordblks);
  delay(1);
  printf("Program static ram used %d\n", &_end - ramstart);
  delay(1);
  printf("Stack ram used %d\n\n", ramend - stack_ptr);
  delay(1);
  printf("My guess at free mem: %d\n", stack_ptr - heapend + mi.fordblks);
}


Card::Card() {

  //#define SD1_ENABLE  45    //internal sd card CS pin
  //#define SD2_ENABLE  44
  card1.pin = SD1_ENABLE;
  card2.pin = SD2_ENABLE;

  card1.enabled = enable_sd_cards;
  card2.enabled = enable_sd_cards;

  card1.working_dir = EXTERNAL_SD_CARD_DIRECTORY_NAME;
  //card1.working_dir = INTERNAL_SD_CARD_DIRECTORY_NAME;    //change this line when using two cards
  card2.working_dir = INTERNAL_SD_CARD_DIRECTORY_NAME;


}

void Card::init_sd_cards() {

  pinMode(card1.pin, OUTPUT);
  digitalWrite(card1.pin, HIGH);

  pinMode(card2.pin, OUTPUT);
  digitalWrite(card2.pin, HIGH);
}

void Card::check_for_sd_card() {

  static uint32_t internal_sd_card_prev_read_time = millis();
  static uint32_t external_sd_card_prev_read_time = millis();
  static bool auto_eject_ext_card_now = false;
  if (card1.enabled) {
    if (millis() > external_sd_card_prev_read_time + (card1.detected ? CHECK_EXTERNAL_SD_CARD_PERIOD<<2 : CHECK_EXTERNAL_SD_CARD_PERIOD)) { //if card detected, reduce polling frequency, only need quick response on insertion
      
      external_sd_card_prev_read_time = millis();
//      noInterrupts();
      bool card_found = external_sd_card.begin(card1.pin);  //this is very slow, ensure only test once
//      interrupts();
      if (card_found && !card1.detected) {
        
        card_led.set_card_colour(CARD_LED_READING); //indicate were reading from card

        card1.detected = true;
        check_for_files(EXTERNAL_CARD);  //check if files exist on external card

        if (card2.enabled && card2.detected) {            //attempt to update to newer version if both internal and external cards exist

          if (card1.network_file_exists)// if file exists on external device, copy
            copy_file(EXT_NETWORK_FILE, INT_NETWORK_FILE, EXTERNAL_CARD , INTERNAL_CARD );
          if (card1.disp_string_file_exists)
            copy_file(EXT_STRING_FILE, INT_STRING_FILE, EXTERNAL_CARD , INTERNAL_CARD);
          if (card1.calibration_file_exists)
            copy_file(EXT_CALIBRATION_FILE, INT_CALIBRATION_FILE, EXTERNAL_CARD , INTERNAL_CARD);
          if (card1.bitmap_file_exists)
            copy_file(EXT_BITMAP_FILE, INT_BITMAP_FILE, EXTERNAL_CARD , INTERNAL_CARD);

          //sucessfully copied all files, at the end of this function, eject card
          auto_eject_ext_card_now = true;
        }

        Serial.println("Retrieve files");
        if (card1.network_file_exists)   //if file exists
          retrieve_data(EXT_NETWORK_FILE);//get contents
        if (card1.disp_string_file_exists) {
          retrieve_data(EXT_STRING_FILE);
          // text_parameters[0].megas_up_to_date = false;
          graphics.push_string_data();//push retrieved string and related data (size, colour etc) to megas
        }
        if (card1.calibration_file_exists)
          retrieve_data(EXT_CALIBRATION_FILE);

        card_led.set_card_colour(CARD_LED_MOUNTED); //back to normal

      }
      else if (!card_found && card1.detected) { //card was previously detected but not initialising now, card removed edge detector
        card1.detected = false;
        files_dont_exist(EXTERNAL_CARD);
        card_led.set_card_colour(CARD_LED_CARD_NOT_FOUND);
      }
    }
  }
  if (card2.enabled) {
    if (millis() > internal_sd_card_prev_read_time + (card2.detected ? CHECK_INTERNAL_SD_CARD_PERIOD<<2 : CHECK_INTERNAL_SD_CARD_PERIOD)) {

      internal_sd_card_prev_read_time = millis();
      bool card_found = internal_sd_card.begin(card2.pin);  //this is very slow, ensure only test once

      if (card_found && !card2.detected) {
        card2.detected = true;

     
        check_for_files(INTERNAL_CARD);  //check if files exist on external card

        if (card1.enabled && card1.detected) {
          card_led.set_card_colour(CARD_LED_READING);
          copy_file(EXT_NETWORK_FILE, INT_NETWORK_FILE, EXTERNAL_CARD , INTERNAL_CARD);
          copy_file(EXT_STRING_FILE, INT_STRING_FILE, EXTERNAL_CARD , INTERNAL_CARD);
          copy_file(EXT_CALIBRATION_FILE, INT_CALIBRATION_FILE, EXTERNAL_CARD , INTERNAL_CARD);
          copy_file(EXT_BITMAP_FILE, INT_BITMAP_FILE, EXTERNAL_CARD , INTERNAL_CARD);
          card_led.set_card_colour(CARD_LED_MOUNTED);

          //sucessfully copied all files, at the end of this function, eject card
          auto_eject_ext_card_now = true;
        }

        if (card2.network_file_exists)
          retrieve_data(INT_NETWORK_FILE);
        if (card2.disp_string_file_exists)
          retrieve_data(INT_STRING_FILE);
        graphics.push_string_data();//push string and related data (size, colour etc)
        if (card2.calibration_file_exists)
          retrieve_data(INT_CALIBRATION_FILE);
      }
      else if (!card_found && card2.detected) {
        card2.detected = false;
        files_dont_exist(INTERNAL_CARD);
      }
    }
  }
#ifdef DISABLE_EXTERNAL_PORT
  if (auto_eject_ext_card_now) {
    auto_eject_ext_card_now = false; // dont run this every loop
    safely_eject_card(EXTERNAL);    // eject card
  }
#endif
}

void Card::check_for_files(byte card_to_check) {

  if (card_to_check == EXTERNAL_CARD) { //checking external card

    external_sd_card.set_as_active();
    //check if dir exists
    if (external_sd_card.exists(card1.working_dir)) {
      card1.directory_exists = true;
      external_sd_card.chdir(card1.working_dir); //open dir if exists
      Serial.println("dir found");
#ifdef ALLOW_NETWORK_FILES
      if (external_sd_card.exists(EXT_NETWORK_FILE))
        card1.network_file_exists = true;
      else
        card1.network_file_exists = false;
#endif

#ifdef ALLOW_DISP_STRING_FILES
      if (external_sd_card.exists(EXT_STRING_FILE)) {
        card1.disp_string_file_exists = true;
        Serial.println("strings file found");
      }
      else
        card1.disp_string_file_exists = false;
#endif

#ifdef ALLOW_DATA_LOG_FILES   //make the log file if it doesnt exist
      if (external_sd_card.exists(EXT_LOG_FILE)) {
        card1.log_file_exists = file1.open(EXT_LOG_FILE, O_WRITE | O_CREAT);
        //        file1.write("DataLog.BIN");
      }
      else
        card1.log_file_exists = true;
      file1.close();
#endif

#ifdef ALLOW_CALIBRATION_FILES
      if (external_sd_card.exists(EXT_CALIBRATION_FILE))
        card1.calibration_file_exists = true;
      else
        card1.calibration_file_exists = false;
#endif

#ifdef ALLOW_INSTRUCTION_FILES
      if (external_sd_card.exists(EXT_INSTRUCTION_FILE))
        card1.instruction_file_exists = true;
      else
        card1.instruction_file_exists = false;
#endif

#ifdef ALLOW_BITMAP_FILES
      if (external_sd_card.exists(EXT_BITMAP_FILE))
        card1.bitmap_file_exists = true;
      else
        card1.bitmap_file_exists = false;
#endif
    }
    else {
      Serial.println("failed to open dir, connot test for files");
      card1.directory_exists = false;
      card1.network_file_exists = false;
      card1.disp_string_file_exists = false;
      card1.log_file_exists = false;
      card1.instruction_file_exists = false;
      card1.calibration_file_exists = false;
      card1.bitmap_file_exists = false;
    }
  }

  else if (card_to_check == INTERNAL_CARD) { //checking internal card

    internal_sd_card.set_as_active();
    if (!internal_sd_card.exists(card2.working_dir)) {
      card2.directory_exists = internal_sd_card.mkdir(card2.working_dir); //make dir if does not exist
    }
    else
      card2.directory_exists = true;

    if (card2.directory_exists) {                // if dir exists
      internal_sd_card.chdir(card2.working_dir); // open dir
      Serial.println("dir found");
#ifdef ALLOW_NETWORK_FILES
      if (!internal_sd_card.exists(INT_NETWORK_FILE)) {
        card2.network_file_exists = false;
        //card2.network_file_exists = file2.open("Networks.BIN", O_WRITE | O_CREAT );
        //file2.write("Networks.BIN");
      }
      else
        card2.network_file_exists = true;
      file2.close();
#endif

#ifdef ALLOW_DISP_STRING_FILES
      if (!internal_sd_card.exists(INT_STRING_FILE)) {
        card2.disp_string_file_exists = false;
        Serial.println("found string file");
        //card2.disp_string_file_exists = file2.open(INT_STRING_FILE, O_WRITE | O_CREAT);
        //file2.write("DisplayString.BIN");
      }
      else
        card2.disp_string_file_exists = true;
      file2.close();
#endif

#ifdef ALLOW_DATA_LOG_FILES
      if (!internal_sd_card.exists(INT_LOG_FILE)) {
        card2.log_file_exists = file2.open(INT_LOG_FILE, O_WRITE | O_CREAT);
        //file2.write("DataLog.BIN");
      }
      else
        card2.log_file_exists = true;
      file2.close();
#endif

#ifdef ALLOW_CALIBRATION_FILES
      if (!internal_sd_card.exists(INT_CALIBRATION_FILE)) {
        card2.calibration_file_exists = false;
        //card2.calibration_file_exists = file2.open(INT_CALIBRATION_FILE, O_WRITE | O_CREAT);
        //file2.write("Calibration.BIN");
      }
      else
        card2.calibration_file_exists = true;
      file2.close();
#endif

#ifdef ALLOW_INSTRUCTION_FILES
      if (!internal_sd_card.exists(INT_INSTRUCTION_FILE)) {
        card2.instruction_file_exists = false;
        //card2.instruction_file_exists = file2.open(INT_INSTRUCTION_FILE, O_WRITE | O_CREAT);
        //file2.write("Instructions.BIN");
      }
      else
        card2.instruction_file_exists = true;
      file2.close();
#endif

#ifdef ALLOW_BITMAP_FILES
      if (!internal_sd_card.exists(INT_BITMAP_FILE))
        card2.bitmap_file_exists = false;
      //card2.bitmap_file_exists = file2.open(INT_BITMAP_FILE, O_WRITE | O_CREAT);
      else
        card2.bitmap_file_exists = true;
      file2.close();
#endif
    }

    else { //dir does not exist, ensure all files disabled
      Serial.println("dir does not exist");
      card2.network_file_exists = false;
      card2.disp_string_file_exists = false;
      card2.log_file_exists = false;
      card2.instruction_file_exists = false;
      card2.calibration_file_exists = false;
      card2.bitmap_file_exists = false;
    }
  }
}

void Card::copy_file(String from_string, String to_string, byte from_device, byte to_device) {

  if (!external_sd_card.exists(card1.working_dir)) return;
  if (!internal_sd_card.exists(card2.working_dir)) return;
  //  if (!ping()) return;

  char from_filename[STRING_FILE_COMMAND_NEXT_FILE_NAME_LENGTH] = {'\0'};
  strcpy(from_filename, SD_string.null_string);
  strcpy(from_filename, from_string.c_str());

  char to_filename[STRING_FILE_COMMAND_NEXT_FILE_NAME_LENGTH] = {'\0'};
  strcpy(to_filename, SD_string.null_string);
  strcpy(to_filename, to_string.c_str());

  switch (from_device) {

    case EXTERNAL_CARD:

      if (to_device == INTERNAL_CARD) {

        bool file1_opened = false;
        bool file2_opened = false;
        int16_t n = 0;

        external_sd_card.set_as_active();
        external_sd_card.chdir(card1.working_dir);

        if (card1.calibration_file_exists && (strcmp(from_filename, EXT_CALIBRATION_FILE) == 0))
          file1_opened = file1.open(EXT_CALIBRATION_FILE, O_READ);
        else if (card1.instruction_file_exists && (strcmp(from_filename, EXT_INSTRUCTION_FILE) == 0))
          file1_opened = file1.open(EXT_INSTRUCTION_FILE, O_READ);
        else if (card1.disp_string_file_exists && (strcmp(from_filename, EXT_STRING_FILE) == 0))
          file1_opened = file1.open(EXT_STRING_FILE, O_READ);
        else if (card1.network_file_exists && (strcmp(from_filename, EXT_NETWORK_FILE) == 0))
          file1_opened = file1.open(EXT_NETWORK_FILE, O_READ);
        else if (card1.bitmap_file_exists && (strcmp(from_filename, EXT_BITMAP_FILE) == 0))
          file1_opened = file1.open(EXT_BITMAP_FILE, O_READ);


        internal_sd_card.set_as_active();
        internal_sd_card.chdir(card2.working_dir);

        if ((strcmp(to_filename, INT_CALIBRATION_FILE) == 0))
          file2_opened = file2.open(INT_CALIBRATION_FILE, O_WRITE | O_TRUNC | O_CREAT);
        else if ((strcmp(to_filename, INT_INSTRUCTION_FILE) == 0))
          file2_opened = file2.open(INT_INSTRUCTION_FILE, O_WRITE | O_TRUNC | O_CREAT);
        else if ((strcmp(to_filename, INT_STRING_FILE) == 0))
          file2_opened = file2.open(INT_STRING_FILE, O_WRITE | O_TRUNC | O_CREAT);
        else if ((strcmp(to_filename, INT_NETWORK_FILE) == 0))
          file2_opened = file2.open(INT_NETWORK_FILE, O_WRITE | O_TRUNC | O_CREAT);
        else if ((strcmp(to_filename, INT_BITMAP_FILE) == 0))
          file2_opened = file2.open(INT_BITMAP_FILE, O_WRITE | O_TRUNC | O_CREAT);

        if (file1_opened && file2_opened) {
          while (n != -1) {
            external_sd_card.set_as_active();
            n = file1.read();
            //Serial.println((char)n);

            if (n == -1)break;
            else {
              internal_sd_card.set_as_active();
              file2.write((char)n);
            }
          }
        }
        else {
          Serial.println("Did not copy, file does not exist");
        }

        if (strcmp(to_filename, EXT_NETWORK_FILE) == 0)           card1.network_file_exists = file1_opened;
        else if (strcmp(to_filename, EXT_STRING_FILE) == 0)       card1.disp_string_file_exists = file1_opened;
        else if (strcmp(to_filename, EXT_INSTRUCTION_FILE) == 0)  card1.instruction_file_exists = file1_opened;
        else if (strcmp(to_filename, EXT_CALIBRATION_FILE) == 0)  card1.calibration_file_exists = file1_opened;
        else if (strcmp(to_filename, EXT_BITMAP_FILE) == 0)       card1.bitmap_file_exists = file1_opened;

        if (strcmp(to_filename, INT_NETWORK_FILE) == 0)           card2.network_file_exists = file2_opened;
        else if (strcmp(to_filename, INT_STRING_FILE) == 0)       card2.disp_string_file_exists = file2_opened;
        else if (strcmp(to_filename, INT_INSTRUCTION_FILE) == 0)  card2.instruction_file_exists = file2_opened;
        else if (strcmp(to_filename, INT_CALIBRATION_FILE) == 0)  card2.calibration_file_exists = file2_opened;
        else if (strcmp(to_filename, INT_BITMAP_FILE) == 0)       card2.bitmap_file_exists = file2_opened;

        file1.close();
        file2.close();

      }
      else if (to_device == RASP_PI) {

      }
      else
        Serial.println(F("Error:cannot copy to given location"));

      break;

    case INTERNAL_CARD:
      if (to_device == EXTERNAL_CARD) {
        file1.open(from_filename, O_WRITE | O_TRUNC);
        file2.open(to_filename, O_READ);
        copy(from_device, to_device);
        file1.close();
        file2.close();
      }
      else if (to_device == RASP_PI) {

      }
      else
        Serial.println(F("Error:cannot copy to given location"));
      break;

    case RASP_PI:
      if (to_device == EXTERNAL_CARD) {

      }
      else if (to_device == EXTERNAL_CARD) {

      }
      else
        Serial.println(F("Error:cannot copy to given location"));
      break;

    default:
      Serial.println(F("Error:cannot copy from given location"));
      break;

  }
}

void Card::copy(byte from_device, byte to_device) {

  int16_t n;
  while (1) {
    //external_sd_card.chdir(card1.working_dir);
    n = file1.read();
    Serial.println(n);
    if (n == -1) break;
    else if (n < 0) {
      Serial.println("error reading external file");
      break;
    }
    else file2.write((char)n);
  }
}

void Card::retrieve_data(String filename, byte obj_num, bool get_next_config) {

  if (filename == INT_STRING_FILE || filename == EXT_STRING_FILE)
    retrieve_string(filename, obj_num, get_next_config);


  else if (filename == INT_NETWORK_FILE || filename == EXT_NETWORK_FILE) {

    char copy_buffer[60] = {'\0'};

    if (filename == INT_NETWORK_FILE) {
      if (!internal_sd_card.exists(card2.working_dir)) return;
      internal_sd_card.chdir(card2.working_dir);

      file2.open(INT_NETWORK_FILE, O_READ);
      file2.read(copy_buffer, sizeof(copy_buffer));
      file2.close();
    }
    else {
      if (!external_sd_card.exists(card1.working_dir)) return;
      external_sd_card.chdir(card1.working_dir);

      file1.open(EXT_NETWORK_FILE, O_READ);
      file1.read(copy_buffer, sizeof(copy_buffer));
      file1.close();
    }
    String file_content = copy_buffer;
    //    Serial.println(file_content);

    //parse content
    byte network_keyword = file_content.indexOf(':');
    byte line_end = file_content.indexOf('\n', network_keyword + 1);
    byte ssid_keyword = file_content.indexOf(':', line_end + 1);

    String network = file_content.substring(network_keyword + 1, line_end);
    String password = file_content.substring(ssid_keyword + 1);

    network.trim();
    password.trim();

    //save locally
    SD_string.Network = network;
    SD_string.Password = password;


  }

  else if (filename == INT_CALIBRATION_FILE || filename == EXT_CALIBRATION_FILE) {

    if (filename == INT_CALIBRATION_FILE) {
      if (!internal_sd_card.exists(card2.working_dir)) return;
      internal_sd_card.chdir(card2.working_dir);
      file1.open(INT_CALIBRATION_FILE, O_READ);
    }

    else if (filename == EXT_CALIBRATION_FILE) {
      if (!external_sd_card.exists(card1.working_dir)) {
        Serial.println("file_doesnt_exist");
        return;
      }
      external_sd_card.chdir(card1.working_dir);
      file1.open(EXT_CALIBRATION_FILE, O_READ);
    }


    int16_t char_read;

    while (char_read != -1 ) { //|| num_lines<max_lines) {

      int reads = 0;
      char command [COMMAND_LENGTH] = {'\0'};

      while (reads < COMMAND_LENGTH) {
        char_read = file1.read();
        if ((char)char_read == ':' || (char)char_read == '\n' ||  char_read == -1 ) break;
        else {
          command[reads] = (char)char_read;
          reads++;
        }
      }

      if (char_read == ':') {
        char data_found[CALIBRATION_FILE_DATA_CHAR_LENGTH] = {'\0'};
        reads = 0;
        while (reads < CALIBRATION_FILE_DATA_CHAR_LENGTH) {
          char_read = file1.read();
          if ((char)char_read == '\n' ||  char_read == -1 ) break;
          else {
            data_found[reads] = (char)char_read;
            reads++;
          }
        }
        int value_found = atoi(data_found); //convert to int for some data types
        bool bool_found;

        if ((data_found[0] == 'T' || data_found[0] == 't') && data_found[1] == 'r' && data_found[2] == 'u' && data_found[3] == 'e')
          bool_found = true;
        else
          bool_found = false;

        if (strcmp(command, CALIBRATION_FILE_COMMAND_FAN_ENABLED) == 0)                   fan_parameters.enabled = bool_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_TEMP1_ENABLED) == 0)            temp_parameters.enabled1 = bool_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_TEMP2_ENABLED) == 0)            temp_parameters.enabled2 = bool_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_TEMP3_ENABLED) == 0)            temp_parameters.enabled3 = bool_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_STRIP_ENABLED) == 0)            led_strip_parameters.enabled = bool_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_CURRENT1_ENABLED) == 0)         current_meter_parameters.enabled1 = bool_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_CURRENT2_ENABLED) == 0)         current_meter_parameters.enabled2 = bool_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_LDR1_ENABLED) == 0)             light_sensor_parameters.enabled1 = bool_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_LDR2_ENABLED) == 0)             light_sensor_parameters.enabled2 = bool_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_ENCODER_ENABLED) == 0)          encoder_parameters.enabled = bool_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_BUTTON_ENABLED) == 0)           button_parameters.enabled = bool_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_SD_CARD1_ENABLED) == 0)         card1.enabled = bool_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_SD_CARD2_ENABLED) == 0)         card2.enabled = bool_found;


        else if (strcmp(command, CALIBRATION_FILE_COMMAND_FAN_MIN) == 0)                  fan_parameters.fan_minimum = value_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_FAN_SPEED) == 0) {
          fan_parameters.manual_set_value = value_found;
          fan_parameters.manual = true;
        }
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_FAN_INCREMENT) == 0)            fan_parameters.change_increment = value_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_FAN_INTERVAL) == 0)             fan_parameters.fan_change_interval = value_found;


        else if (strcmp(command, CALIBRATION_FILE_COMMAND_STRIP_BRIGHTNESS) == 0)         led_strip_parameters.current_brightness = value_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_STRIP_MINUMUM) == 0)            led_strip_parameters.minimum_on = value_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_STRIP_INCREMENT) == 0)          led_strip_parameters.change_increment = value_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_STRIP_INTERVAL) == 0)           led_strip_parameters.change_interval = value_found;


        else if (strcmp(command, CALIBRATION_FILE_COMMAND_ENCODER_SENSITIVITY) == 0)      encoder_parameters.sensitivity = value_found;


        else if (strcmp(command, CALIBRATION_FILE_COMMAND_LDR_CONFIG_MAX1) == 0)           light_sensor_parameters.config_max1 = value_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_LDR_CONFIG_MAX2) == 0)           light_sensor_parameters.config_max2 = value_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_LDR_CONFIG_MIN1) == 0)           light_sensor_parameters.config_min1 = value_found;
        else if (strcmp(command, CALIBRATION_FILE_COMMAND_LDR_CONFIG_MIN2) == 0)           light_sensor_parameters.config_min2 = value_found;
      }
    }
    file1.close();


  }
}

void Card::log_data(String filename, bool truncate, bool print_header) {


  char header_buff[] = "Time,Fans,AvgTemp,Current,LDR,ScreenBrightness";
  char buf[10];

  String time_ = String(millis());
  String fans_ = String(fan_parameters.target_speed);
  String temp_ = String(temp_parameters.avg);
  String current_ = String(current_meter_parameters.total);
  String light_ = String(light_sensor_parameters.avg_reading);
  String screen_ = String(screen_brightness);

  String data_to_write = time_ + ',' + fans_ + ',' + temp_ + ',' + current_ + ',' + light_ + ',' + screen_;
  char data_buff[data_to_write.length()];

  strcpy(data_buff, data_to_write.c_str());

  SdFat sd_card;

  if (filename == INT_LOG_FILE) {
    sd_card.begin(card2.pin);
    if (!sd_card.exists(card2.working_dir)) return;
    sd_card.chdir(card2.working_dir);

    if (truncate && print_header)
      file2.open(INT_LOG_FILE, O_WRITE | O_TRUNC);
    else
      file2.open(INT_LOG_FILE, O_WRITE | O_AT_END);

    if (print_header) {
      file2.write(header_buff, sizeof(header_buff)) ;
      file2.write("\n", 1);
    }


    file2.write(data_buff, sizeof(data_buff));
    file2.write("\n", 1);
    file2.close();
  }

  else if (filename == EXT_LOG_FILE) {
    sd_card.begin(card1.pin);
    if (!sd_card.exists(card1.working_dir)) return;
    sd_card.chdir(card1.working_dir);

    if (truncate && print_header)
      file1.open(EXT_LOG_FILE, O_WRITE | O_TRUNC);
    else
      file1.open(EXT_LOG_FILE, O_WRITE | O_AT_END);

    if (print_header) {
      file1.write(header_buff, sizeof(header_buff)) ;
      file1.write("\n", 1);
    }


    file1.write(data_buff, sizeof(data_buff));
    file1.write("\n", 1);
    file1.close();
  }
}


void Card::update_data_log(byte give_priority_to) {

  static uint32_t last_log_time = millis();
  static byte logging_on_device = 0;      // what device were we last using
  static bool first_internal_card_log_event = true; //print header if first time logging to this device
  static bool first_external_card_log_event = true;
  static bool first_r_pi_log_event = true;
  static byte error_code = 0;

  if (millis() > last_log_time + LOGGING_PERIOD) {

    last_log_time = millis();

    switch (give_priority_to) {

      case INTERNAL_CARD:

        if (card2.enabled && card2.detected && card2.log_file_exists) { //internal card gets priority, then pi, then external

          if (first_internal_card_log_event) {
            log_data(INT_LOG_FILE, true, true); //on startup, or first time switching to this device, clear file and print header
            first_internal_card_log_event = false;
          }

          else  //otherwise just print the data
            log_data(INT_LOG_FILE, false, false);
        }

        else if (error_code == 0) {
          Serial.println(F("Failed to log to internal card"));
          error_code = 1; //print once
        }
        break;

      case EXTERNAL_CARD:

        if (card1.enabled && card1.detected && card1.log_file_exists) {
          error_code = 0;
          if (first_external_card_log_event) {
            log_data(EXT_LOG_FILE, true, true);
            first_external_card_log_event = false;
          }

          else
            log_data(EXT_LOG_FILE, false, false);
        }
        else if (error_code == 0) {

          Serial.println(F("Failed to log to external card"));
          error_code = 1;
        }
        break;

      case RASP_PI:
        break;

      default:  //log to whichever is available, internal card gets priority, then pi, then external
        if (card2.enabled && card2.detected && card2.log_file_exists) {
          error_code = 0;
          if (first_internal_card_log_event) {
            log_data(INT_LOG_FILE, true, true); //on startup, clear file and print header
            first_internal_card_log_event = false;
          }


          else  //otherwise just print the data
            log_data(INT_LOG_FILE, false, false);

        }



        //else if(pi is detected){
        //
        //}


        else if (card1.enabled && card1.detected && card1.log_file_exists) {
          error_code = 0;
          if (first_external_card_log_event) {
            log_data(EXT_LOG_FILE, true, true);
            first_external_card_log_event = false;
          }
          else
            log_data(EXT_LOG_FILE, false, false);
        }

        else if (error_code == 0) {
          Serial.println(F("ERROR: Data log failed, no devices found, or file doesnt exist"));
          error_code = 1;
        }
    }
  }
}

void Card::safely_eject_card(byte card_num) {

  if (card_num == INTERNAL_CARD) { // stop checking for card
    card2.enabled = false;     // prevent scanning for card
    card2.detected = false;
  }

  else if (card_num == EXTERNAL_CARD) {
    card1.enabled = false;
    card1.detected = false;
    card_led.set_card_colour(CARD_LED_UNMOUNTED);
  }
}


void Card::mount_card(byte card_num) {

  if (card_num == INTERNAL_CARD) { //check for card
    card2.enabled = true;     // allow scanning for card
    card2.detected = false;   //on first detection, read card
  }

  else if (card_num == EXTERNAL_CARD) {
    card1.enabled = true;
    card1.detected = false;
    card_led.set_card_colour(CARD_LED_CARD_NOT_FOUND);
  }
}

void Card::files_dont_exist(byte device) {

  if (device == INTERNAL_CARD) {
    card2.network_file_exists = false;
    card2.disp_string_file_exists = false;
    card2.log_file_exists = false;
    card2.instruction_file_exists = false;
    card2.calibration_file_exists = false;
    card2.bitmap_file_exists = false;
  }

  else if (device == EXTERNAL_CARD) {
    card1.network_file_exists = false;
    card1.disp_string_file_exists = false;
    card1.log_file_exists = false;
    card1.instruction_file_exists = false;
    card1.calibration_file_exists = false;
    card1.bitmap_file_exists = false;
  }
}
//
//bool Card::file_is_a_next_file(String filename) {
//  for (byte i = 0; i < MAX_NUM_OF_TEXT_OBJECTS; i++) {
//    Serial.println(SD_string.next_config_profile[i]);
//    if (text_cursor[i].check_for_new_file && filename == SD_string.next_config_profile[i]) return true;
//
//  }
//  return false;
//}



//spooky function, behaves weirdly, possible memery issue?
void Card::retrieve_string(String filename, byte obj_num, bool get_next_config) {
  byte i = 0;
  int16_t char_read;
  byte max_i = MAX_NUM_OF_TEXT_OBJECTS;
  
//  ram_stats();

  SD_Card card1_temp = card1; //fix this issue, bodge fix for now
  SD_Card card2_temp = card2; //this functions seems to over write card 1 and 2 struct data

  /*if (get_next_config && filename == INT_STRING_FILE) {
    internal_sd_card.set_as_active();
    if (!internal_sd_card.exists(INT_STRING_FILE)) {
      Serial.println(F("file doesnt exist, cant retrieve data"));
      return;
    }
    Serial.println("file exists");
    file1.open(INT_STRING_FILE, O_READ);
    }

    else if (get_next_config && filename == EXT_STRING_FILE) {
    //external_sd_card.set_as_active();
    if (!external_sd_card.exists(EXT_STRING_FILE)) {
      Serial.println(F("file doesnt exist, cant retrieve data"));
      return;
    }
    //Serial.println("file exists");
    file1.open(EXT_STRING_FILE, O_READ);
    }

    else*/ if (filename == INT_STRING_FILE) {
    internal_sd_card.set_as_active();
    //internal_sd_card.ls();
    if (!internal_sd_card.exists(card2.working_dir)) {

      //Serial.println(F("int dir doesnt exist, cant retrieve data"));
      return;
    }
    internal_sd_card.chdir(card2.working_dir);
    file1.open(INT_STRING_FILE, O_READ);
  }
  else if (filename == EXT_STRING_FILE) {
    external_sd_card.set_as_active();
    if (!external_sd_card.exists(card1.working_dir)) {
      external_sd_card.chdir("/");
      if (!external_sd_card.exists(card1.working_dir)) {
        //Serial.println(F("ext dir doesnt exist, cant retrieve data"));
        return;
      }
    }
    external_sd_card.chdir(card1.working_dir);
    //external_sd_card.ls();
    file1.open(EXT_STRING_FILE, O_READ);
  }

  else {
    return;
  }

  //  Serial.print(card1.pin);
  //  Serial.print("\t");
  //  Serial.println(card2.pin);


  bool break_after_one_config_profile = false;  //if meant to look for one specific profile
  bool x_start = false;   //place holders until file read
  bool y_start = false;
  bool x_end = false;
  bool y_end = false;
  bool disp_loops_found_x = false;
  bool disp_loops_found_y = false;
  bool disp_time_found = false;
  bool next_file_found = false;
  uint16_t reads = 0;
  
  while (char_read != -1 && i < max_i && !break_after_one_config_profile) { //scan file for text obj maker ('{')
    char_read = file1.read();

    if (char_read == -1 ) break;
    else if (char_read == '{') {   //found an obj marker, decode until '}' found
      //       Serial.println(F("found obj marker"));

      x_start = false;   //place holders until file read
      y_start = false;
      x_end = false;
      y_end = false;
      disp_loops_found_x = false;
      disp_loops_found_y = false;
      disp_time_found = false;
      next_file_found = false;

      if (file1.peek() == 13) //skip return carraige
        file1.read();
      if (file1.peek() == 10)
        file1.read();
      if (file1.peek() != 40) {
        while (char_read != '}') {

          reads = 0;

          if (get_next_config) {        //if were meant to be looking for a specific text config block
            //            char config_name[CONFIG_PROFILE_NAME_LENGTH] = {'\0'};
            config_name[CONFIG_PROFILE_NAME_LENGTH] = {'\0'};

            bool outside_config_profile_name = true;

            while (char_read != ')') {  //search until we find the specific identifier
              char_read = file1.read();
              if (char_read == -1) break;
              else if (char_read == '(' && outside_config_profile_name) {
                memset(config_name, '\0', CONFIG_PROFILE_NAME_LENGTH);  //found the start of a config profile
                outside_config_profile_name = false;
              }
              else if (char_read == ')' && !outside_config_profile_name)
                outside_config_profile_name = true;
              else if (!outside_config_profile_name) {
                //Serial.print((char)char_read);
                config_name[reads] = (char)char_read;
                reads++;
                if (strcmp(config_name, SD_string.next_config_profile[obj_num]) == 0) {
                  break_after_one_config_profile = true;
                }
              }
            }
          }
          reads = 0;
          memset(command, '\0', COMMAND_LENGTH);
          //command [COMMAND_LENGTH] = {'\0'};

          while (reads < COMMAND_LENGTH) {
            char_read = file1.read();
            if ((char)char_read == ':' || (char)char_read == '\r'|| (char)char_read == '\n' || (char)char_read == '}' || char_read == -1 ) break;
            else {
              command[reads] = (char)char_read;
              reads++;
            }
          }
          //Serial.println();
          if (char_read == ':') {
            //char data_found[MAX_TWEET_SIZE] = {'\0'};
            //            data_found[MAX_TWEET_SIZE] = {'\0'};
            memset(data_found, '\0', MAX_TWEET_SIZE);
            reads = 0;
            while (reads < MAX_TWEET_SIZE) {
              char_read = file1.read();
              if ((char)char_read == '\r' || (char)char_read == '\n' ||  char_read == -1 ) break;
              else {
                data_found[reads] = (char)char_read;
                reads++;
              }
            }
            int value_found = atoi(data_found); //convert to int for some data types
            if (strcmp(command, STRING_FILE_COMMAND_STRING) == 0) {
              strncpy(text_str[i], data_found, sizeof(text_str[i]));
              text_parameters[i].text_str_length = reads;
              text_parameters[i].megas_up_to_date = false;
              text_cursor[i].object_used = true;  // set as true once string found, ignore if data provided but no string

            }
            else if (strcmp(command, STRING_FILE_COMMAND_RED) == 0)   {
              text_parameters[i].red = value_found;
              text_parameters[i].use_hue = false;

            }
            else if (strcmp(command, STRING_FILE_COMMAND_GREEN) == 0) {
              text_parameters[i].green = value_found;
              text_parameters[i].use_hue = false;
            }
            else if (strcmp(command, STRING_FILE_COMMAND_BLUE) == 0) {
              text_parameters[i].blue = value_found;
              text_parameters[i].use_hue = false;
            }
            else if (strcmp(command, STRING_FILE_COMMAND_HUE) == 0) {
              text_parameters[i].hue = value_found;
              text_parameters[i].use_hue = true;
            }
            else if (strcmp(command, STRING_FILE_COMMAND_SIZE) == 0)
              text_parameters[i].text_size = value_found;
            else if (strcmp(command, STRING_FILE_COMMAND_X_SPEED) == 0)
              text_cursor[i].x_pos_dir = value_found + 128;
            else if (strcmp(command, STRING_FILE_COMMAND_Y_SPEED) == 0)
              text_cursor[i].y_pos_dir = value_found + 128;
            else if (strcmp(command, STRING_FILE_COMMAND_X_START_POS) == 0) {
              text_cursor[i].x_start = value_found;
              x_start = true;
            }
            else if (strcmp(command, STRING_FILE_COMMAND_Y_START_POS) == 0) {
              text_cursor[i].y_start = value_found;
              y_start = true;
            }
            else if (strcmp(command, STRING_FILE_COMMAND_X_END_POS) == 0)  {
              text_cursor[i].x_end = value_found;
              x_end = true;
            }
            else if (strcmp(command, STRING_FILE_COMMAND_Y_END_POS) == 0)   {
              text_cursor[i].y_end = value_found;
              y_end = true;
            }
            else if (strcmp(command, STRING_FILE_COMMAND_NUM_LOOPS_X) == 0)   {
              if (value_found > 0) {
                text_cursor[i].loops_x = value_found;
                text_cursor[i].found_loops_x = true;

              }
            }
            else if (strcmp(command, STRING_FILE_COMMAND_NUM_LOOPS_Y) == 0)   {
              if (value_found > 0) {
                text_cursor[i].loops_y = value_found;
                text_cursor[i].found_loops_y = true;
              }
            }
            else if (strcmp(command, STRING_FILE_COMMAND_DISP_TIME) == 0)   {
              if (value_found > 0) {
                text_cursor[i].str_disp_time = value_found;
                text_cursor[i].found_time = true;
              }
            }
            else if (strcmp(command, STRING_FILE_COMMAND_NEXT_FILE) == 0)   {
              //Serial.println(reads);
              strncpy(SD_string.next_config_profile[i], SD_string.null_string, STRING_FILE_COMMAND_NEXT_FILE_NAME_LENGTH);
              strncpy(SD_string.next_config_profile[i], data_found, reads - 1);
              next_file_found = true;
              //Serial.println(SD_string.next_config_profile[i]);
            }
            else if (strcmp(command, STRING_FILE_COMMAND_SCREEN_MODE) == 0)   {
              screen_mode = value_found;
            }
          }
        }


        //        Serial.println(F("found obj end marker"));
        if (text_cursor[i].found_time || text_cursor[i].found_loops_y || text_cursor[i].found_loops_x) {
          text_cursor[i].check_for_new_file = true;
        }
        else
          text_cursor[i].check_for_new_file = false;


        text_cursor[i].x_start_set = x_start;  //drive to false if not found
        text_cursor[i].y_start_set = y_start;
        text_cursor[i].x_end_set = x_end;
        text_cursor[i].y_end_set = y_end;

        graphics.configure_limits(i);
        graphics.reset_position(i);
        i++;
      }
    }
    //  Serial.print(card1.pin);
    //  Serial.print("\t");
    //  Serial.println(card2.pin);
  }
  file1.close();

  //  Serial.print(card1_temp.pin);
  //  Serial.print("\t");
  //  Serial.println(card2_temp.pin);

  card1 = card1_temp; //fix this issue, see top of function
  card2 = card2_temp;

  //  Serial.print(card1.pin);
  //  Serial.print("\t");
  //  Serial.println(card2.pin);
  
//  ram_stats();
}


Card_LED::Card_LED() {
#ifdef ENABLE_CARD_LED

  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);

#ifdef CARD_LED_OUTPUT_INVERTING  //invert the values of each colour
  for (byte i = 0; i < 3; i++) {
    CARD_LED_RED[i] =  !CARD_LED_RED[i];
    CARD_LED_PURPLE[i] =  !CARD_LED_PURPLE[i];
    CARD_LED_GREEN[i] =  !CARD_LED_GREEN[i];
    CARD_LED_ORANGE[i] =  !CARD_LED_ORANGE[i];
    CARD_LED_BLUE[i] = !CARD_LED_BLUE[i];
  }

#endif
  memcpy(CARD_LED_READING, CARD_LED_RED, 3);
  memcpy(CARD_LED_MOUNTED, CARD_LED_BLUE, 3);
  memcpy(CARD_LED_UNMOUNTED, CARD_LED_GREEN, 3);
  memcpy(CARD_LED_CARD_NOT_FOUND, CARD_LED_ORANGE, 3);

#endif
}

void compare_colour(bool colour[3]) {

  if (colour[0] == CARD_LED_RED[0] && colour[1] == CARD_LED_RED[1] && colour[2] == CARD_LED_RED[2])
    Serial.println("red");
  else if (colour[0] == CARD_LED_PURPLE[0] && colour[1] == CARD_LED_PURPLE[1] && colour[2] == CARD_LED_PURPLE[2])
    Serial.println("purple");
  else if (colour[0] == CARD_LED_GREEN[0] && colour[1] == CARD_LED_GREEN[1] && colour[2] == CARD_LED_GREEN[2])
    Serial.println("green");
  else if (colour[0] == CARD_LED_ORANGE[0] && colour[1] == CARD_LED_ORANGE[1] && colour[2] == CARD_LED_ORANGE[2])
    Serial.println("orange");
  else if (colour[0] == CARD_LED_BLUE[0] && colour[1] == CARD_LED_BLUE[1] && colour[2] == CARD_LED_BLUE[2])
    Serial.println("blue");
}

void Card_LED::set_card_colour(bool colour[3]) {
#ifdef ENABLE_CARD_LED
  //compare_colour(colour);

  digitalWrite(red_pin, colour[0]);
  digitalWrite(green_pin, colour[1]);
  digitalWrite(blue_pin, colour[2]);

  //  Serial.println(colour[0]);
  //  Serial.println(colour[1]);
  //  Serial.println(colour[2]);
#endif
}





#endif // SD_Cards_CPP
