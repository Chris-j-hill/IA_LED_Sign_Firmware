#ifndef SD_Cards_H
#define SD_Cards_H

#include "Due_Libraries.h"

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

//sd card objects,
SdFat external_sd_card;            //external sd card
SdFat internal_sd_card;            //onboard sd card

SdFile file1;                 // file handling objects, use two objects to address both open files at once
SdFile file2;                 // nb: if modifying files, make sure to close at the end of function

const uint8_t SD2_CS = SD1_ENABLE;   // chip select for internal_sd_card
const uint8_t SD1_CS = SD2_ENABLE;  // chip select for external_sd_card

const uint8_t SD_FILE_COPY_BUF_SIZE = 100;
uint8_t sd_file_copy_buffer[SD_FILE_COPY_BUF_SIZE];

char sd_file_read_buffer[67];       //buffer to read some data, dont need to read whole file at once, and doing so could be problematic if file large,
//read 15 bytes to recognise id word (eg Network) and have 50 bytes for string (default could be long) and two for \n type characters

// put sd card file name strings here:

const char *sd_ext_dir = EXTERNAL_SD_CARD_DIRECTORY_NAME;
const char *sd_int_dir = INTERNAL_SD_CARD_DIRECTORY_NAME;

const char *sd_ext_file = NETWORK_LOGIN_FILENAME;
const char *sd_int_file = NETWORK_LOGIN_FILENAME;

//const char *sd_ext_file2 = "Instructions.BIN";      //not implemented
//const char *sd_int_file2 = "Instructions.BIN";
//
//const char *sd_ext_file3 = "bitmap.BIN";
//const char *sd_int_file3 = "bitmap.BIN";

struct SD_Strings{
  
String str_sd ="123456789 123456789 123456789 123456789 123456789 ";   //<- 60 bytes, sample default string, needed to set the length correctly
String Network = "init network";    //store current network
String Password = "init password";   //store current password
 
};
SD_Strings SD_string;

bool sd_card1_detected = true;    //display these parameters, update with check_for_SD_card_inserted()
bool sd_card2_detected = true;

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

  public:

    Card() {}

    int init_sd_cards();
    int check_for_SD_card_inserted();
      //print the currently stored frame to the serial monitor
};


#endif  // SD_Cards_H
