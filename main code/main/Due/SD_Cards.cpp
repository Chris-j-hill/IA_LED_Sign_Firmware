#ifndef SD_Cards_CPP
#define SD_Cards_CPP

#include "SD_Cards.h"
//#include "Due.h"
#include "Coms.h"
#include "Fans.h"
#include "Current_Control.h"
#include "Graphics.h"


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




SD_Card card1;
SD_Card card2;

extern char text_str[MAX_TWEET_SIZE];

extern struct Fan_Struct fan_parameters;
extern struct Temp_sensor temp_parameters;
extern struct LDR_Struct light_sensor_parameters;
extern struct Current_Meter_Struct current_meter_parameters;
extern struct Text text_parameters;
extern struct Text_cursor text_cursor;


extern Graphics graphics;

extern byte screen_brightness;



Card::Card() {
  card1.pin = SD1_ENABLE;
  card2.pin = SD2_ENABLE;

  card1.enabled = enable_sd_cards;
  card2.enabled = enable_sd_cards;

  //card1.working_dir = EXTERNAL_SD_CARD_DIRECTORY_NAME;
  card1.working_dir = INTERNAL_SD_CARD_DIRECTORY_NAME;    //change this line when using two cards
  card2.working_dir = INTERNAL_SD_CARD_DIRECTORY_NAME;
}

int Card::og_init_sd_cards() {      // code to init sd cards and copy data from external to internal card

  // code for initialising sd cards, waits for both cards to be active then copies files from
  // the external one to the internal one. listas are made of all files in directory and errors are
  // thrown if files or directories cant be accessed.
  // TO DO: 1) copy all network info into array, will need to use maloc or stupidly big array to guarantee all data copied
  //        2) maybe look into copying more files (for instruction set, bitmap images etc)

  pinMode(SD1_CS, OUTPUT);

  Sprintln(F("\t Sd init setup..."));
  bool external_sd_card_initialised = false;
  bool internal_sd_card_initialised = false;

  PgmPrint("\t FreeRam: ");

  Sprintln(FreeRam());

  // fill buffer with known data
  for (int i = 0; i < sizeof(sd_file_copy_buffer); i++) sd_file_copy_buffer[i] = i;

  // disable internal_sd_card while initializing external_sd_card
  pinMode(SD2_CS, OUTPUT);
  digitalWrite(SD2_CS, HIGH);


  // initialize the external card
  int alpha = 0;
  int beta = millis();


  while (millis() < beta + WAIT_TIME_FOR_SD_ON_STARTUP && external_sd_card_initialised == 0) {    //wait for 40 seconds or until card inserted
    if (external_sd_card.begin(SD1_CS)) {    //if card initialised sucessfully exit wait loop
      external_sd_card_initialised = true;              //loop exit conditions if card detected
    }
#ifdef DEBUG
    else {          //flash led and print prompt
      if (alpha % 50 == 0) {

        Sprintln(F("\t Insert external sd card"));

      }
      delay(20);
      alpha++;
      //send_frame... //display error on screen
    }
#endif
  }

  if (!external_sd_card_initialised) {
    Sprintln(F("\t No SD card detected"));
    external_sd_card.initError("external_sd_card:");    //stops code when called
  }


  // create SIGN1 on external_sd_card if it does not exist
  // Use this to test if card is inserted, if directory can't be made, functional card not inserted (or wiring problem..., presume card)
  beta = millis();
  external_sd_card_initialised = false;
  alpha = 0;

  while (millis() < beta + 5000 && external_sd_card_initialised == false) {    // make sure directory exists on external card, may take a moment, if longer than 5 seconds then error

    if (!external_sd_card.exists(sd_ext_dir)) {
      if (alpha % 50 == 0) Sprintln(F("\t External Directory does not exist"));
      if (!external_sd_card.mkdir(sd_ext_dir)) {
        if (alpha % 50 == 0) Sprintln(F("\t Can't create External Directory, ensure card works and in write mode"));
        if (alpha == 20) Sprintln(F("\t Please insure card inserted properly"));
        delay(20);
      }
    }
    else external_sd_card_initialised = true;    //folder exists, exit loop
    alpha++;
  }

  if (!external_sd_card_initialised) {
    external_sd_card.errorExit("external_sd_card.mkdir");   //while(1)...
  }


  //repeat some process for internal card
  beta = millis();
  alpha = 0;

  while (millis() < beta + 10000 && internal_sd_card_initialised == 0) {    //wait until card initialised correctly, shorter time to insert as it should be in all the time
    if (internal_sd_card.begin(SD2_CS)) {
      internal_sd_card_initialised = true;              //loop exit conditions if card detected
    }
    else {
      if (alpha % 100 == 0)
        Sprintln(F("\t Insert internal sd card "));   //promt for user
      //send_frame... //display error on screen
      delay(20);
      alpha++;
    }
  }

  if (!internal_sd_card_initialised)
    internal_sd_card.initError("internal_sd_card:");    //stops code when called

  // create SIGN2 on internal_sd_card if it does not exist
  if (!internal_sd_card.exists(sd_int_dir)) {
    if (!internal_sd_card.mkdir(sd_int_dir)) internal_sd_card.errorExit("internal_sd_card.mkdir");
  }

#if defined (DEBUG)
  // list root directory on both cards
  Sprintln("\t ------external_sd_card root-------");
  external_sd_card.ls();
  Sprintln("\t ------internal_sd_card root-------");
  internal_sd_card.ls();
#endif

  // make /SIGN1 the default directory for external_sd_card
  if (!external_sd_card.chdir(sd_ext_dir) && external_sd_card_initialised) external_sd_card.errorExit("external_sd_card.chdir");

  // make /SIGN2 the default directory for internal_sd_card
  if (!internal_sd_card.chdir(sd_int_dir) && internal_sd_card_initialised) internal_sd_card.errorExit("internal_sd_card.chdir");

  // list current directory on both cards
#if defined(DEBUG)
  Sprintln("\t ------external_sd_card SIGN1-------");
  external_sd_card.ls();
  Sprintln("\t ------internal_sd_card SIGN2-------");
  internal_sd_card.ls();
  Sprintln("\t ---------------------");
#endif

  copy_sd_data(sd_ext_file, sd_int_file, sd_ext_dir, sd_int_dir);

}

int Card::copy_sd_data(const char *ext_file, const char *int_file, const char *ext_dir, const char *int_dir) {   //funtion to copy the data from /ext_dir/ext_file to /int_dir/int_file


  // set the current working directory for open() to external_sd_card
  external_sd_card.chvol();

  //  // make /SIGN1 the default directory for external_sd_card    //shouldnt need to call these but do anyway to be sure
  //  if (!external_sd_card.chdir(ext_dir)) external_sd_card.errorExit("external_sd_card.chdir");
  //
  //  // make /SIGN2 the default directory for internal_sd_card
  //  if (!internal_sd_card.chdir(int_dir)) internal_sd_card.errorExit("internal_sd_card.chdir");



  // create or open /ext_dir/ext_file and truncate it to zero length

  if (!file1.open(ext_file, O_RDWR | O_CREAT )) {
    external_sd_card.errorExit("error opening external sd file");
  }

  // set the current working directory for open() to internal_sd_card
  internal_sd_card.chvol();

  // create or open /int_dir/int_file and truncate it to zero length

  if (!file2.open(int_file, O_WRITE | O_CREAT | O_TRUNC)) {
    internal_sd_card.errorExit("error opening internal sd file");
  }
  Sprintln("\t Copying");

  // copy file1 to file2
  file1.rewind();
  uint32_t t = millis();

  while (1) {
    int n = file1.read(sd_file_copy_buffer, sizeof(sd_file_copy_buffer));
    if (n < 0) external_sd_card.errorExit("error reading external file");
    if (n == 0) break;
    if (file2.write(sd_file_copy_buffer, n) != n) internal_sd_card.errorExit("error writing internal file");

  }


  t = millis() - t;
  Sprint("\t File size: ");
  Sprintln(file2.fileSize());
  Sprint("\t Copy time: ");
  Sprint(t);
  Sprintln(" millis");


  // close files
  file1.close();

  file2.close();

  Sprintln("\t Done Transfer\n");

}

int Card::extract_network_data() {   // parse the file and extract network info (test network login not implmented yet 28/7/17)
  // function to extract data from the sd card on the ethernet shield, passwords should already be copied here when this is called
  // network info is identifiable the keywords network, password and default, the associated string is identifiable by a tab or return carrige at their end
  // default is a string to be taken hould no known network be available


  char temp_password[MAX_NETWORK_PASSWORD_LENGTH];   //temp arrays to copy data into if found, makes converting from array of lots of data to string of only selected data easier
  char temp_network[MAX_NETWORK_NAME_LENGTH];
  char temp_default [MAX_DEFAULT_NO_NETWORK_STRING_LENGTH];

  int str_len = 0;
  bool Connected = false;     //value to define if ethernet/wifi connected with current network and password, if true stop looping through file
  int n = 1;                  //define n as non zero
  int alpha = 0;
  int beta = 0;

  SD_string.str_sd.reserve(150);

  internal_sd_card.chvol();

  if (!file2.open("TEST2.BIN", O_READ)) {       //open file for reading
    Sprintln(F("Can't open file to extract data"));
    return (-1);
  }

  while (!Connected && n != 0) {  //while Connected not set to 1 and file end no reached

    file2.seekSet(alpha);       //set cursor position, and increment by one on each loop (probably better way to do this)
    alpha++;
    n = file2.read(sd_file_read_buffer, sizeof(sd_file_read_buffer));   //returns 0 if file finished, negative if error
    if (n < 0) internal_sd_card.errorExit("read2");

    if (sd_file_read_buffer[14] == ':') {   // if it equals a colon it could be a password or network
      Sprintln(F("colon found"));
      if (buffer_in_header())  Sprintln(F("colon in file header"));

      else {
        if (buffer_is_network()) {  //if valid, check if connection available
          str_len = string_length();
          for (beta = 0; beta < str_len; beta++) { //loop through network name
            SD_string.Network[beta] = sd_file_read_buffer[beta + 16]; // offset to where string starts
          }
          SD_string.Network.remove(beta);
        }

        else if (buffer_is_password()) {
          str_len = string_length();
          for (beta = 0; beta < str_len; beta++) { //loop through network name
            SD_string.Password[beta] = sd_file_read_buffer[beta + 16]; // offset to where string starts
          }
          SD_string.Password.remove(beta);

          // attempt to connect to network using network and password...
          //wifi_connect....
          //ethernet_connect...
        }

        else if (buffer_is_default()) {
          str_len = string_length();
          beta = 0;
          while (beta != str_len) { //loop through default string
            SD_string.str_sd[beta] = sd_file_read_buffer[beta + 16]; // offset to where string starts
            Sprint(SD_string.str_sd[beta]);
            beta++;
          }
          Sprintln("");
          SD_string.str_sd.remove(str_len);
        }
      }
    }
  }
  file2.close();
  if (Connected) {
    SD_string.str_sd = "Connected to network";    //set string as something for clarity
    SD_string.str_sd.remove(SD_string.str_sd.length());      //remove excess characters of string
  }
}

int Card::buffer_in_header() {      //return 1 if the colon is in the header, otherwise 0

  if (sd_file_read_buffer[15] == '"')
    return 1;

  if (sd_file_read_buffer[16] == 'E' && sd_file_read_buffer[17] == 'x' && sd_file_read_buffer[18] == 'a' && sd_file_read_buffer[19] == 'm' && sd_file_read_buffer[20] == 'p' && sd_file_read_buffer[21] == 'l' && sd_file_read_buffer[22] == 'e' )//&& (sd_file_read_buffer[23] == '\n' || sd_file_read_buffer[23] == '\t'))
    return 1;

  return 0;
}

int Card::buffer_is_network() {     //check if network stored in buffer
  if (sd_file_read_buffer[7] == 'N' && sd_file_read_buffer[8] == 'e' && sd_file_read_buffer[9] == 't' && sd_file_read_buffer[10] == 'w' && sd_file_read_buffer[11] == 'o' && sd_file_read_buffer[12] == 'r' && sd_file_read_buffer[13] == 'k')
    return 1;
  return 0;

}

int Card::buffer_is_password() {   //check if password stored in buffer
  if (sd_file_read_buffer[6] == 'P' && sd_file_read_buffer[7] == 'a' && sd_file_read_buffer[8] == 's' && sd_file_read_buffer[9] == 's' && sd_file_read_buffer[10] == 'w' && sd_file_read_buffer[11] == 'o' && sd_file_read_buffer[12] == 'r' && sd_file_read_buffer[13] == 'd')
    return 1;
  return 0;
}

int Card::buffer_is_default() {    //check if default string stored in buffer
  if (sd_file_read_buffer[7] == 'D' && sd_file_read_buffer[8] == 'e' && sd_file_read_buffer[9] == 'f' && sd_file_read_buffer[10] == 'a' && sd_file_read_buffer[11] == 'u' && sd_file_read_buffer[12] == 'l' && sd_file_read_buffer[13] == 't')
    return 1;
  return 0;
}

int Card::string_length() {  // get string length, data strings seperated by tab or return carrige at end
  int alpha = 16;
  while (sd_file_read_buffer[alpha] != '\n' && sd_file_read_buffer[alpha] != '\t') {  //if the current index is a carriage return exit
    alpha++;    //increment alpha
  }
  return alpha - 16 - 1; //alpha-offset-1
}

int Card::remove_card_1() {  //function to stall code until the external sd is removed, safety to prevent sd containing network info left unattended

  // function to prevent standard operation of the display until external sd card removed.

  // TO DO, currently the code stalls indefinitely while waiting for the sd card to be removed, maybe add a timer interrupt and enter a restricted operation mode

  int alpha = 0;
  int theta = millis();
  static bool external_sd_card_initialised = false;

  while (external_sd_card_initialised == 0) {
    if (!external_sd_card.begin(SD1_CS)) {
      external_sd_card_initialised = true;              //loop exit conditions if card detected
    }
#ifdef DEBUG
    else {
      if (alpha == 10) {            //prompt
        Sprintln(F("Remove sd card 1"));
        //send frame to matrix...
      }
      delay(50);
      alpha++;
    }

#endif
  }
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

  if (millis() > external_sd_card_prev_read_time + (card1.detected ? 5 * CHECK_EXTERNAL_SD_CARD_PERIOD : CHECK_EXTERNAL_SD_CARD_PERIOD)) { //if card detected, reduce polling frequency, only need quick response on insertion

    external_sd_card_prev_read_time = millis();
    if (external_sd_card.begin(card1.pin) && !card1.detected) {
      card1.detected = true;
      check_for_files(EXTERNAL_CARD);  //check if files exist on external card
      if (card1.network_file_exists) {  //if file exists
        if (card2.detected) {}            //attempt to update to newer version
        copy_file(EXT_NETWORK_FILE, INT_NETWORK_FILE, EXTERNAL_CARD , INTERNAL_CARD );
        retrieve_data(EXT_NETWORK_FILE);//get contents
      }
      if (card1.disp_string_file_exists) {
        if (card2.detected) {}
        copy_file(EXT_STRING_FILE, INT_STRING_FILE, EXTERNAL_CARD , INTERNAL_CARD );
        retrieve_data(EXT_STRING_FILE);
      }

    }
    else if (!external_sd_card.begin(card1.pin) && card1.detected) { //card was previously detected but not initialising now
      card1.detected = false;
      files_dont_exist(EXTERNAL_CARD);
    }
  }

  if (millis() > internal_sd_card_prev_read_time + (card2.detected ? 5 * CHECK_INTERNAL_SD_CARD_PERIOD : CHECK_INTERNAL_SD_CARD_PERIOD)) {

    internal_sd_card_prev_read_time = millis();
    if (internal_sd_card.begin(card2.pin) && !card2.detected) {
      card2.detected = true;
      check_for_files(INTERNAL_CARD);  //check if files exist on external card
      if (card2.network_file_exists) {
        //if (card1.detected){}
        //copy_file(EXT_NETWORK_FILE, INT_NETWORK_FILE, EXTERNAL_CARD , INTERNAL_CARD );
        retrieve_data(INT_NETWORK_FILE);
      }
      if (card2.disp_string_file_exists) {
        //if (card1.detected){}
        //copy_file(EXT_STRING_FILE, INT_STRING_FILE, EXTERNAL_CARD , INTERNAL_CARD );
        retrieve_data(INT_STRING_FILE);
      }

    }
    else if (!internal_sd_card.begin(card2.pin, SPI_HALF_SPEED) && card2.detected) {
      card2.detected = false;
      files_dont_exist(INTERNAL_CARD);
    }
  }
}

void Card::check_for_files(byte card_to_check) {

  if (card_to_check == EXTERNAL_CARD) {//checking external card

    //check if dir exists
    if (external_sd_card.exists(card1.working_dir)) {
      card1.directory_exists = true;
      external_sd_card.chdir(card1.working_dir); //open dir if exists

#ifdef ALLOW_NETWORK_FILES
      if (external_sd_card.exists(EXT_NETWORK_FILE))
        card1.network_file_exists = true;
      else
        card1.network_file_exists = false;
#endif

#ifdef ALLOW_DISP_STRING_FILES
      if (external_sd_card.exists(EXT_STRING_FILE))
        card1.disp_string_file_exists = true;
      else
        card1.disp_string_file_exists = false;
#endif

#ifdef ALLOW_DATA_LOG_FILES   //make the log file if it doesnt exist
      if (external_sd_card.exists(EXT_LOG_FILE)) {
        card1.log_file_exists = file1.open(EXT_LOG_FILE, O_WRITE | O_CREAT);
        file1.write("DataLog.BIN");
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

    if (!internal_sd_card.exists(card2.working_dir)) {
      card2.directory_exists = internal_sd_card.mkdir(card2.working_dir); //make dir if does not exist
    }
    else
      card2.directory_exists = true;

    if (card2.directory_exists) {                // if dir exists
      internal_sd_card.chdir(card2.working_dir); // open dir

#ifdef ALLOW_NETWORK_FILES
      if (!internal_sd_card.exists("Networks.BIN")) {
        card2.network_file_exists = file2.open("Networks.BIN", O_WRITE | O_CREAT );
        file2.write("Networks.BIN");
      }
      else
        card2.network_file_exists = true;
      file2.close();
#endif

#ifdef ALLOW_DISP_STRING_FILES
      if (!internal_sd_card.exists(INT_STRING_FILE)) {
        card2.disp_string_file_exists = file2.open(INT_STRING_FILE, O_WRITE | O_CREAT);
        file2.write("DisplayString.BIN");
      }
      else
        card2.disp_string_file_exists = true;
      file2.close();
#endif

#ifdef ALLOW_DATA_LOG_FILES
      if (!internal_sd_card.exists(INT_LOG_FILE)) {
        card2.log_file_exists = file2.open(INT_LOG_FILE, O_WRITE | O_CREAT);
        file2.write("DataLog.BIN");
      }
      else
        card2.log_file_exists = true;
      file2.close();
#endif

#ifdef ALLOW_CALIBRATION_FILES
      if (!internal_sd_card.exists(INT_CALIBRATION_FILE)) {
        card2.calibration_file_exists = file2.open(INT_CALIBRATION_FILE, O_WRITE | O_CREAT);
        file2.write("Calibration.BIN");
      }
      else
        card2.calibration_file_exists = true;
      file2.close();
#endif

#ifdef ALLOW_INSTRUCTION_FILES
      if (!internal_sd_card.exists(INT_INSTRUCTION_FILE)) {
        card2.instruction_file_exists = file2.open(INT_INSTRUCTION_FILE, O_WRITE | O_CREAT);
        file2.write("Instructions.BIN");
      }
      else
        card2.instruction_file_exists = true;
      file2.close();
#endif

#ifdef ALLOW_BITMAP_FILES
      if (!internal_sd_card.exists(INT_BITMAP_FILE))
        card2.bitmap_file_exists = file2.open(INT_BITMAP_FILE, O_WRITE | O_CREAT);
      else
        card2.bitmap_file_exists = true;
      file2.close();
#endif
    }

    else { //dir does not exist, ensure all files disabled
      card2.network_file_exists = false;
      card2.disp_string_file_exists = false;
      card2.log_file_exists = false;
      card2.instruction_file_exists = false;
      card2.calibration_file_exists = false;
      card2.bitmap_file_exists = false;
    }
  }
}

void Card::copy_file(const char *from_filename, const char *to_filename, byte from_device, byte to_device) {

  if (!external_sd_card.exists(card1.working_dir)) return;
  if (!internal_sd_card.exists(card2.working_dir)) return;
  //  if (!ping()) return;

  external_sd_card.chdir(card1.working_dir);
  internal_sd_card.chdir(card2.working_dir);
  switch (from_device) {

    case EXTERNAL_CARD: {

        if (to_device == INTERNAL_CARD) {

          file1.open(from_filename, O_READ);
          file2.open(to_filename, O_WRITE | O_TRUNC);
          copy(from_device, to_device);
          file1.close();
          file2.close();
        }
        else if (to_device == RASP_PI) {

        }
        else
          Serial.println(F("Error:cannot copy to given location"));
      }
    case INTERNAL_CARD: {
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
      }
    case RASP_PI: {
        if (to_device == EXTERNAL_CARD) {

        }
        else if (to_device == EXTERNAL_CARD) {

        }
        else
          Serial.println(F("Error:cannot copy to given location"));
      }
    default:
      Serial.println(F("Error:cannot copy from given location"));

  }
}

void Card::copy(byte from_device, byte to_device) {

  uint8_t copy_buffer[COPY_BUF_SIZE];

  if (from_device == EXTERNAL_CARD) file1.rewind();
  else if (from_device == INTERNAL_CARD) file2.rewind();

  while (1) {
    int n = file1.read(copy_buffer, sizeof(copy_buffer));
    //if (n < 0) external_sd_card.errorExit("error reading external file");
    if (n == 0) break;
    else file2.write(copy_buffer, n);

  }
}

void Card::retrieve_data(String filename) {

  if (filename == INT_STRING_FILE || filename == EXT_STRING_FILE) {
    if (filename == INT_STRING_FILE) {
      if (!internal_sd_card.exists(card2.working_dir)) return;
      internal_sd_card.chdir(card2.working_dir);
      file1.open(INT_STRING_FILE, O_READ);
    }

    else if (filename == EXT_STRING_FILE) {
      if (!external_sd_card.exists(card1.working_dir)) return;
      external_sd_card.chdir(card1.working_dir);
      file1.open(EXT_STRING_FILE, O_READ);
    }

    int16_t char_read;
    bool x_start = false;   //place holders until file read
    bool y_start = false;
    bool x_end = false;
    bool y_end = false;

    while (char_read != -1 ) { //|| num_lines<max_lines) {

      int reads = 0;
      char command [DISP_STRING_COMMAND_LENGTH] = {'\0'};

      while (reads < DISP_STRING_COMMAND_LENGTH) {
        char_read = file1.read();
        if ((char)char_read == ':' || (char)char_read == '\n' ||  char_read == -1 ) break;
        else {
          command[reads] = (char)char_read;
          reads++;
        }
      }

      if (char_read == ':') {
        char data_found[MAX_TWEET_SIZE / 2] = {'\0'};
        reads = 0;
        while (reads < MAX_TWEET_SIZE) {
          char_read = file1.read();
          if ((char)char_read == '\n' ||  char_read == -1 ) break;
          else {
            data_found[reads] = (char)char_read;
            reads++;
          }
        }
        int value_found = atoi(data_found); //convert to int for some data types

        if (strcmp(command, STRING_FILE_COMMAND_STRING) == 0) {
          strncpy(text_str, data_found, sizeof(text_str));
          text_parameters.text_str_length = reads;
        }
        else if (strcmp(command, STRING_FILE_COMMAND_RED) == 0)   {
          text_parameters.red = value_found;
          text_parameters.use_hue = false;
        }
        else if (strcmp(command, STRING_FILE_COMMAND_GREEN) == 0) {
          text_parameters.green = value_found;
          text_parameters.use_hue = false;
        }
        else if (strcmp(command, STRING_FILE_COMMAND_BLUE) == 0) {
          text_parameters.blue = value_found;
          text_parameters.use_hue = false;
        }
        else if (strcmp(command, STRING_FILE_COMMAND_HUE) == 0) {
          text_parameters.hue = value_found;
          text_parameters.use_hue = true;
        }
        else if (strcmp(command, STRING_FILE_COMMAND_SIZE) == 0)
          text_parameters.text_size = value_found;
        else if (strcmp(command, STRING_FILE_COMMAND_X_SPEED) == 0)
          text_cursor.x_pos_dir = value_found;
        else if (strcmp(command, STRING_FILE_COMMAND_Y_SPEED) == 0)
          text_cursor.y_pos_dir = value_found;
        else if (strcmp(command, STRING_FILE_COMMAND_X_START_POS) == 0) {
          text_cursor.x_start = value_found;
          x_start = true;
        }
        else if (strcmp(command, STRING_FILE_COMMAND_Y_START_POS) == 0) {
          text_cursor.y_start = value_found;
          y_start = true;
        }
        else if (strcmp(command, STRING_FILE_COMMAND_X_END_POS) == 0)  {
          text_cursor.x_end = value_found;
          x_end = true;
        }
        else if (strcmp(command, STRING_FILE_COMMAND_Y_END_POS) == 0)   {
          text_cursor.y_end = value_found;
          y_end = true;
        }

      }
    }

    text_cursor.x_start_set = x_start;  //drive to false if not found
    text_cursor.y_start_set = y_start;
    text_cursor.x_end_set = x_end;
    text_cursor.y_end_set = y_end;

    graphics.configure_limits();
    graphics.reset_position();
    
    
    //
    //    if (text_cursor.x_start_set && text_cursor.x_end_set) {
    //      if (text_cursor.x_end > text_cursor.x_start) {
    //        text_cursor.x_limit_max = text_cursor.x_end;
    //        text_cursor.x_limit_min = text_cursor.x_start;
    //      }
    //      else if (text_cursor.x_end < text_cursor.x_start) {
    //        text_cursor.x_limit_max = text_cursor.x_start;
    //        text_cursor.x_limit_min = text_cursor.x_end;
    //      }
    //      else
    //        text_cursor.x_limit_max = text_cursor.x_limit_min = text_cursor.x_start;
    //
    //    }
    //    else {
    //      text_cursor.x_limit_min = -1*(text_parameters.text_size*text_parameters.text_width*text_parameters.text_str_length);
    //      text_cursor.x_limit_max = TOTAL_WIDTH;
    //    }
    //
    //    if (text_cursor.y_start_set && text_cursor.y_end_set) {
    //      if (text_cursor.y_end > text_cursor.y_start) {
    //        text_cursor.y_limit_max = text_cursor.y_end;
    //        text_cursor.y_limit_min = text_cursor.y_start;
    //      }
    //      else if (text_cursor.y_end < text_cursor.y_start) {
    //        text_cursor.y_limit_max = text_cursor.y_start;
    //        text_cursor.y_limit_min = text_cursor.y_end;
    //      }
    //      else
    //        text_cursor.y_limit_max = text_cursor.y_limit_min = text_cursor.y_start;
    //
    //    }
    //    else {
    //      text_cursor.y_limit_min = -1*(text_parameters.text_size*text_parameters.text_height);
    //      text_cursor.y_limit_max = SINGLE_MATRIX_HEIGHT;
    //    }
    //

  }



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

  if (filename == INT_LOG_FILE) {
    if (!internal_sd_card.exists(card2.working_dir)) return;
    internal_sd_card.chdir(card2.working_dir);

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
    if (!external_sd_card.exists(card1.working_dir)) return;
    external_sd_card.chdir(card1.working_dir);

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

  static int last_log_time = millis();
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

void Card::safely_eject_card(byte card) {

  if (card == INTERNAL_CARD) { //stop checking for card
    card2.enabled = false;
    card2.detected = false;
  }

  else if (card == EXTERNAL_CARD) {
    card1.enabled = false;
    card1.detected = false;
  }
}


void Card::mount_card(byte card) {

  if (card == INTERNAL_CARD) { //check for card
    card2.enabled = true;
    card2.detected = false;   //on first detection, read card
  }

  else if (card == EXTERNAL_CARD) {
    card1.enabled = true;
    card1.detected = false;
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
#endif // SD_Cards_CPP
