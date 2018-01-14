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
  int card1_init = 0;
  int card2_init = 0;

  pinMode(notifier, OUTPUT);
  PgmPrint("\t FreeRam: ");

  Sprintln(FreeRam());

  // fill buffer with known data
  for (int i = 0; i < sizeof(buf); i++) buf[i] = i;

  // disable sd_internal while initializing sd_external
  pinMode(SD2_CS, OUTPUT);
  digitalWrite(SD2_CS, HIGH);


  // initialize the external card
  int beta = millis();
  int theta = beta;
  int alpha = 0;
  while (millis() < beta + 40000 && card1_init == 0) {    //wait for 40 seconds or until card inserted
    if (sd_external.begin(SD1_CS)) {    //if card initialised sucessfully exit wait loop
      card1_init = 1;              //loop exit conditions if card detected
    }
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
      //send_frame... //display error on screen
    }
  }

  if (card1_init == 0) {
    Sprintln(F("\t No SD card detected"));
    sd_external.initError("sd_external:");    //stops code when called
  }


  // create SIGN1 on sd_external if it does not exist
  // Use this to test if card is inserted, if directory can't be made, functional card not inserted (or wiring problem..., presume card)
  beta = millis();
  card1_init = 0;
  alpha = 0;

  while (millis() < beta + 5000 && card1_init == 0) {    // make sure directory exists on external card, may take a moment, if longer than 5 seconds then error

    if (!sd_external.exists(sd_ext_dir)) {
      if (alpha % 50 == 0) //fast loop, do not print all itterations
        Sprintln(F("\t External Directory does not exist"));

      if (!sd_external.mkdir(sd_ext_dir)) {
        if (alpha % 50 == 0)
          Sprintln(F("\t Can't create External Directory"));

        alpha++;
        if (alpha == 20)  //print once, after short delay
          Sprintln(F("\t Please insure card inserted properly"));
        delay(20);

      }

    }
    else
      card1_init = 1;    //folder exists, exit loop
  }
  if (card1_init == 0) {
    sd_external.errorExit("sd_external.mkdir");   //while(1)...
  }


  //repeat some process for internal card
  beta = millis();
  alpha = 0;

  while (millis() < beta + 10000 && card2_init == 0) {    //wait until card initialised correctly, shorter time to insert as it should be in all the time
    if (sd_internal.begin(SD2_CS)) {
      card2_init = 1;              //loop exit conditions if card detected
    }
    else {
      if (alpha % 100 == 0)
        Sprintln(F("\t Insert internal sd card "));   //promt for user
      //send_frame... //display error on screen
      delay(20);
      alpha++;
    }
  }

  if (card2_init == 0)
    sd_external.initError("sd_internal:");    //stops code when called

  // create SIGN2 on sd_internal if it does not exist
  if (!sd_internal.exists(sd_int_dir)) {
    if (!sd_internal.mkdir(sd_int_dir)) sd_internal.errorExit("sd_internal.mkdir");
  }

  #if defined (DEBUG)
    // list root directory on both cards
    Sprintln("\t ------sd_external root-------");
    sd_external.ls();
    Sprintln("\t ------sd_internal root-------");
    sd_internal.ls();
  #endif

  // make /SIGN1 the default directory for sd_external
  if (!sd_external.chdir(sd_ext_dir)) sd_external.errorExit("sd_external.chdir");

  // make /SIGN2 the default directory for sd_internal
  if (!sd_internal.chdir(sd_int_dir)) sd_internal.errorExit("sd_internal.chdir");

  // list current directory on both cards
  #if defined(DEBUG) 
    Sprintln("\t ------sd_external SIGN1-------");
    sd_external.ls();
    Sprintln("\t ------sd_internal SIGN2-------");
    sd_internal.ls();
    Sprintln("\t ---------------------");
  #endif

  copy_sd_data(sd_ext_file, sd_int_file, sd_ext_dir, sd_int_dir);

}

int card::copy_sd_data(const char *ext_file, const char *int_file, const char *ext_dir, const char *int_dir) {   //funtion to copy the data from /ext_dir/ext_file to /int_dir/int_file


  // set the current working directory for open() to sd_external
  sd_external.chvol();

  //  // make /SIGN1 the default directory for sd_external    //shouldnt need to call these but do anyway to be sure
  //  if (!sd_external.chdir(ext_dir)) sd_external.errorExit("sd_external.chdir");
  //
  //  // make /SIGN2 the default directory for sd_internal
  //  if (!sd_internal.chdir(int_dir)) sd_internal.errorExit("sd_internal.chdir");



  // create or open /ext_dir/ext_file and truncate it to zero length

  if (!file1.open(ext_file, O_RDWR | O_CREAT )) {
    sd_external.errorExit("error opening external sd file");
  }

  // set the current working directory for open() to sd_internal
  sd_internal.chvol();

  // create or open /int_dir/int_file and truncate it to zero length

  if (!file2.open(int_file, O_WRITE | O_CREAT | O_TRUNC)) {
    sd_internal.errorExit("error opening internal sd file");
  }
  Sprintln("\t Copying");

  // copy file1 to file2
  file1.rewind();
  uint32_t t = millis();

  while (1) {
    int n = file1.read(buf, sizeof(buf));
    if (n < 0) sd_external.errorExit("error reading external file");
    if (n == 0) break;
    if (file2.write(buf, n) != n) sd_internal.errorExit("error writing internal file");
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


  char temp_password[25];   //temp arrays to copy data into if found, makes converting from array of lots of data to string of only selected data easier
  char temp_network[25];
  char temp_default [50];

  int str_len = 0;
  bool Connected = false;     //value to define if ethernet/wifi connected with current network and password, if true stop looping through file
  int n = 1;                  //define n as non zero
  int alpha = 0;
  int beta = 0;

  SD_string.str_sd.reserve(150);

  sd_internal.chvol();

  if (!file2.open("TEST2.BIN", O_READ)) {       //open file for reading
    Sprintln(F("Can't open file to extract data"));
    return (-1);
  }

  while (!Connected && n != 0) {  //while Connected not set to 1 and file end no reached

    file2.seekSet(alpha);       //set cursor position, and increment by one on each loop (probably better way to do this)
    alpha++;
    n = file2.read(read_buffer, sizeof(read_buffer));   //returns 0 if file finished, negative if error
    if (n < 0) sd_internal.errorExit("read2");

    if (read_buffer[14] == ':') {   // if it equals a colon it could be a password or network
       Sprintln(F("colon found"));

      if (in_header()) { // check if colon is from header, such as examples
        //do nothing
       Sprintln(F("colon in header"));
      }

      else {            // if not, presume to be valid string found
        if (is_network()) {
          str_len = string_length();    //call function to find length of string

          for (beta = 0; beta < str_len; beta++) { //loop through network name
            SD_string.Network[beta] = read_buffer[beta + 16]; // offset to where string starts

          }
          SD_string.Network.remove(beta);
          
        }

        else if (is_password()) {
          str_len = string_length();    //call function to find length of string

          for (beta = 0; beta < str_len; beta++) { //loop through network name
            SD_string.Password[beta] = read_buffer[beta + 16]; // offset to where string starts

          }
          SD_string.Password.remove(beta);
          Connected=true;
          // attempt to connect to network using network and password...
          //wifi_connect....
          //ethernet_connect...
        }

        else if (is_default()) {
          str_len = string_length();    //call function to find length of string
          Sprint(F("string length: "));
          Sprintln(str_len);
          

          beta = 0;
          while (beta != str_len) { //loop through network name
            SD_string.str_sd[beta] = read_buffer[beta + 16]; // offset to where string starts
            
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

  if (read_buffer[15] == '"')
    return 1;

  if (read_buffer[16] == 'E' && read_buffer[17] == 'x' && read_buffer[18] == 'a' && read_buffer[19] == 'm' && read_buffer[20] == 'p' && read_buffer[21] == 'l' && read_buffer[22] == 'e' )//&& (read_buffer[23] == '\n' || read_buffer[23] == '\t'))
    return 1;

  return 0;
}

int card::buffer_is_network() {     //check if network stored in buffer
  if (read_buffer[7] == 'N' && read_buffer[8] == 'e' && read_buffer[9] == 't' && read_buffer[10] == 'w' && read_buffer[11] == 'o' && read_buffer[12] == 'r' && read_buffer[13] == 'k')
    return 1;
  return 0;

}

int card::buffer_is_password() {   //check if password stored in buffer
  if (read_buffer[6] == 'P' && read_buffer[7] == 'a' && read_buffer[8] == 's' && read_buffer[9] == 's' && read_buffer[10] == 'w' && read_buffer[11] == 'o' && read_buffer[12] == 'r' && read_buffer[13] == 'd')
    return 1;
  return 0;
}

int card::buffer_is_default() {    //check if default string stored in buffer
  if (read_buffer[7] == 'D' && read_buffer[8] == 'e' && read_buffer[9] == 'f' && read_buffer[10] == 'a' && read_buffer[11] == 'u' && read_buffer[12] == 'l' && read_buffer[13] == 't')
    return 1;
  return 0;
}

int card::string_length() {  // get string length, data strings seperated by tab or return carrige at end
  int alpha = 16;
  while (read_buffer[alpha] != '\n' && read_buffer[alpha] != '\t') {  //if the current index is a carriage return exit
    alpha++;    //increment alpha
  }
  return alpha - 16 - 1; //alpha-offset-1
}

int card::remove_card_1() {  //function to stall code until the external sd is removed, safety to prevent sd containing network info left unattended

  // function to prevent standard operation of the display until external sd card removed.

  // TO DO, currently the code stalls indefinitely while waiting for the sd card to be removed, maybe add a timer interrupt and enter a restricted operation mode


  int theta = millis();
  int card1_init = 0;
  int alpha = 0;
  while (card1_init == 0) {
    if (!sd_external.begin(SD1_CS)) {
      card1_init = 1;              //loop exit conditions if card detected
    }
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
  }
}


#endif // SD_Cards_CPP
