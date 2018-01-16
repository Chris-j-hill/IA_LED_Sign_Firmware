
#ifndef Coms_CPP
#define Coms_CPP

#include "Coms.h"
#include "Arduino.h"

// ______  non class functions _______

int attach_timer_pos_update(){
  //attach pos update interrupt
    if (!timers.pos_timer_attached){
      timers.pos_timer_attached = true;       //indicate the timer is attached
      
      Timer3.attachInterrupt(send_pos_interrupt);   //attach ISR
      int fail = set_pos_update_frequency(pos_update_freq);         // set the freq
      
      if (fail !=0){
        Sprintln(F("Failed to attach pos timer"));
        timers.pos_timer_attached = false;       
        return(-1);     //stop code   
      }
     
      Timer3.start();
      Sprintln(F("Attached pos timer"));
    }
}

int set_pos_update_frequency(int freq){
  
  if (!timers.pos_timer_attached){
      Sprintln(F("From 'set_pos_update_frequency': trying to set frequency but timer not attached"));
      return (-1);
  }
  else{   //all good, set freq
    
    pos_update_freq = freq;   //variable for frame
    
    Timer3.setFrequency(freq);   //set interval
    
    return(0);
  }

}





// ______ class functions _______

int Coms::init_software_serial_to_usb_port(){           // init the serial at 115200 baud rate

  Serial.begin(COMS_SPEED)
  alpha=millis();
  while (!Serial){
    if (millis() > alpha + WAIT_TIME_FOR_USB_PORT_CONNECTION) {  //after 5 seconds elapsed, assume serial failed to initialise
      debug = false;
      return -1;
    }
  }
  
  return 0; 
}

int Coms::init_software_serial_to_usb_port(int speed){  // init the serial at a custom speed

  if (speed != 300 && speed != 600 && speed != 1200 && speed != 2400 && speed != 4800 && speed != 14400 && speed != 9600 && speed != 14400 && speed != 19200 && speed != 28800 && speed != 38400 && speed != 57600 && speed != 115200)
    return (-2);
  
  Serial.begin(speed)
  alpha=millis();
  while (!Serial){
    if (millis() > alpha + WAIT_TIME_FOR_USB_PORT_CONNECTION) {  //after 5 seconds elapsed, assume serial failed to initialise
      debug = false;
      return -1;
    }
  }

  return 0;
}

int Coms::startup_handshake() {  //code to delay the due in initialisation and enable mega startup, simply wait until the megas all set ready pin high

#ifdef USING_SIMPLE_BINARY_NODE_READY_PROTOCOL
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
#else

  delay(STARTUP_DUE_DELAY_PERIOD);

#endif
}

int Coms::send_disp_string_frame(int address) {   //function to send strings to display on screen

  // function calculates the number of frames required to send the string, then loops,
  // generates a frame hader and fills up to 27 bytes of the string and calculates the checksum
  // it also calls the send frame function to send it on to the specified address when frame complete
  
  int type = 1;

  text_cursor.x_min = -text.text_width*strlen(text_str)*2; // set this based on size of string being sent, will update if string changed

  frame.num_frames = 1 + (strlen(text_str) / (FRAME_DATA_LENGTH)); //send this many frames
  frame.this_frame = 1;

  do {    //loop to send multiple frames if string is long

    if (frame.num_frames != frame.this_frame)
      frame.frame_buffer[0]  = MEGA_SERIAL_BUFFER_LENGTH;  //if there are more than one frame left to send, this frame is max size

    else
      frame.frame_buffer[0]  = strlen(text_str) - ((frame.num_frames - 1) * (FRAME_DATA_LENGTH)) + (HEADER_LENGTH + TRAILER_LENGTH); //remaining frame is string length-text offset+5 bytes overhead


    frame.frame_buffer[1] = (byte) type;
    frame.frame_buffer[2] = (byte) frame.num_frames;
    frame.frame_buffer[3] = (byte) frame.this_frame;
    frame.checksum = frame.frame_buffer[0] + frame.frame_buffer[1] + frame.frame_buffer[2] + frame.frame_buffer[3] ;

    pack_disp_string_frame(type, frame.this_frame);//function to pack the frame with which ever data is relevant
    frame.frame_buffer[frame.frame_buffer[0] - 1] = (byte)256 - frame.checksum;
    write_frame(address);

    frame.this_frame++;   //increment this_frame after sending, will prepare for next loop or break
    delayMicroseconds(100000);       //small delay, want reciever to read through its buffer, otherwise the buffer may overload when we send next frame

  } while (frame.this_frame <= frame.num_frames);
  
  return (0);
}

int Coms::pack_disp_string_frame(int frame_type, int frame_offset) {   //function to pack a frame of text to display

  // function to pack a frame based on a given offset (ie this frames number)
  // maybe generalise later to accept calls from multiple frame building methods

  frame_offset = ((frame_offset - 1) * (FRAME_DATA_LENGTH)); //if this frame is 1 offset in data set is 0, if 2 offset 27, etc

  if (frame_type == 1) { //send str variable
    for (int i = 0; i < strlen(text_str) - frame_offset; i++) { //loop through string until end or break
      frame.frame_buffer[i + HEADER_LENGTH] = (byte)text_str[frame_offset + i];
      frame.checksum = frame.checksum + (byte)frame.frame_buffer[i + HEADER_LENGTH];
      if (i == 27) break;     //copy string untilend or 28 bytes copied
    }
  }

  return (0);
}

int Coms::send_pos_frame(int address) {

  int type = 2;
  Sprint(F("Sending cursor positions to address "));
  Sprintln(address);

  frame.frame_buffer[0] = (byte)  ;   //build frame
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

  write_frame(address);    //send frame

  return (0);
  }

int Coms::pack_xy_coordinates() {       //function to pack the 4 bytes to send the x and y positions of the text cursor

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

int Coms::send_all_calibration_data(int address) {     //function to send all data

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

int Coms::send_specific_calibration_data(byte sensor_prefix, int address, bool more_bytes, int offset) { //sensor to send specific value

  // function to pack a frame with specific sensor data. the bool more_bytes can be used if htis is called as part of a loop to send more than one value
  // in the case that more_bytes is true it will hold off sending the frame until it is called and is false. offset is the number of sensor readings previously
  // written, so the place to write the new data is 4+2*offset for the prefix and 5+2*offset for the data
  byte type = 3;
  int HEADER_PLUS_ONE = HEADER_LENGTH+1;

  //switch statement to pack the frame;
  switch (sensor_prefix) {
    case 10:  frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = current1;
      break;

    case 11:  frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = current2;
      break;

    case 20:  frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = temp1;
      break;

    case 21:  frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = temp2;
      break;

    case 22:  frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = temp3;
      break;

    case 30:  frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = light1;
      break;

    case 31:  frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = light2;
      break;

    case 40:  frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = fans.fan1.fan_target_speed;
      break;

    case 50:  frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = led_strip.led_strip1.target_brightness;
      break;

    case 60:  frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = sd_card1_detected ? (byte) 1 : (byte) 0;   //convert boolean to byte
      break;

    case 61:  frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = sd_card2_detected ? (byte) 1 : (byte) 0;
      break;

    case 70:  frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = ethernet_connected ? (byte) 1 : (byte) 0;
      break;

    case 80:  frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = wifi_connected ? (byte) 1 : (byte) 0;
      break;

    case 90:  frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = screen_brightness;
      break;

    case 100: frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = text_size;
      break;

    case 110: frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = text_colour_r;
      break;


    case 120: frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = text_colour_g;
      break;


    case 130: frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = text_colour_b;
      break;

    case 140: frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = (byte)get_text_colour_hue(1);   //function to ge the MS byte or LS byte, 1 returns MSB, 2 returns LSB
      break;

    case 150: frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = (byte)get_text_colour_hue(2);
      break;

    case 160: frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = use_hue ? (byte) 1 : (byte) 0;
      break;

    case 170: frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = debug ? (byte) 1 : (byte) 0;
      break;

    case 180: frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      frame.  frame_buffer[HEADER_PLUS_ONE + 2 * offset] = screen_mode;
      break;

    default:  Sprint("Error: Prefix not defined. Prefix :");
      Sprintln(sensor_prefix);

  }


  if (more_bytes && (HEADER_LENGTH + (offset * 2)) <= 29) { //this round element 29 an 30 written (ok), next round 30 and 31 writted then full
    return (0);
  }
  else {
    frame.frame_buffer[0] = FRAME_OVERHEAD + (offset * 2);
    frame.checksum = 0; //calculate checksum
    for (int alpha = 0; alpha < frame.frame_buffer[0] - 1; alpha++) {
      frame.checksum = frame.checksum + frame.frame_buffer[alpha];
    }
    frame.frame_buffer[frame.frame_buffer[0] - 1] = frame.checksum;
    write_frame(address);    //send frame
    return (1);                    //frame_sent, send notification back
  }

}

int Coms::send_all_pos_on_interrupt() {    // function to send pos data to all megas if timer interrupt has indicated they should be sent

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

int Coms::send_all_pos_now(){    //function to send all the positional info now, not wait for timer to trigger flag
  
     send_pos_frame(1);   // send frames
     send_pos_frame(2);
     send_pos_frame(3);
     send_pos_frame(4);    
  
  return(0);
}

int Coms::calc_delay() {    // function to calculate the dalay in sending frames to the megas
  // this could be useful for syncing up the screen updates. this value is only necessary for the pos data
  //frame as the other ones dont require accurate timing

  int beta = millis();

  send_all_pos_now();
  beta = millis() - beta;
  comms_delay = beta / 4; //time to send one frame

  return (0);
}

int Coms::send_all_text() {   // send the text frame to all megas

  if (millis()>mega_last_sent_time+TEXT_TRANSMIT_PERIOD){
    mega_last_sent_time = millis();
    for (int alpha = 1; alpha <= NUM_SCREENS; alpha++) {
     
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

void Coms::print_frame() {
  for (int alpha = 0; alpha < frame.frame_buffer[0]; alpha++) {
    Sprint((char)frame.frame_buffer[alpha]);
  }
  Sprintln("");
}

int Coms::write_frame(int address){

#if defined(USE_SERIAL_TO_MEGAS)
serial_write_frame(address);
#elif defined(USE_I2C_TO_MEGAS)
wire_write_frame(address);
#else
#error "Define a coms protocol"
#endif
}




int get_text_colour_hue(int byte_number) {  //function to return the MSB or LSB of the current hue value to send over i2c

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

#endif // Coms_CPP
