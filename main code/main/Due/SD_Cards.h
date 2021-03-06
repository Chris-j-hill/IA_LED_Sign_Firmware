#ifndef SD_Cards_H
#define SD_Cards_H

//#include "Due_Libraries.h"
#include "Config_Local.h"
#include "function_declarations.h"    //any functions that need prototypes
#include "Due_Pins.h"

#include "src/Due_Sd_Cards/SdFat/SdFat.h"
#include "src/Due_Sd_Cards/SdFat/SdFatUtil.h"

#if !USE_MULTIPLE_CARDS
#error You must set USE_MULTIPLE_CARDS nonzero in SdFatConfig.h
#endif



#define set_as_active() chvol()



#define NETWORK_INFO_NAME_LENGTH 25 //number of bytes to use when parsing files for network info
#define MAX_NETWORK_NAME_LENGTH               NETWORK_INFO_NAME_LENGTH
#define MAX_NETWORK_PASSWORD_LENGTH           NETWORK_INFO_NAME_LENGTH
#define MAX_DEFAULT_NO_NETWORK_STRING_LENGTH  2*NETWORK_INFO_NAME_LENGTH


//#define ENCRYPT_LOCAL           // encrypt copied data
#define DISABLE_EXTERNAL_PORT     // disable external port once data copied, wont log to external or allow user to select new string file (without remounting card through menu or otherwise)

//#define ALLOW_CONFIG_CHAINING   // allow strings profiles to be chained together, ie display one for specified time then move to next


#define ALLOW_INSTRUCTION_FILES
#define ALLOW_BITMAP_FILES
#define ALLOW_CALIBRATION_FILES
#define ALLOW_DATA_LOG_FILES
#define ALLOW_DISP_STRING_FILES
#define ALLOW_NETWORK_FILES

#define EXTERNAL_SD_CARD_DIRECTORY_NAME "/EXTERNAL"
#define INTERNAL_SD_CARD_DIRECTORY_NAME "/INTERNAL"

#define  EXT_NETWORK_FILE "NetworksE.BIN"
#define  INT_NETWORK_FILE "NetworksI.BIN"

#define EXT_STRING_FILE "StringE.BIN"
#define INT_STRING_FILE "StringI.BIN"

#define EXT_LOG_FILE "DataLogE.CSV"
#define INT_LOG_FILE "DataLogI.CSV"

#define EXT_CALIBRATION_FILE "CalE.BIN"
#define INT_CALIBRATION_FILE "CalI.BIN"

#define EXT_INSTRUCTION_FILE "InstrE.BIN"
#define INT_INSTRUCTION_FILE "InstrI.BIN"

#define EXT_BITMAP_FILE "bitmapE.BIN"
#define INT_BITMAP_FILE "bitmapI.BIN"



#define CHECK_EXTERNAL_SD_CARD_PERIOD 500
#define CHECK_INTERNAL_SD_CARD_PERIOD 1000 //much less likely to be inserted/removed, check less frequencly

#define INTERNAL_CARD 1
#define EXTERNAL_CARD 2
#define RASP_PI 3


#define LOGGING_PERIOD 500

#define COMMAND_LENGTH 20 //max length of the commands used in the string file
#define CONFIG_PROFILE_NAME_LENGTH 20// max size of config profile name


#define STRING_FILE_COMMAND_STRING          "String"
#define STRING_FILE_COMMAND_RED             "Red"
#define STRING_FILE_COMMAND_GREEN           "Green"
#define STRING_FILE_COMMAND_BLUE            "Blue"
#define STRING_FILE_COMMAND_HUE             "Hue"
#define STRING_FILE_COMMAND_SIZE            "Size"
#define STRING_FILE_COMMAND_X_SPEED         "X Speed"
#define STRING_FILE_COMMAND_Y_SPEED         "Y Speed"
#define STRING_FILE_COMMAND_X_START_POS     "X Start Pos"
#define STRING_FILE_COMMAND_Y_START_POS     "Y Start Pos"
#define STRING_FILE_COMMAND_X_END_POS       "X End Pos"
#define STRING_FILE_COMMAND_Y_END_POS       "Y End Pos"
#define STRING_FILE_COMMAND_NUM_LOOPS_X     "X Loops"
#define STRING_FILE_COMMAND_NUM_LOOPS_Y     "Y Loops"
#define STRING_FILE_COMMAND_DISP_TIME       "Disp time"
#define STRING_FILE_COMMAND_NEXT_FILE       "Next File"   //<- define file to read after defined loops is completed
#define STRING_FILE_COMMAND_SCREEN_MODE     "Screen Mode"


#define STRING_FILE_COMMAND_NEXT_FILE_NAME_LENGTH 20


#define CALIBRATION_FILE_COMMAND_FAN_ENABLED          "fan enabled"
#define CALIBRATION_FILE_COMMAND_TEMP1_ENABLED        "temp1 enabled"
#define CALIBRATION_FILE_COMMAND_TEMP2_ENABLED        "temp2 enabled"
#define CALIBRATION_FILE_COMMAND_TEMP3_ENABLED        "temp3 enabled"
#define CALIBRATION_FILE_COMMAND_STRIP_ENABLED        "strip enabled"
#define CALIBRATION_FILE_COMMAND_CURRENT1_ENABLED     "current1 enabled"
#define CALIBRATION_FILE_COMMAND_CURRENT2_ENABLED     "current2 enabled"
#define CALIBRATION_FILE_COMMAND_LDR1_ENABLED         "ldr1 enabled"
#define CALIBRATION_FILE_COMMAND_LDR2_ENABLED         "ldr2 enabled"
#define CALIBRATION_FILE_COMMAND_ENCODER_ENABLED      "encoder enabled"
#define CALIBRATION_FILE_COMMAND_BUTTON_ENABLED       "button enabled"
#define CALIBRATION_FILE_COMMAND_SD_CARD1_ENABLED     "card1 enabled"   //<- careful now
#define CALIBRATION_FILE_COMMAND_SD_CARD2_ENABLED     "card2 enabled"

#define CALIBRATION_FILE_COMMAND_FAN_MIN              "fan min"
#define CALIBRATION_FILE_COMMAND_FAN_SPEED            "fan speed"
#define CALIBRATION_FILE_COMMAND_FAN_INCREMENT        "fan increment"
#define CALIBRATION_FILE_COMMAND_FAN_INTERVAL         "fan interval"

#define CALIBRATION_FILE_COMMAND_STRIP_BRIGHTNESS     "strip birghtness"
#define CALIBRATION_FILE_COMMAND_STRIP_MINUMUM        "strip min"
#define CALIBRATION_FILE_COMMAND_STRIP_INCREMENT      "strip increment"
#define CALIBRATION_FILE_COMMAND_STRIP_INTERVAL       "fan interval"

#define CALIBRATION_FILE_COMMAND_ENCODER_SENSITIVITY  "encoder sensitivity"

#define CALIBRATION_FILE_COMMAND_LDR_CONFIG_MAX1      "ldr max 1"
#define CALIBRATION_FILE_COMMAND_LDR_CONFIG_MAX2      "ldr max 2"
#define CALIBRATION_FILE_COMMAND_LDR_CONFIG_MIN1      "ldr min 1"
#define CALIBRATION_FILE_COMMAND_LDR_CONFIG_MIN2      "ldr min 2"

#define CALIBRATION_FILE_DATA_CHAR_LENGTH 10

#define CARD_LED_OUTPUT_INVERTING

struct SD_Strings {

  String str_sd = "123456789 123456789 123456789 123456789 123456789 ";  //<- 60 bytes, sample default string, needed to set the length correctly
  String Network = "init network";    //store current network
  String Password = "init password";   //store current password
  char next_config_profile[MAX_NUM_OF_TEXT_OBJECTS][STRING_FILE_COMMAND_NEXT_FILE_NAME_LENGTH] = {{'\0'}};
  char null_string[STRING_FILE_COMMAND_NEXT_FILE_NAME_LENGTH] = {'\0'}; //array of nulls to clear with
};

struct SD_Card {
  byte pin = 0;
  bool enabled = false;
  bool detected = false;
  const char *working_dir = "123456789 123456789 "; // <- allow 20 characters for working dir name
  bool directory_exists = false;

  bool network_file_exists = false;       //types of files that we might work with
  bool disp_string_file_exists = false;
  bool log_file_exists = false;
  bool instruction_file_exists = false;
  bool calibration_file_exists = false;
  bool bitmap_file_exists = false;
};




class Card {

  private:

    void check_for_files(byte check_card);
    void copy_file(String from_string, String to_string, byte from_device, byte to_device); //copy file to multiple places so as to retain backups of data
    void copy(byte from_device, byte to_device);
    void files_dont_exist(byte device);   //set all file_exists variables to false when device connection lost

    void retrieve_string(String filename, byte obj_num, bool get_next_config);

    void log_data(String filename, bool truncate = false, bool print_header = false);

  public:

    Card();

    void check_for_sd_card(); //check if can .begin

    void init_sd_cards();


    int og_init_sd_cards();
    int check_for_SD_card_inserted();

    void update_data_log(byte give_priority_to = 0);

    void safely_eject_card(byte card_num);
    void mount_card(byte card_num);
    void retrieve_data(String filename, byte obj_num = 0, bool get_next_config = false);

};


class Card_LED {

  private:

    byte red_pin = SD_LED_RED_PIN;
    byte green_pin = SD_LED_GREEN_PIN;
    byte blue_pin = SD_LED_BLUE_PIN;


  public:
    Card_LED();

    void set_card_colour(bool colour[3]);


};

#endif  // SD_Cards_H
