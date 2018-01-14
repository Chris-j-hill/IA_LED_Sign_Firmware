
#ifndef Sign_coms_i2c_H
#define Sign_coms_i2c_H

#include "Global_variables.h"
#include "Config_Local.h"
class coms {

  private:
    

  public:

    coms(){}

    int init_serial();            // init the serial at 115200 baud rate
    int init_serial(int speed);   // init the serial at a custom speed
    int init_i2c();               // higher level function to initialise i2c
    int init_i2c(int address);
    int init_i2c_due();           // initialise i2c for the due
    int init_i2c_mega(int address);          // initialise 12c for the megas
    int i2c_scanner();            // scan all addresses and make sure decive responds
    int i2c_scanner(int address); // test specific address

    int generate_checksum();
    int error_check();

    int get_frame_code();
    int get_data();             //extract data from frame

    int startup_handshake();      //startup sequence to ensure due boots first and transmission begins when all megas are ready




    
    int get_serial();                     //function to interpret serial data recieved without a user prompt

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


//methods

int coms::init_serial() {      // initialise serial at set baud rate speed of 115200

  int speed = 115200;
  //if (debug){      //if debug mode, init serial, if debug false dont bether with this
  int alpha = millis();

  //Serial.begin(speed);
  while (!Serial) {
    if (millis() > alpha + 5000) {  //after 5 seconds elapsed, assume serial failed to initialise
      debug = false;
      return -1;
    }
  }

  //init four soft serial objects on due
  Serial_1.begin(
    RX_PIN_1,
    TX_PIN_1,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
  Serial_2.begin(
    RX_PIN_2,
    TX_PIN_2,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
  Serial_3.begin(
    RX_PIN_3,
    TX_PIN_3,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
  Serial_4.begin(
    RX_PIN_4,
    TX_PIN_4,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
  return 0;

}
#ifdef USE_SERIAL_TO_MEGAS
int coms::init_serial(int speed) {   // initialise serial at specified speed, must be standardised speed 115200 or below, otherwise error thrown
  if (speed != 300 && speed != 600 && speed != 1200 && speed != 2400 && speed != 4800 && speed != 14400 && speed != 9600 && speed != 14400 && speed != 19200 && speed != 28800 && speed != 38400 && speed != 57600 && speed != 115200)
    return (-2);
  //if (debug){      //if debug mode, init serial, if debug false dont bether with this
  int alpha = millis();

  Serial.begin(speed);
  while (!Serial) {
    if (millis() > alpha + 5000) {  //after 5 seconds elapsed, assume serial failed to initialise
      debug = false;
      return -1;
    }
  }
  return 0;
  // }
  //   else
  //   debug = false;
  //   return 0;
}

#endif

#ifdef USE_I2C_TO_MEGAS
int coms::init_i2c() { //if init does not specify address call with address 5(safe value), if due, value not used

  init_i2c_due();   //attach the i2c to wire1
  //Serial.println(F("Pause for megas to initialise"));
  //delay(500);
  //init_i2c(0);      //check all i2c devices activated

}

int coms::init_i2c(int address) {   //initialise i2c by calling specify routines for this chipset


  int alpha = i2c_scanner();   //test all addresses
  if (alpha != 0)
    Serial.println(F("Failed to detect 4 megas on i2c"));
  //alpha = init_i2c_due();

  if (alpha < 0) {     //error if less than 0
    
      Sprintln("error setting up i2c, stopping program");
      Sprintln("Error code: " + alpha);
    
    while (1) {}    //stop code
  }

}

int coms::init_i2c_due() {  //i2c init for the due specifically

  //ensure the device running this is a due

  Sprintln(F("Error, runinng due i2c init but using mega chipset"));
  return (-1);

}

int coms::i2c_scanner() {           // scan all addresses and make sure decive responds

  // based on the i2c scanner methodology send dummy frame to mega
  // check endTransmission() return value, if non zero there is an error
  if (test_i2c) {
    int fail;
    for (int i = 1; i <= num_screens; i++) {
      fail = i2c_scanner(i);
      if (fail != 0) {
          Sprint(F("Error connecting to i2c device:"));
          Sprintln(i);
        
        return (-1);
      }

    }
   
    Sprintln(F("i2c return success"));
    return (0);
  }
  else
    return (0);

}

int coms::i2c_scanner(int address) { // test specific address

  Wire1.beginTransmission(address);
  //for(int i=0; i<strlen(dummy_frame);i++){
  //Wire1.write(dummy_frame[i]);
  //}
  int fail = Wire1.endTransmission();
  return fail;

}
#endif

int coms::startup_handshake() {  //code to delay the due in initialisation and enable mega startup, simply wait until the megas all set ready pin high

  pinMode(due_ready_pin, OUTPUT);           //due handshake
  pinMode(mega1_ready_pin, INPUT_PULLUP);
  pinMode(mega2_ready_pin, INPUT_PULLUP);
  pinMode(mega3_ready_pin, INPUT_PULLUP);
  pinMode(mega4_ready_pin, INPUT_PULLUP);

  digitalWrite(due_ready_pin, HIGH);

  bool ready = false;
  int startup_time = millis();
  while (!ready) {

    bool mega1_ready = !digitalRead(mega1_ready_pin);
    bool mega2_ready = !digitalRead(mega2_ready_pin);
    bool mega3_ready = !digitalRead(mega3_ready_pin);
    bool mega4_ready = !digitalRead(mega4_ready_pin);

    if (!mega_enabled[0]) mega1_ready = true;
    if (!mega_enabled[1]) mega2_ready = true;
    if (!mega_enabled[2]) mega3_ready = true;
    if (!mega_enabled[3]) mega4_ready = true;

    //Serial.println(mega1_ready);
    if (mega1_ready && mega2_ready && mega3_ready && mega4_ready)
      ready = true;

    if (millis() > startup_time + 5000) { //provide ample time for mega to start
      Serial.println(F("Startup time exceeded, mega(s) not responding"));
      return (-1);
    }
  }
  return (0);






}


int coms::get_serial() {                   //function to interpret serial data recieved without a user prompt

  // function to interpret any serial data recieved during the main loop (ie without a user prompt)
  // the function should work on a key word basis whereby any recieved data should be of the form 'keyword: data'
  // where 'keyword' defines the type of data recieved, such as displayable string, instruction, sensor get request etc
  // and 'data' is any relevant string for the specified keyword. The function should return an error message if the keyword isnt defined.


  // an example would be: 'disp_string: dispaly this string'
  // this is defined by the disp_string keyword meaning the associated data should be pushed to the screen, then the data is the string to be pushed


}

int coms::send_disp_string_frame(int address) {   //function to send strings to display on screen

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

int coms::pack_disp_string_frame(int frame_type, int frame_offset) {   //function to pack a frame of text to display

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

int coms::wire_write_frame(int address) {   //function to actually send the frame to given address

  
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

int coms::send_pos_frame(int address) {

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

int coms::pack_xy_coordinates() {       //function to pack the 4 bytes to send the x and y positions of the text cursor

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

int coms::send_all_calibration_data(int address) {     //function to send all data

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

int coms::send_specific_calibration_data(byte sensor_prefix, int address, bool more_bytes, int offset) { //sensor to send specific value

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

int coms::send_all_pos() {    // function to send pos data to all megas if timer interrupt has indicated they should be sent

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

int coms::send_all_pos_now(){    //function to send all the positional info now, not wait for timer to trigger flag
  
     send_pos_frame(1);   // send frames
     send_pos_frame(2);
     send_pos_frame(3);
     send_pos_frame(4);    
  
  return(0);
}

int coms::calc_delay() {    // function to calculate the dalay in sending frames to the megas
  // this could be useful for syncing up the screen updates. this value is only necessary for the pos data
  //frame as the other ones dont require accurate timing

  int beta = millis();

  send_all_pos_now();
  beta = millis() - beta;
  comms_delay = beta / 4; //time to send one frame

  return (0);
}


int coms::send_all_text() {   // send the text frame to all megas

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

int coms::print_frame() {
  //if (debug){
  for (int alpha = 0; alpha < frame.frame_buffer[0]; alpha++) {
    Sprint((char)frame.frame_buffer[alpha]);
  }
  Sprintln("");
  //}
  return (0);
}




#endif  //Sign_coms_i2c_H

