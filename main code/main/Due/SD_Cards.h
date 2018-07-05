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
#define INT_CALIBRATION_FILE "Cal.BIN"

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

#define DISP_STRING_COMMAND_LENGTH 12 //max length of the commands used in the string file

#define STRING_FILE_COMMAND_STRING "String"
#define STRING_FILE_COMMAND_RED "Red"
#define STRING_FILE_COMMAND_GREEN "Green"
#define STRING_FILE_COMMAND_BLUE "Blue"
#define STRING_FILE_COMMAND_HUE "Hue"
#define STRING_FILE_COMMAND_SIZE "Size"
#define STRING_FILE_COMMAND_X_SPEED "X Speed"
#define STRING_FILE_COMMAND_Y_SPEED "Y Speed"
#define STRING_FILE_COMMAND_X_START_POS "X Start Pos"
#define STRING_FILE_COMMAND_Y_START_POS "Y Start Pos"
#define STRING_FILE_COMMAND_X_END_POS "X End Pos"
#define STRING_FILE_COMMAND_Y_END_POS "X End Pos"
#define STRING_FILE_COMMAND_NUM_LOOPS "Loops"
#define STRING_FILE_COMMAND_DISP_TIME "Disp time"
 
#define STRING_FILE_COMMAND_NEXT_FILE "Next File"   //<- define file to read after defined loops is completed


struct SD_Strings {

  String str_sd = "123456789 123456789 123456789 123456789 123456789 ";  //<- 60 bytes, sample default string, needed to set the length correctly
  String Network = "init network";    //store current network
  String Password = "init password";   //store current password
  char next_file[30] = {'\0'};
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
    void copy_file(const char *from_filename, const char *to_filename, byte from_device, byte to_device); //copy file to multiple places so as to retain backups of data
    void copy(byte from_device, byte to_device);
    void files_dont_exist(byte device);   //set all file_exists variables to false when device connection lost

    void retrieve_data(String filename);
        
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

};


#endif  // SD_Cards_H
