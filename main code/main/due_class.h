#include "libraries.h"


#ifdef _SAM3XA_
class due_class : public sign_coms {

  private:

    //Led_Strip_Struct led_strip;
    //Fan_Struct fan;
    //Encoder_Struct encoder;

  public:

    due_class() {}

    //void stop();

    int init_due();       //initialiser for all peripherals, specify which to use in global_variables.h file

    int init_led_strip();           //initialisations of individual items, set pins, test if working if applicable, etc
    int init_fans();

    int init_temp_sensors();
    int init_LDR();
    int init_current_meter();
    int init_sd_cards();
    int init_ethernet();
    int init_wifi();

    int set_address();    //set the address variable in each of the mega structures
    int calc_offset();    //calc offset of text from global coordinates

    int test_i2c();                 //use i2c scanner to find addreses connected and send bad frame to find nack pin
    int calc_delay();               //find delay between sending all four pos frames

    int writeLedStrips(int newValue);       // change value of led strips, attach timers to fade in an out over period
    int writeFanSpeed(int newValue);

    int led_gradual_pulse(int minValue, int maxValue);  //code to fade the leds in and out in sinusoidal pattern

    int toggle_led();                   // toggle the on/off bool, detach interrupt if off
    int toggle_fan();
    int toggle_led(int newValue);       // toggle on/off and set new value at same time, detach interrupt if off
    int toggle_fan(int newValue);

    int is_led_on();                    // useful funcions that may be needed
    int is_fan_on();
    int return_led_brightness();
    int return_fan_speed();

    int recenter_encoder();             // reset position to 0


    int handle_interrupts();            // handle interrupt interpretation in code. Use structures  interrupt bool value (eg encoder_moved) to identify if an interrupt occured in the last loop.
    // If corresponding structure variable "clean loop" is false, set true for one loop. this is an indicator to other functions
    // to execute their code. the proceedure should go, interrupt happens, interrupt bool set true, loop ends, handle interrupts checks
    // if interrupt occured, if yes set interrupt bool to false and clean loop to true, functions in loop use clean loop run their code,
    // loop ends, handle interrupts sets clean loop false

    int copy_sd_data(const char *ext_file, const char *int_file, const char *ext_dir, const char *int_dir);
    int extract_network_data();
    int in_header();         // return 1 if the colon is in the header, otherwise 0
    int is_network();         // check if network stored in buffer
    int is_password();        // check if password stored in buffer
    int is_default();        // check if default string stored in buffer
    int string_length();      // calculate the length of the relevent string in file
    int remove_card_1();

    int read_current_meter(int pin);  // read the current sensor
    float reading_to_amps(int value);   // convert 10 bit analog reading to amps

    int LDR_calibration();        //calibrate maximum minimum and room brightness lights
    int LDR_check_value();        //read the average value of the LDR's, if one fails to read correctly or large disparity, return other, if both fail return fail

    int set_LDR_enable(int sensor);   // toggle, ueful if you dint care about ambient brightness, or sensor not working
    int set_LDR_disable(int sensor);

    int temp_check_temp(int sensor);   // get the temp vaule from the specified sensor register
    int temp_check_humidity(int sensor);         // likewise, might be useful, probably not

    int set_temp_sensor_enable(int sensor);   //code to enable sensors, might be useful to toggle them, maybe autotoggle if not working?
    int set_temp_sensor_disable(int sensor);


    int temp_read_sensor (int pin);        // read temperature based on specified pin, return value in degrees C
    int read_temp_sensor (int pin);       // read temp sensor registers, used to update stored value to most recent
    byte read_temp_data (int pin);        // convert incoming bits from temp sensor to a byte

    int log_temp_to_sd();               // log data to the sd card data related to temps over time

    int get_serial();                     //function to interpret serial data recieved without a user prompt


    // i2c functions
    int send_disp_string_frame(int address);                             //complete function to send strings over i2c to display on specified screen
    int pack_disp_string_frame(int frame_type, int frame_offset);        //function to pack a frame of text to display
    int wire_write_frame(int address) ;                                  //function to actually send the frame to given address
    int send_pos_frame(int address);                                     //function to send the xy coordinates along with a number of other related variables
    int pack_xy_coordinates() ;                                          //function to pack the 4 bytes to send the x and y positions of the text cursor
    int send_all_calibration_data(int address);                          //function to send all data calibration
    int send_specific_calibration_data(byte sensor_prefix, int address, bool more_bytes, int offset);  //function to send specific value
    int get_text_colour_hue(int byte_number);                            //function to retrun the MSB or LSB of the current hue value to send over i2c

    int send_all_pos();     // function to send pos data to all megas periodically based on timer interrupt
    int send_all_pos_now();
    int send_all_text();    // send the text frame to all megas
    int print_frame();      //print the currently stored frame to the serial monitor
};






//methods for due_class

// initialiser functions
int due_class::init_due() {           //initialise everything listed true in the global variables enable list

  // repeat same procedure for all inits: check if should be enabled, try enable function,
  // if returns fail stop code and print message (unless its something that doesnt need to work
  // to run the code), otherwise indicate device sucessfully enabled and repeat for next init

  int fail = 0;   //failing variable, should function fail to initialise, respond
  if (enable_serial) {
    fail = sign_coms::init_serial();
    if (fail != 0) {
        if (fail == -2)
          Sprintln(F("Baud rate not vailid "));
        Sprintln(F("Failed to initialise serial, stopping execution..."));
      
      serial_enabled = false;
      sign_coms::stop();
    }
    else
      Sprintln(F("Initialised Serial"));
    
    if (fail == 0)
      serial_enabled = true;

    Sprintln("");
  }

  if (enable_i2c) {
    
      Sprintln(F("Initialising i2c..."));
    fail = sign_coms::init_i2c();
    if (fail != 0) {
      
        Sprintln(F("Failed to initialise i2c, stopping execution..."));
      i2c_enabled = false;
      sign_coms::stop();
    }
    else
      Sprintln(F("Initialised i2c"));
    
    if (fail == 0)
      i2c_enabled = true;

    Sprintln("");
  }


  if (enable_fans) {
    
      Sprintln(F("Initialising fans..."));
    fail = init_fans();
    if (fail != 0) {
      
      Sprintln(F("Failed to initialise fans, stopping execution..."));
      fans_enabled = false;
      sign_coms::stop();

    }
    else 
      Sprintln(F("Initialised fans"));
    
    if (fail == 0)
      fans_enabled = true;


    Sprintln("");
  }


  if (enable_led_strip) {
    
    Sprintln(F("Initialising led strip..."));
    fail = init_led_strip();
    if (fail != 0) {
      Sprintln(F("Failed to initialise led_strip, stopping execution..."));
      led_strip_enabled = false;
      sign_coms::stop();

    }
    else 
      Sprintln(F("Initialised led strip"));
    
    if (fail == 0)
      led_strip_enabled = true;


    Sprintln("");
  }


  if (enable_encoder) {
    
    Sprintln(F("Initialising encoder..."));
    fail = init_encoder();
    if (fail != 0) {
      Sprintln(F("Failed to initialise encoder, stopping execution..."));
      encoder_enabled = false;
      sign_coms::stop();

    }
    else
      Sprintln(F("Initialised encoder"));
    
    if (fail == 0)
      encoder_enabled = true;


    Sprintln("");
  }


  if (enable_button) {
    Sprintln(F("Initialising button..."));
    fail = init_button();
    if (fail != 0) {
      Sprintln(F("Failed to initialise button, stopping execution..."));
      button_enabled = false;
      sign_coms::stop();
    }
    else
      Sprintln(F("Initialised button"));
    
    if (fail == 0)
      button_enabled = true;

    Sprintln("");
  }


  if (enable_sd_cards) {
    Sprintln(F("Initialising sd cards..."));
    fail = init_sd_cards();
    if (fail != 0) {
      Sprintln(F("Failed to initialise sd cards, stopping execution..."));
      sd_cards_enabled = false;
      sign_coms::stop();
    }
    else
      Sprintln(F("Initialised sd cards"));
    
    if (fail == 0)
      sd_cards_enabled = true;


    Sprintln("");
  }


  if (enable_current_meter) {
    Sprintln(F("Initialising current meter..."));
    fail = init_current_meter();
    if (fail != 0) {
      Sprintln(F("Failed to initialise current meter, stopping execution..."));
      current_meter_enabled = false;
      sign_coms::stop();
    }
    else
      Sprintln(F("Initialised current meter"));
    
    if (fail == 0)
      current_meter_enabled = true;


    Sprintln("");
  }


  if (enable_temp_sensor) {
    Sprintln(F("Initialising temp sensor..."));
    fail = init_temp_sensors();
    if (fail != 0) {
      Sprintln(F("Failed to initialise temp sensor, stopping execution..."));
      temp_sensor_enabled = false;
      sign_coms::stop();
    }
    else
      Sprintln(F("Initialised temp sensor"));
    
    if (fail == 0)
      temp_sensor_enabled = true;

    Sprintln("");
  }


  if (enable_LDR) {
    Sprintln(F("Initialising LDR's..."));
    fail = init_LDR();
    if (fail != 0) {
      Sprintln(F("Failed to initialise LDR's, stopping execution..."));
      LDR_enabled = false;
      sign_coms::stop();
    }
    else
      Sprintln(F("Initialised LDR's"));
    
    if (fail == 0)
      LDR_enabled = true;


    Sprintln("");
  }


  if (enable_ethernet) {
    Sprintln(F("Initialising ethernet..."));
    fail = init_ethernet();
    if (fail != 0) {
      Sprintln(F("Failed to initialise ethernet, stopping execution..."));
      ethernet_enabled = false;
      sign_coms::stop();
    }
    else
      Sprintln(F("Initialised ethernet"));
    
    if (fail == 0)
      ethernet_enabled = true;


    Sprintln("");
  }

  if (enable_wifi) {
    Sprintln(F("Initialising wifi..."));
    fail = init_wifi();
    if (fail != 0) {
      Sprintln(F("Failed to initialise wifi, stopping execution..."));
      wifi_enabled = false;
      sign_coms::stop();
    }
    else
      Sprintln(F("Initialised wifi"));
    
    if (fail == 0)
      wifi_enabled = true;


    Sprintln("");
  }


  return 0;

}

int due_class::init_led_strip() {     // initialise the led strip and set it to starting value

  pinMode(led_strip.pin, OUTPUT);
  
  Sprintln(F("\t Set led strip to value..."));
  int fail = writeLedStrips(led_strip.target_brightness);  // start leds and fade to target brightness

  if (fail != 0) {
    Sprintln(F("Failed to write initial led strip brightness"));
    return (-1);
  }
  else
    return (0);
}

int due_class::init_fans() {          //initialise fans and set to starting value
  pinMode(fan.fan_pin, OUTPUT);
  
  Sprintln(F("\t Set fans to value..."));
  int fail = writeFanSpeed(fan.fan_target_speed);

  if (fail != 0) {
    Sprintln(F("Failed to write initial fan speed"));
    return (-1);
  }
  else
    return (0);

}

int due_class::init_current_meter() {   //code to intialise the both current meters and get initial values

  pinMode(current.pin1, INPUT);
  pinMode(current.pin2, INPUT);

  // initial readings for sensor 1
  Sprintln(F("\t Get reading for Current Meter 1..."));
  int raw = read_current_meter(current.pin1);   // get initial reading
  float reading = reading_to_amps(raw);         // convert reading to amps for sensor 1

  current.raw_reading_1 = raw;            //set initial values
  current.current_reading_1 = reading;

  //sensor 2
    Sprint(F("\t Current meter 1 :"));
    Sprint(current.current_reading_1);
    Sprintln(F(" Amps"));

    Sprintln(F("\t Get reading for Current Meter 2..."));
  
  raw = read_current_meter(current.pin2);
  reading = reading_to_amps(raw);

  current.raw_reading_2 = raw;
  current.current_reading_2 = reading;

  
    Sprint(F("\t Current meter 2 :"));
    Sprint(current.current_reading_2);
    Sprintln(F(" Amps"));
  

  if ((int)current.current_reading_1 == 0 || (int)current.current_reading_2 == 0) {
    
      Sprintln(F("WARNING: check current meter connected, reading ground"));

    // return(-1);    //verify this before return -1. my get reading if screen is off
  }
  return (0);
}



int due_class::init_sd_cards() {      // code to init sd cards and copy data from external to internal card

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


int due_class::copy_sd_data(const char *ext_file, const char *int_file, const char *ext_dir, const char *int_dir) {   //funtion to copy the data from /ext_dir/ext_file to /int_dir/int_file


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

int due_class::init_temp_sensors() {    //code to initialise temp sensors

  //not much to initialise since the pin is set to output and input to request and recieve data
  // just test the sensor responds

  int fail;

  fail = temp_read_sensor(temp.pin1);

  if (fail == -100) { //if the value is -100 -> error
  return(-1);
  }
  else {
    Sprint(F("Read temp1 as:"));
    Sprintln(fail);
  }
  return(0);
}

int due_class::init_LDR() {           //code to initialise LDR's

  pinMode(LDR.pin1, INPUT);
  pinMode(LDR.pin2, INPUT);

  int fail = LDR_check_value();     //check ldrs are returning reasonable values

  if (fail != 0) {
   
    Sprintln(F("One or more LDR's failed to initialise properly"));
    return (-1);
  }

  else 
    return (0);

}

int due_class::init_ethernet() {}     //not implented

int due_class::init_wifi() {       //not implented

  if (sd_cards_enabled) {        //make sure the sd cards have been enabled, otherwise due cant have access to network info

  }

  else {
    if (debug)
      Sprintln(F("From init wifi: Sd cards not initialised, dont have network info"));
  }
}


int due_class::set_address() {   //set the address variable in each of the mega structures
  mega_var1.address = 1;
  mega_var2.address = 2;
  mega_var3.address = 3;
  mega_var4.address = 4;
  return (0);
}

int due_class::calc_offset() {}

// sd card methods
int due_class::extract_network_data() {   // parse the file and extract network info (test network login not implmented yet 28/7/17)
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

int due_class::in_header() {      //return 1 if the colon is in the header, otherwise 0

  if (read_buffer[15] == '"')
    return 1;

  if (read_buffer[16] == 'E' && read_buffer[17] == 'x' && read_buffer[18] == 'a' && read_buffer[19] == 'm' && read_buffer[20] == 'p' && read_buffer[21] == 'l' && read_buffer[22] == 'e' )//&& (read_buffer[23] == '\n' || read_buffer[23] == '\t'))
    return 1;

  return 0;
}

int due_class::is_network() {     //check if network stored in buffer
  if (read_buffer[7] == 'N' && read_buffer[8] == 'e' && read_buffer[9] == 't' && read_buffer[10] == 'w' && read_buffer[11] == 'o' && read_buffer[12] == 'r' && read_buffer[13] == 'k')
    return 1;
  return 0;

}

int due_class::is_password() {   //check if password stored in buffer
  if (read_buffer[6] == 'P' && read_buffer[7] == 'a' && read_buffer[8] == 's' && read_buffer[9] == 's' && read_buffer[10] == 'w' && read_buffer[11] == 'o' && read_buffer[12] == 'r' && read_buffer[13] == 'd')
    return 1;
  return 0;
}

int due_class::is_default() {    //check if default string stored in buffer
  if (read_buffer[7] == 'D' && read_buffer[8] == 'e' && read_buffer[9] == 'f' && read_buffer[10] == 'a' && read_buffer[11] == 'u' && read_buffer[12] == 'l' && read_buffer[13] == 't')
    return 1;
  return 0;
}

int due_class::string_length() {  // get string length, data strings seperated by tab or return carrige at end
  int alpha = 16;
  while (read_buffer[alpha] != '\n' && read_buffer[alpha] != '\t') {  //if the current index is a carriage return exit
    alpha++;    //increment alpha
  }
  return alpha - 16 - 1; //alpha-offset-1
}

int due_class::remove_card_1() {  //function to stall code until the external sd is removed, safety to prevent sd containing network info left unattended

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

int recenter_encoder() {    //code to recenter the encoder position, useful when changing menu or after time out
  
  Sprintln(F("\t Encoder recentered"));
  encoder.position = encoder.center;

}

int handle_interrupts() {   // function to repond to an ISR.

  //ISR is short ideally meant to be a short piee of code, and can arrive at any point in the loop
  // this code forces all code that responds to an ISR to do so for one complete loop only, regardless of where in the loop the ISR may occur


  if (encoder.encoder_moved || encoder.clean_loop) {   //if interrupt just happened or happened one loop ago

    if (encoder.encoder_moved) { //interrupt just happened
      encoder.encoder_moved = false;         //acknowlege interrupt just happened
      encoder.clean_loop = true;             //set clean loop to true to allow funtions to execute next loop
      
      Sprintln(F("encoder interrupt detected"));
    }

    else if (encoder.clean_loop) {
      encoder.clean_loop = false;              // functions executed, set clean loop false until next ISR
      
        Sprintln(F("interrupt completed"));
    }
  }


  if (button.button_pressed || button.clean_loop) {

    if (button.button_pressed) {
      button.button_pressed = false;
      button.clean_loop = true;
      
      Sprintln(F("button interrupt detected"));
    }
    else if (encoder.clean_loop) {
      button.clean_loop = false;
     
        Sprintln(F("interrupt completed"));
    }
  }


  // ...  other ISR handling  ...

}




int due_class::writeLedStrips(int newValue) {     //function to fade in led strip from current value to new value

  // function will set new value, and attach interrupt if not attached already
  // when target value is reached, isr will set ISR_attached to false and isr will be detached at the end of the loop


}

int due_class::writeFanSpeed(int newValue) {  //function to fade in fans from current value to new value

  // function will set new value, and attach interrupt if not attached already
  // when target value is reached, isr will set ISR_attached to false and isr will be detached at the end of the loop
}



int due_class::read_current_meter(int pin) {} // read the current sensor

float due_class::reading_to_amps(int value) {}  // convert 10 bit analog reading to amps


int due_class::temp_read_sensor(int pin) {  //return the temperature as read by the specified pin

  // call the read_temp_sensor to get it to return the data in its registers
  // then get the relavent value of dat[] array and return it


  read_temp_sensor(pin);      //update registers

  //display results from function
  #if defined(DEBUG)
  Sprint ("Current humdity =");
  Serial.print (temp.dat1 [0], DEC); // display the humidity-bit integer;
  Sprint ('.');
  Serial.print (temp.dat1 [1], DEC); // display the humidity decimal places;
  Sprintln ('%');

  Sprint ("Current temperature =");
  Serial.print (temp.dat1 [2], DEC); // display the temperature of integer bits;
  Sprint ('.');
  Serial.print (temp.dat1 [3], DEC); // display the temperature of decimal paces;
  Sprintln ('C');
  #endif

  Sprintln(F("WARNING: function not fully implemented, only reading sensor 1")); 

}

int due_class::read_temp_sensor (int pin) {    //adapted from this: https://tkkrlab.nl/wiki/Arduino_KY-015_Temperature_and_humidity_sensor_module

  pinMode(pin, OUTPUT);      // confirm that the pin is an output
  digitalWrite (pin, LOW);   // bus down, send start signal, drive line to ground
  delay (30);                   // delay greater than 18ms, so DHT11 start signal can be detected
  digitalWrite (pin, HIGH);  // drive bus to logic high again
  delayMicroseconds (40);       // Wait for DHT11 response


  pinMode (pin, INPUT);               // set pin to recieve data
  while (digitalRead (pin) == HIGH);  // wait for sensor
  delayMicroseconds (80);                // DHT11 response, pulled the bus 80us
  if (digitalRead (pin) == LOW);
  delayMicroseconds (80);                // DHT11 80us after the bus pulled to start sending data

  if (pin = TEMP1) {
    for (int i = 0; i < 4; i ++)           // receive temperature and humidity data, the parity bit is not considered
      temp.dat1[i] = read_temp_data (pin);               // data to global array

    
      Sprintln(F("Read temp sensor 3"));

  }

  else if (pin = TEMP2) {
    for (int i = 0; i < 4; i ++)           // receive temperature and humidity data, the parity bit is not considered
      temp.dat2[i] = read_temp_data (pin);
    
      Sprintln(F("Read temp sensor 3"));
  }

  else if (pin = TEMP3) {
    for (int i = 0; i < 4; i ++)           // receive temperature and humidity data, the parity bit is not considered
      temp.dat3[i] = read_temp_data (pin);

   
      Sprintln(F("Read temp sensor 3"));
  }

  else {

    Sprintln(F("Pin not defined as a temperature sensor, define as input to avoid possible damage"));
    pinMode(pin, INPUT);
    return (-1);
  }


  pinMode (pin, OUTPUT);
  digitalWrite (pin, HIGH);            // send data once after releasing the bus, wait for the host to open the next Start signal
  return (0);
}

byte due_class::read_temp_data (int pin) {   // read the data back from the register as bits and convert to a byte, call this for every byte to read

  byte data;
  for (int i = 0; i < 8; i ++) {
    if (digitalRead (pin) == LOW) {
      while (digitalRead (pin) == LOW);    // wait for 50us
      delayMicroseconds (30);                 // determine the duration of the high level to determine the data is '0 'or '1'
      if (digitalRead (pin) == HIGH)
        data |= (1 << (7 - i));               // high front and low in the post
      while (digitalRead (pin) == HIGH);   // data '1 ', wait for the next one receiver
    }
  }
  return data;   //return one byte of data
}



int due_class::LDR_check_value() {       // read the average value of the LDR's, if one fails to read correctly or large disparity fail returned

  int reading1 = analogRead(LDR.pin1);      // read pin 1 and pin 2
  int reading2 = analogRead(LDR.pin2);


  // error detection, if rail voltage dont include average ambient light calculation

  if (LDR.LDR1_enable && (reading1 == 0 || reading1 == 1023)) {  //check if reading is reliable, is potiential divider not connected correctly reading not usable
    Sprintln(F("From LDR_check_value: \t Problem reading light sensor 1, check wiring"));
    LDR.LDR1_enable = false;

  }

  if (LDR.LDR2_enable && (reading2 == 0 || reading2 == 1023)) {
    Sprintln(F("From LDR_check_value: \t Problem reading light sensor 2, check wiring"));
    LDR.LDR2_enable = false;
  }



  if (LDR.LDR1_enable == false && LDR.LDR2_enable == true) {  //if one sensor is usable just return the value
    
      Sprintln(F("From LDR_check_value: \t Return sensor 2 reading"));
    return reading2;
  }

  if (LDR.LDR2_enable == false && LDR.LDR1_enable == true) {
   
      Sprintln(F("From LDR_check_value: \t Return sensor 1 reading"));
    return reading1;
  }

  if (LDR.LDR1_enable == true && LDR.LDR2_enable == true) {   //if both usable, compare readings
    int avg;

    if (abs(reading1 - reading2) >= LDR.large_disparity) { //if large disparity between sensor readings, return the higher, possibly one covered
      
        Sprintln(F("From LDR_check_value: \t Large disparity in sensor readings, return higher"));
        Sprint(F("sensor 1: "));
        Sprint(reading1);
        Sprint(F("\t sensor 2: "));
        Sprintln(reading2);
      
      if (reading1 > reading2)
        return reading1;

      else
        return reading2;
    }
    else {                                      //if values close assume both valid, return average
      avg = (reading1 + reading2) / 2;
      
        Sprintln(F("From LDR_check_value: Both LRD readings good, return average"));
        Sprint(F("Average reading: "));
        Sprintln(avg);
      
      return avg;
    }
  }

  if (LDR.LDR1_enable == false && LDR.LDR2_enable == false) {   //if neither usable return -1, error both disconnected
    
      Sprintln(F("From LDR_check_value: \t Error reading both sensors, check connections"));
    return (-1);
  }
}


int due_class::LDR_calibration() {        // code to set ambient brightness, max and min values

  // since the lrd likely will never read near the  rail voltage when in light or darkness
  // ambient, max and min will need to be calibrated. this could be calculated roughly too, based on
  // approximate series resistor values, but will vary depending on component tollerances
  // TO DO: 1) add screen interface not just serial feedback
  //        2) configure sensors individually not just the average, in case using non identical components


  if (debug) {    // need dubug for this to work
    char accept_val = 'w';    //variable to detect if serial input given, if val == x -> quit
    //                                          id val == s -> skip current
    int reading1;             // variables to store readings from ldr's
    int reading2;
    // configure ambient
    Sprintln (F("LDR configuration tool:"));


    // set ambient brightness value
    Sprint(F("Set the ambient brightness, make sure neither ldr's covered"));
    while (Serial.available() == 0) { //wait until some serial data arrives
      // give prompt on value read from pins

      delay(500);
      Sprint(F("LDR 1 value: "));
      Sprint(analogRead(LDR.pin1));
      Sprint(F("LDR 2 value: "));
      Sprintln(analogRead(LDR.pin2));
    }

    accept_val = Serial.read();    //read one byte
    Serial.flush();                  //empty buffer
    if (accept_val == 'x')
      return (0);                      // if user quits, return 0

    if (accept_val != 's') {      // if not skipping this stage
      reading1 = analogRead(LDR.pin1);
      reading2 = analogRead(LDR.pin2);

      LDR.ambient = (reading1 + reading2) / 2; //set average as ambient

      if (abs(reading1 - reading2) >= LDR.large_disparity) //if large disparity, give warning
        Sprintln(F("Warning, large disparity on readings"));
    }


    //set maximum brightness value

    Sprint(F("Set the maximum brightness, make sure neither ldr's covered, shine light on ldr 1"));
    while (Serial.available() == 0) { //wait until some serial data arrives
      // give prompt on value read from pin, help to identify pin
      delay(500);
      Sprint(F("LDR 1 value: "));
      Sprintln(analogRead(LDR.pin1));
    }

    accept_val = Serial.read();
    Serial.flush();
    if (accept_val == 'x')
      return (0);                      // if user quits, return 0

    if (accept_val != 's') {

      LDR.max_reading1 = analogRead(LDR.pin1);

      Sprint(F("Set the maximum brightness, make sure neither ldr's covered, shine light on ldr 2"));
      while (Serial.available() == 0) { //wait until some serial data arrives
        // give prompt on value read from pin, help to identify pin
        delay(500);
        Sprint(F("LDR 2 value: "));
        Sprintln(analogRead(LDR.pin2));
      }

      accept_val = Serial.read();
      Serial.flush();
      if (accept_val == 'x')
        return (0);                      // if user quits, return 0



      LDR.max_reading2 = analogRead(LDR.pin2);


      if (abs(LDR.max_reading1 - LDR.max_reading2) >= LDR.large_disparity)
        Sprintln(F("Warning, large disparity on readings"));

    }

    //set minimum brightness value

    Sprint(F("Set the minimum brightness, cover ldr 1"));
    while (Serial.available() == 0) { //wait until some serial data arrives
      // give prompt on value read from pin, help to identify pin
      delay(500);
      Sprint(F("LDR 1 value: "));
      Sprintln(analogRead(LDR.pin1));
    }

    accept_val = Serial.read();
    Serial.flush();
    if (accept_val == 'x')
      return (0);                      // if user quits, return 0

    if (accept_val != 's') {

      LDR.min_reading1 = analogRead(LDR.pin1);

      Sprint(F("Set the maximum brightness, cover ldr 2"));
      while (Serial.available() == 0) { //wait until some serial data arrives
        // give prompt on value read from pin, help to identify pin
        delay(500);
        Sprint(F("LDR 2 value: "));
        Sprintln(analogRead(LDR.pin2));
      }

      accept_val = Serial.read();
      Serial.flush();
      if (accept_val == 'x')
        return (0);                      // if user quits, return 0



      LDR.min_reading2 = analogRead(LDR.pin2);


      if (abs(LDR.min_reading1 - LDR.min_reading2) >= LDR.large_disparity)
        Sprintln(F("Warning, large disparity on readings"));
    }

  }

  else {  //debug not enabled, cant provide feedback to user
    return (-2);
  }
}


int due_class::set_LDR_enable(int sensor) {  // toggle, useful if you dint care about ambient brightness, or sensor not working

  // if these are disabled, program should just assume max value
  // TO DO: add feature to manually adjust max value using the encoder when these are disabled

  if (LDR.LDR1_enable)
    LDR.LDR1_enable = false;


  if (LDR.LDR2_enable)
    LDR.LDR2_enable = false;

}


int due_class::set_LDR_disable(int sensor) {   //oppose of set_LRD_enable

  //use if in manual mode and want to dynamically adjust the brightness

  if (!LDR.LDR1_enable)
    LDR.LDR1_enable = true;


  if (!LDR.LDR2_enable)
    LDR.LDR2_enable = true;

}



int due_class::get_serial() {                   //function to interpret serial data recieved without a user prompt

  // function to interpret any serial data recieved during the main loop (ie without a user prompt)
  // the function should work on a key word basis whereby any recieved data should be of the form 'keyword: data'
  // where 'keyword' defines the type of data recieved, such as displayable string, instruction, sensor get request etc
  // and 'data' is any relevant string for the specified keyword. The function should return an error message if the keyword isnt defined.


  // an example would be: 'disp_string: dispaly this string'
  // this is defined by the disp_string keyword meaning the associated data should be pushed to the screen, then the data is the string to be pushed


}


//i2c functions

int due_class::send_disp_string_frame(int address) {   //function to send strings to display on screen

  // function calculates the number of frames required to send the string, then loops,
  // generates a frame hader and fills up to 27 bytes of the string and calculates the checksum
  // it also calls the send frame function to send it on to the specified address when frame complete
  
  int type = 1;

  text_cursor.x_min = -text.text_width*strlen(text_str)*2; // set this based on size of string being sent, will update if string changed

  frame.num_frames = 1 + (strlen(text_str) / 27); //send this many frames
  frame.this_frame = 1;

  do {    //loop to send multiple frames if string is long

    if (frame.num_frames != frame.this_frame)
      frame.frame_buffer[0]  = 32;  //if there are more than one frame to send, this

    else
      frame.frame_buffer[0]  = strlen(text_str) - ((frame.num_frames - 1) * 27) + 5; //remaining frame is string length-offset+5 bytes overhead


    frame.frame_buffer[1] = (byte) type;
    frame.frame_buffer[2] = (byte) frame.num_frames;
    frame.frame_buffer[3] = (byte) frame.this_frame;
    frame.checksum = frame.frame_buffer[0] + frame.frame_buffer[1] + frame.frame_buffer[2] + frame.frame_buffer[3] ;

    pack_disp_string_frame(type, frame.this_frame);//function to pack the frame with which ever data is relevant
    frame.frame_buffer[frame.frame_buffer[0] - 1] = (byte)256 - frame.checksum;
    wire_write_frame(address);

    frame.this_frame++;   //increment this_frame after sending, will prepare for next loop or break
    delayMicroseconds(100000);       //small delay, want reciever to read through its buffer, otherwise the buffer may overload when we send next frame

  } while (frame.this_frame <= frame.num_frames);
  
  return (0);
}

int due_class::pack_disp_string_frame(int frame_type, int frame_offset) {   //function to pack a frame of text to display

  // function to pack a frame based on a given offset (ie this frames number)
  // maybe generalise later to accept calls from multiple frame building methods

  frame_offset = ((frame_offset - 1) * 27); //if this frame is 1 offset in data set is 0, if 2 offset 27, etc

  if (frame_type == 1) { //send str variable
    for (int i = 0; i < strlen(text_str) - frame_offset; i++) {
      frame.frame_buffer[i + 4] = (byte)text_str[frame_offset + i];
      frame.checksum = frame.checksum + (byte)frame.frame_buffer[i + 4];
      if (i == 27) break;     //copy string untilend or 28 bytes copied
    }
  }

  // else{
  //   if (debug)
  //     Serial.println(F("Error from pack_disp_string_frame: can only pack type 1 frames"));
  // }

  return (0);
}

int due_class::wire_write_frame(int address) {   //function to actually send the frame to given address

  
  if (!mega_enabled[address - 1]) {
    
      Sprintln(F("Mega disabled, did not attempt transmission"));
      return (-1);
  }

  if (address == 1) {
    for (int i = 0; i < frame.frame_buffer[0]; i++) {
      Serial_1.write(frame.frame_buffer[i]);
 
    }
  }
  else if (address == 2) {
    for (int i = 0; i < frame.frame_buffer[0]; i++) {
      Serial_2.write(frame.frame_buffer[i]);
 
    }
  }
  else if (address == 3) {
    for (int i = 0; i < frame.frame_buffer[0]; i++) {
      Serial_3.write(frame.frame_buffer[i]);

    }
  }
  else  if (address == 4) {
    for (int i = 0; i < frame.frame_buffer[0]; i++) {
      Serial_4.write(frame.frame_buffer[i]);

    }
  }
  else {
    Sprintln(F("Address invalid"));
    return -1;
  }
  
  
  //clear frame from last iteration
  for (int beta = 0; beta < 32; beta++) {
    frame.frame_buffer[beta] = 0;
  }

  return (0);
}

int due_class::send_pos_frame(int address) {

  int type = 2;
  Sprint("Sending cursor positions to address ");
  Sprintln(address);

  frame.frame_buffer[0] = (byte) pos_frame_length;   //build frame
  frame.frame_buffer[1] = (byte) type;
  frame.frame_buffer[2] = (byte) 1;
  frame.frame_buffer[3] = (byte) 1;

  pack_xy_coordinates();        //seperate function to bit shift values to correct order.

  frame.frame_buffer[8] = (byte) x_pos_dir;
  frame.frame_buffer[9] = (byte) x_pos_dir;
  frame.frame_buffer[10] = (byte) comms_delay;
  frame.frame_buffer[11] = (byte) pos_update_freq;

  frame.frame_buffer[pos_frame_length - 1] = 0; //zero checksum
  for (int alpha = 0; alpha < pos_frame_length - 2; alpha++) { //sum of elements
    frame.frame_buffer[pos_frame_length - 1] = frame.frame_buffer[pos_frame_length - 1] + frame.frame_buffer[alpha];
  }
  frame.frame_buffer[pos_frame_length - 1] = (byte) 256 - (frame.frame_buffer[pos_frame_length - 1] % 256); //calc checksum

  wire_write_frame(address);    //send frame

  return (0);
  }

int due_class::pack_xy_coordinates() {       //function to pack the 4 bytes to send the x and y positions of the text cursor

  // Wire.write can only handle bytes (0-256) whereas we will need to use positive and negaitve values potientially significantly larger than 256
  // to accomplish this two sucessive bytes are sent to repersent one number. to deal with positive and negative numbers, the coordinate is split into
  // two bytes, leasat significant 8 bits are the second byte and the most significant 7 bits are in the first byte sent. if the coordinate is negaitve the
  // remaining bit is set 0 and 1 otherwise.

  // NOTE: current implementation will overflow if an out of bounds coordinate is presented (+/-32738 is usable)
  //       I cant see a reason this would be an issue so not fixing it for now

  if (abs(text_cursor.x) > 32738 || abs(text_cursor.y) > 32738) //print warning that coordinate will be wrong
    Sprintln("WARNING: failed to send correct coordinate, out of bounds, overflow likely to occur");

  if (text_cursor.x > 0) {
    x_pos_MSB = text_cursor.x / 256; //take the multiples 256 and set as MS byte
    x_pos_MSB = x_pos_MSB + 128; //greater than zero, set MSB to 1
    x_pos_LSB = text_cursor.x % 256; //take the modulo to get the LS byte
  }
  else {
    x_pos_MSB = text_cursor.x / 256; //take the multiples 256 and set as MS byte
    x_pos_LSB = text_cursor.x % 256; //take the modulo to get the LS byte
  }

  if (text_cursor.y > 0) {
    y_pos_MSB = text_cursor.y / 256; //take the multiples 256 and set as MS byte
    y_pos_MSB = y_pos_MSB + 128; //greater than zero, set MSB to 1
    y_pos_LSB = text_cursor.y % 256; //take the modulo to get the LS byte
  }
  else {
    y_pos_MSB = text_cursor.y / 256; //take the multiples 256 and set as MS byte
    y_pos_LSB = text_cursor.y % 256; //take the modulo to get the LS byte
  }


  frame.frame_buffer[4] = x_pos_MSB;   //write new values to frame
  frame.frame_buffer[5] = x_pos_LSB;
  frame.frame_buffer[6] = y_pos_MSB;
  frame.frame_buffer[7] = y_pos_LSB;

  return (0);
}

int due_class::send_all_calibration_data(int address) {     //function to send all data

  //function to send all the sensor data. loop through all sensor values
  byte type = 3;
  byte frameNum = 1;
  byte numFrames = ((sizeof(to_mega_prefix_array) * 2) / 26) + 1;
  int offset = 0;
  int val = 0;

  frame.frame_buffer[1] = type;        //set frame starting bytes
  frame.frame_buffer[2] = numFrames;
  frame.frame_buffer[3] = frameNum;

  for (int alpha = 0; alpha < sizeof(to_mega_prefix_array) + 1; alpha++) {

    if (alpha == sizeof(to_mega_prefix_array)) { //if last byte
      val = send_specific_calibration_data(to_mega_prefix_array[alpha],  address, false,  offset);   //indicate this is the last element

    }
    else
      val = send_specific_calibration_data(to_mega_prefix_array[alpha],  address, true, offset);    //pack byte and dont send

    if (val == 0)  //if the frame was sent (function returns 1), reset offset otherwise increment
      offset++;

    else if (val == 1) {
      frameNum++;     //increment the frame number
      offset = 0;     //reset offset for new frame

      frame.frame_buffer[1] = type;        //set frame starting bytes
      frame.frame_buffer[2] = numFrames;
      frame.frame_buffer[3] = frameNum;
    }

  }
  
  return (0);

}

int due_class::send_specific_calibration_data(byte sensor_prefix, int address, bool more_bytes, int offset) { //sensor to send specific value

  // function to pack a frame with specific sensor data. the bool more_bytes can be used if htis is called as part of a loop to send more than one value
  // in the case that more_bytes is true it will hold off sending the frame until it is called and is false. offset is the number of sensor readings previously
  // written, so the place to write the new data is 4+2*offset for the prefix and 5+2*offset for the data
  byte type = 3;


  //switch statement to pack the frame;
  switch (sensor_prefix) {
    case 10:  frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = current1;
      break;

    case 11:  frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = current2;
      break;

    case 20:  frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = temp1;
      break;

    case 21:  frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = temp2;
      break;

    case 22:  frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = temp3;
      break;

    case 30:  frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = light1;
      break;

    case 31:  frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = light2;
      break;

    case 40:  frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = fan_speed;
      break;

    case 50:  frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = led_strip_brightness;
      break;

    case 60:  frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = sd_card1_detected ? (byte) 1 : (byte) 0;   //convert boolean to byte
      break;

    case 61:  frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = sd_card2_detected ? (byte) 1 : (byte) 0;
      break;

    case 70:  frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = ethernet_connected ? (byte) 1 : (byte) 0;
      break;

    case 80:  frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = wifi_connected ? (byte) 1 : (byte) 0;
      break;

    case 90:  frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = screen_brightness;
      break;

    case 100: frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = text_size;
      break;

    case 110: frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = text_colour_r;
      break;


    case 120: frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = text_colour_g;
      break;


    case 130: frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = text_colour_b;
      break;

    case 140: frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = (byte)get_text_colour_hue(1);   //function to ge the MS byte or LS byte, 1 returns MSB, 2 returns LSB
      break;

    case 150: frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = (byte)get_text_colour_hue(2);
      break;

    case 160: frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = use_hue ? (byte) 1 : (byte) 0;
      break;

    case 170: frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.frame_buffer[5 + 2 * offset] = debug ? (byte) 1 : (byte) 0;
      break;

    case 180: frame.frame_buffer[4 + 2 * offset] = sensor_prefix;
      frame.  frame_buffer[5 + 2 * offset] = screen_mode;
      break;

    default:  Sprint("Error: Prefix not defined. Prefix :");
      Sprintln(sensor_prefix);

  }


  if (more_bytes && (4 + (offset * 2)) <= 29) { //this round element 29 an 30 written (ok), next round 30 and 31 writted then full
    return (0);
  }
  else {
    frame.frame_buffer[0] = 5 + (offset * 2);
    frame.checksum = 0; //calculate checksum
    for (int alpha = 0; alpha < frame.frame_buffer[0] - 1; alpha++) {
      frame.checksum = frame.checksum + frame.frame_buffer[alpha];
    }
    frame.frame_buffer[frame.frame_buffer[0] - 1] = frame.checksum;
    wire_write_frame(address);    //send frame
    return (1);                    //frame_sent, send notification back
  }

}

int get_text_colour_hue(int byte_number) {  //function to retrun the MSB or LSB of the current hue value to send over i2c

  if (byte_number == 1) { //looking for MSB
    if (text_colour_hue < 0)
      return (abs(text_colour_hue) / 256);    //get quotient of absolute value and 256 rounded down

    else
      return (abs(text_colour_hue) / 256 + 128); //add 128 to indicate positve number
  }
  else if (byte_number == 2) { //LSB
    return (abs(text_colour_hue) % 256);    //get modulo of value and 256;
  }
  else {
    Sprintln("Error, cant get hue MSB/LSB, invalid byte number presented");
    return (-1);
  }
}


int due_class::send_all_pos() {    // function to send pos data to all megas if timer interrupt has indicated they should be sent

    if (!timers.pos_timer_attached){
      Sprintln(F("Error: Cant send pos, timer not enabled"));
      return(-1);
    }
  
    if (send_pos_now){    //send pos based on timer interrupt variable
     send_pos_frame(1);   // send frames
     send_pos_frame(2);
     send_pos_frame(3);
     send_pos_frame(4);    
     send_pos_now=false;
  }
  return(0);
}

int due_class::send_all_pos_now(){    //function to send all the positional info now, not wait for timer to trigger flag
  
     send_pos_frame(1);   // send frames
     send_pos_frame(2);
     send_pos_frame(3);
     send_pos_frame(4);    
  
  return(0);
}

int due_class::calc_delay() {    // function to calculate the dalay in sending frames to the megas
  // this could be useful for syncing up the screen updates. this value is only necessary for the pos data
  //frame as the other ones dont require accurate timing

  int beta = millis();

  send_all_pos_now();
  beta = millis() - beta;
  comms_delay = beta / 4; //time to send one frame

  return (0);
}


int due_class::send_all_text() {   // send the text frame to all megas

  if (millis()>mega_last_sent_time+2000){
    mega_last_sent_time = millis();
    for (int alpha = 1; alpha <= 4; alpha++) {
     
        Sprint(F("Sending text frame to address "));
        Sprintln(alpha);
      
      int fail = send_disp_string_frame(alpha);
      if (fail != 0) {
        Sprint(F("Failed to send string to mega"));
        Sprintln(alpha);
        return (-1);
      }
    }
  }
  return (0);
}

int due_class::print_frame() {
  //if (debug){
  for (int alpha = 0; alpha < frame.frame_buffer[0]; alpha++) {
    Sprint((char)frame.frame_buffer[alpha]);
  }
  Sprintln("");
  //}
  return (0);
}

#endif
