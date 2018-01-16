#ifndef SD_Cards_CPP
#define SD_Cards_CPP 

#include "SD_Cards.h"
#include "Due.h"


int card::init_sd_cards() {      // code to init sd cards and copy data from external to internal card

  // code for initialising sd cards, waits for both cards to be active then copies files from
  // the external one to the internal one. listas are made of all files in directory and errors are
  // thrown if files or directories cant be accessed.
  // TO DO: 1) copy all network info into array, will need to use maloc or stupidly big array to guarantee all data copied
  //        2) maybe look into copying more files (for instruction set, bitmap images etc)

  pinMode(SD1_CS, OUTPUT);

  Sprintln(F("\t Sd init setup..."));
  bool external_sd_card_initialised = false;
  bool internal_sd_card_initialised = false;

  pinMode(notifier, OUTPUT);
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
  int theta = beta;

  while (millis() < beta + WAIT_TIME_FOR_SD_ON_STARTUP && external_sd_card_initialised == 0) {    //wait for 40 seconds or until card inserted
    if (external_sd_card.begin(SD1_CS)) {    //if card initialised sucessfully exit wait loop
      external_sd_card_initialised = true;              //loop exit conditions if card detected
    }
#ifdef DEBUG
    else {          //flash led and print promt
      if (alpha % 50 == 0) {
        
          Sprintln(F("\t Insert external sd card"));

      }
      delay(20);
      alpha++;
      if (millis() >= theta + 3000) { //flash built in led while waiting as a notification if debug not enabled
        digitalWrite(notifier, HIGH);
        delay(200);
        digitalWrite(notifier, LOW);
        delay(200);
        digitalWrite(notifier, HIGH);
        delay(200);
        digitalWrite(notifier, LOW);
        theta = millis();
      }
#endif
      //send_frame... //display error on screen
    }
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
  if (!external_sd_card.chdir(sd_ext_dir)&& external_sd_card_initialised) external_sd_card.errorExit("external_sd_card.chdir");

  // make /SIGN2 the default directory for internal_sd_card
  if (!internal_sd_card.chdir(sd_int_dir)&& internal_sd_card_initialised) internal_sd_card.errorExit("internal_sd_card.chdir");

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

int card::copy_sd_data(const char *ext_file, const char *int_file, const char *ext_dir, const char *int_dir) {   //funtion to copy the data from /ext_dir/ext_file to /int_dir/int_file


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
    digitalWrite(notifier, !digitalRead(notifier));
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

int card::extract_network_data() {   // parse the file and extract network info (test network login not implmented yet 28/7/17)
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

        else if (is_default()) {
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

int card::buffer_in_header() {      //return 1 if the colon is in the header, otherwise 0

  if (sd_file_read_buffer[15] == '"')
    return 1;

  if (sd_file_read_buffer[16] == 'E' && sd_file_read_buffer[17] == 'x' && sd_file_read_buffer[18] == 'a' && sd_file_read_buffer[19] == 'm' && sd_file_read_buffer[20] == 'p' && sd_file_read_buffer[21] == 'l' && sd_file_read_buffer[22] == 'e' )//&& (sd_file_read_buffer[23] == '\n' || sd_file_read_buffer[23] == '\t'))
    return 1;

  return 0;
}

int card::buffer_is_network() {     //check if network stored in buffer
  if (sd_file_read_buffer[7] == 'N' && sd_file_read_buffer[8] == 'e' && sd_file_read_buffer[9] == 't' && sd_file_read_buffer[10] == 'w' && sd_file_read_buffer[11] == 'o' && sd_file_read_buffer[12] == 'r' && sd_file_read_buffer[13] == 'k')
    return 1;
  return 0;

}

int card::buffer_is_password() {   //check if password stored in buffer
  if (sd_file_read_buffer[6] == 'P' && sd_file_read_buffer[7] == 'a' && sd_file_read_buffer[8] == 's' && sd_file_read_buffer[9] == 's' && sd_file_read_buffer[10] == 'w' && sd_file_read_buffer[11] == 'o' && sd_file_read_buffer[12] == 'r' && sd_file_read_buffer[13] == 'd')
    return 1;
  return 0;
}

int card::buffer_is_default() {    //check if default string stored in buffer
  if (sd_file_read_buffer[7] == 'D' && sd_file_read_buffer[8] == 'e' && sd_file_read_buffer[9] == 'f' && sd_file_read_buffer[10] == 'a' && sd_file_read_buffer[11] == 'u' && sd_file_read_buffer[12] == 'l' && sd_file_read_buffer[13] == 't')
    return 1;
  return 0;
}

int card::string_length() {  // get string length, data strings seperated by tab or return carrige at end
  int alpha = 16;
  while (sd_file_read_buffer[alpha] != '\n' && sd_file_read_buffer[alpha] != '\t') {  //if the current index is a carriage return exit
    alpha++;    //increment alpha
  }
  return alpha - 16 - 1; //alpha-offset-1
}

int card::remove_card_1() {  //function to stall code until the external sd is removed, safety to prevent sd containing network info left unattended

  // function to prevent standard operation of the display until external sd card removed.

  // TO DO, currently the code stalls indefinitely while waiting for the sd card to be removed, maybe add a timer interrupt and enter a restricted operation mode

  int alpha = 0;
  int theta = millis();
  bool external_sd_card_initialised = false;

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

    if (millis() >= theta + 3000) { //flash built in led while waiting as notification if serial not connected
      digitalWrite(notifier, HIGH);
      delay(200);
      digitalWrite(notifier, LOW);
      delay(200);
      digitalWrite(notifier, HIGH);
      delay(200);
      digitalWrite(notifier, LOW);
      theta = millis();
    }
#endif
  }
}


#endif // SD_Cards_CPP
