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


#define CHECK_EXTERNAL_SD_CARD_PERIOD 1000
#define CHECK_INTERNAL_SD_CARD_PERIOD 5000 //much less likely to be inserted/removed, check less frequencly

#define INTERNAL_CARD 1     
#define EXTERNAL_CARD 2
#define RASP_PI 3


#define COPY_BUF_SIZE 100


#define LOGGING_PERIOD 500

struct SD_Strings {

  String str_sd = "123456789 123456789 123456789 123456789 123456789 ";  //<- 60 bytes, sample default string, needed to set the length correctly
  String Network = "init network";    //store current network
  String Password = "init password";   //store current password

};

struct SD_Card {
  byte pin = 0;
  bool enabled = false;
  bool detected = false;
  const char *working_dir = "123456789 123456789 "; // <- 20 characters for working dir name

  bool directory_exists = false;
  
  byte network_file_exists = false;       //types of files that we might work with
  byte disp_string_file_exists = false;
  byte log_file_exists = false;
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
