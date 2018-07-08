#ifndef SD_Cards_H
#define SD_Cards_H

//#include "Due_Libraries.h"
#include "Config_Local.h"
#include "function_declarations.h"    //any functions that need prototypes
#include "Due_Pins.h"

#include "libs/sdfatlib/SdFat/SdFat.h"
#include "libs/sdfatlib/SdFat/SdFatUtil.h"

#if !USE_MULTIPLE_CARDS
#error You must set USE_MULTIPLE_CARDS nonzero in SdFatConfig.h
#endif


// print error msg, any SD error codes, and halt.
// store messages in flash
#define errorExit(msg) errorHalt_P(PSTR(msg))
#define initError(msg) initErrorHalt_P(PSTR(msg))

#define set_as_active() chvol()



#define NETWORK_INFO_NAME_LENGTH 25 //number of bytes to use when parsing files for network info
#define MAX_NETWORK_NAME_LENGTH               NETWORK_INFO_NAME_LENGTH
#define MAX_NETWORK_PASSWORD_LENGTH           NETWORK_INFO_NAME_LENGTH
#define MAX_DEFAULT_NO_NETWORK_STRING_LENGTH  2*NETWORK_INFO_NAME_LENGTH


#define WAIT_TIME_FOR_SD_ON_STARTUP 40000 // wait this long before stopping code

#ifndef EXPECTS_SD_ON_STARTUP
#define DEFAULT_STARTUP_STRING_NO_SD_CARD "Not searching for sd card, heres some text instead"
#endif

//#define ENCRYPT_LOCAL           // encrypt copied data
#define DISABLE_EXTERNAL_PORT   // disable external port once data copied



#define ALLOW_INSTRUCTION_FILES
#define ALLOW_BITMAP_FILES
#define ALLOW_CALIBRATION_FILES
#define ALLOW_DATA_LOG_FILES
#define ALLOW_DISP_STRING_FILES
#define ALLOW_NETWORK_FILES

#define  EXT_NETWORK_FILE NETWORK_LOGIN_FILENAME
#define  INT_NETWORK_FILE NETWORK_LOGIN_FILENAME

#define EXT_STRING_FILE "String.BIN"
#define INT_STRING_FILE "String.BIN"

#define EXT_LOG_FILE "DataLog.CSV"
#define INT_LOG_FILE "DataLog.CSV"

#define EXT_CALIBRATION_FILE "Cal.BIN"
#define INT_CALIBRATION_FILE "Cal_int.BIN"

#define EXT_INSTRUCTION_FILE "Instruct.BIN"
#define INT_INSTRUCTION_FILE "Instruct.BIN"

#define EXT_BITMAP_FILE "bitmap.BIN"
#define INT_BITMAP_FILE "bitmap.BIN"



#define CHECK_EXTERNAL_SD_CARD_PERIOD 500
#define CHECK_INTERNAL_SD_CARD_PERIOD 1000 //much less likely to be inserted/removed, check less frequencly

#define INTERNAL_CARD 1
#define EXTERNAL_CARD 2
#define RASP_PI 3


#define COPY_BUF_SIZE 100


#define LOGGING_PERIOD 500

#define COMMAND_LENGTH 30 //max length of the commands used in the string file

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


#define STRING_FILE_COMMAND_NEXT_FILE_NAME_LENGTH 30


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



struct SD_Strings {

  String str_sd = "123456789 123456789 123456789 123456789 123456789 ";  //<- 60 bytes, sample default string, needed to set the length correctly
  String Network = "init network";    //store current network
  String Password = "init password";   //store current password
  char next_file[STRING_FILE_COMMAND_NEXT_FILE_NAME_LENGTH] = {'\0'};
  char null_string[STRING_FILE_COMMAND_NEXT_FILE_NAME_LENGTH] = {'\0'}; //array of nulls to clear with
};

struct SD_Card {
  byte pin = 0;
  bool enabled = false;
  bool detected = false;
  const char *working_dir = "123456789 123456789 "; // <- 20 characters for working dir name

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


    int copy_sd_data(const char *ext_file, const char *int_file, const char *ext_dir, const char *int_dir);
    int extract_network_data();
    int buffer_in_header();         // return 1 if the colon is in the header, otherwise 0
    int buffer_is_network();         // check if network stored in buffer
    int buffer_is_password();        // check if password stored in buffer
    int buffer_is_default();        // check if default string stored in buffer
    int string_length();      // calculate the length of the relevent string in file
    int remove_card_1();
    int log_temp_to_sd();               // log data to the sd card data related to temps over time



    void check_for_files(byte check_card);
    void copy_file(String from_string, String to_string, byte from_device, byte to_device); //copy file to multiple places so as to retain backups of data
    void copy(byte from_device, byte to_device);
    void files_dont_exist(byte device);   //set all file_exists variables to false when device connection lost

    
        
    void log_data(String filename, bool truncate = false, bool print_header = false);

  public:

    Card();

    void check_for_sd_card(); //check if can .begin

    void init_sd_cards();


    int og_init_sd_cards();
    int check_for_SD_card_inserted();

    void update_data_log(byte give_priority_to = 0);

    void safely_eject_card(byte card);
    void mount_card(byte card);
    void retrieve_data(String filename);

};


#endif  // SD_Cards_H
