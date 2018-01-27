//#ifndef MEGA_H
//#define MEGA_H
//// class for the all mega functionality 
//#include "libraries.h"
//
//#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
//
//class mega_class : public sign_coms {
//
//private:                //initialise private fields
//
//    
//  
//    int last_sent_code =-1;
//    String Current_string = "Default class string1";
//    String Last_string = "Default class string2";
//    int cursor_x=-1;
//    int cursor_y =-1;
//    int offset_x=0;
//    int offset_y=0;
//    int Brightness =1;
//    int delay = 0;
//    int last_frame[150] = {0};          //full frame, ready to resend if needed
//    int nack_pin = -1;
//    int text_size=2;
//    int text_colour=-1;
//
//public:                 //methods of fractions class
//    mega_class ();             //constructor
//    int init_mega();
//    int init_mega(int address);
//    int init_matrix();
//    int init_matrix(int address);
//    
//    
//    //send data -> all methods should return -1 if fail can be identified
//    int increment(int address, int x, int y); //send code to increment cursor position
//
//    int send_string();     // send the "Current_sting" variable
//    int resend_frame();    // if nack recieved, transmit last frame, confirms checksum
//
//    int update_brightness();
//    int zero_cursor_pos();      //set cursor pos to 0,0 of overall sign (relative position to be calculated on mega)
//
//    //int begin();            // method to establish connection is valid, set up mega with new values
//    int set_delay();            //delay used to syncronie updates of positions on megas, measure delay first
//    int send_bad_frame();       //generate a bad frame to conirm nack system works (and identify nack pin if needed)
//
//    int set_refresh_rate(int rate);  //refresh rate of matrix library ( modify library to access this value)
//
//    int set_text_size();
//    int set_text_colour();
//
//    int i2c_scanner();          // find connection pins for megas (ie verify valid address, send bad frame, check for nack line)
//
//    int text_min();
//
//    int extract_data();      //function to respond to a received frame and set variables based on what type of frame arrived
//    int extract_pos_data();      //frame for extracting the relevant info for he cursor position
//    int extract_disp_string(byte numFrames, byte frameNum, byte frameLength); //extract the frame as a string to display
//    int extract_sensor_data();
//    int set_hue_colour(byte receivedData, int dataType);
//
//
//};
//
//
//
//
////methods for mega class
//
//mega_class::mega_class(){}
//
//
//
//int mega_class::init_mega(){
//  //find address
//  int fail = init_mega(5);
//}
//
//
//int mega_class::init_mega(int address){
//
//    offset_x = (address-1)*64;    //set offsets. position data recieved must be shifted by this
//    offset_y = 0;
//
//    int fail = sign_coms::init_serial();
//    // if (fail!=0) 
//    // sign_coms::stop();
//    Sprintln(F("Serial initialised"));
//    //sign_coms::init_i2c();
//    // if (fail!=0) 
//    // sign_coms::stop();
//    
//    //fail = init_matrix();
//    //if (fail!=0) 
//    //sign_coms::stop();
//
//    
//}
//
//int mega_class::init_matrix(){
////  matrix.begin();
////  matrix.setTextWrap(false); // Allow text to run off right edge
////  matrix.setTextSize(text_size);
//}
//
//int mega_class::extract_data() {      //function to respond to a received frame and set variables based on what type of frame arrived
//
//  
//  byte numFrames = (byte)frame.frame_str[2];    //local variables used to remove headers and trailers
//  byte frameNum =  (byte)frame.frame_str[3];
//  byte frameType = (byte)frame.frame_str[1];
//  byte frameLength = (byte)frame.frame_str[0];
//
//
//  //Serial.println("Extracting data");
//
//  if (frameType == 1) {
//    Sprintln("Frame type 1, extracting string");
//    extract_disp_string(numFrames, frameNum, frameLength);
//    
//  }
//
//  else if (frameType == 2) {  //extract positions
//    Sprintln("Frame type 2, extracting positions");
//    extract_pos_data();
//  }
//
//  else if (frameType==3){
//    Sprintln("Frame type 3, extracting calibration data");
//    extract_sensor_data();
//  }
//  else
//  Sprintln("Frame type not recognised");
//
//
//
//  //else if...
//
//
//  //rewrite frame_str to zeros
//  for (int gamma = 0; gamma < strlen(frame.frame_str); gamma++)
//    frame.frame_str[gamma] = 0;
//
//  frame.frame_end = false;
//}
//
//int mega_class::extract_disp_string(byte numFrames, byte frameNum, byte frameLength) {  //extract the frame as a string to display
//
//  // currently this function only extracts the data and returns 0 when done. maybe do
//  // checksum calculation of all frames and verify no errors. currently no callback method for
//  // dealing with errors so no need to calculate checksum (if its wrong theres nothing can be done)
//
//
//  int alpha = 0;
//  int beta = 0;
//
//  //frame.disp_str.reserve((frame.frame_str[2] - 1) * 27) + (frame.frame_str[(frame.frame_str[2] - 1) * 32] - OVERHEAD)+1;
//  for (alpha = 0; alpha < numFrames; alpha++) { //loop data, based on number of frames transmitted
//
//    for (beta = 0; beta < frame.frame_str[alpha * 32] - OVERHEAD; beta++) { //loop from index zero to frame length-overhead (usually 27 unless not full frame)
//
//      frame.disp_str[beta + alpha * (32 - OVERHEAD)] = frame.frame_str[beta + 4 + (alpha * 32)]; //copy data
//      Sprint((char)frame.frame_str[beta + 4 + (alpha * 32)]);
//    }
//  }
//  Sprintln();
//  //Serial.println(((frame.frame_str[2] - 1) * 27) + (frame.frame_str[(frame.frame_str[2] - 1) * 32] - OVERHEAD)+1);
//  //frame.disp_str[((frame.frame_str[2] - 1) * 27) + (frame.frame_str[(frame.frame_str[2] - 1) * 32] - OVERHEAD)] = '\0';
//  frame.disp_str.remove(((frame.frame_str[2] - 1) * 27) + (frame.frame_str[(frame.frame_str[2] - 1) * 32] - OVERHEAD));
//
//  return (0);
//}
//
//int mega_class::extract_pos_data() {      //frame for extracting the relevant info for he cursor position
//
//  x_pos_MSB = (byte)frame.frame_str[4];           // current positions
//  x_pos_LSB = (byte)frame.frame_str[5];           
//  y_pos_MSB = (byte)frame.frame_str[6];           
//  y_pos_LSB = (byte)frame.frame_str[7];           
//  x_pos_dir = (byte)frame.frame_str[8];       // direction text is moving
//  y_pos_dir = (byte)frame.frame_str[9];
//  comms_delay = (byte)frame.frame_str[10];     //delay between recieving new pos data and applying it to the screen
//  pos_update_freq = (byte)frame.frame_str[11]; //frequency the text position will change in specified direction (useful if not sending frame for every pos update)
//
//  if (x_pos_MSB>=128)    //positive number
//    x_pos= ((x_pos_MSB-128)*256) + x_pos_LSB;
//  
//  else //negative
//    x_pos= -1*((x_pos_MSB*256) + x_pos_LSB);
//  
//
//  if (y_pos_MSB>=128)    //positive number
//    y_pos= ((y_pos_MSB-128)*256) + y_pos_LSB;
//  
//  else //negative
//    y_pos= -1*((y_pos_MSB*256) + y_pos_LSB);
//  
//  x_pos = x_pos-64*(address-1);   //correct position data for this mega
////  Serial.print("Pos x = ");
////  Serial.print(x_pos);
////  Serial.print("Pos y = ");
////  Serial.print(y_pos);
//  return (0);
//}
//
//int mega_class::extract_sensor_data(){
//
//  int frameLength = frame.frame_str[0];
//  
//  for (int alpha =4; alpha<frameLength-3; alpha= alpha+2){    //step through frame, identify prefix and extract following data byte
//
//     //set the value of a variable based on what the prefix is
//    switch (frame.frame_str[alpha]) {
//      case 10:  current1 = frame.frame_str[alpha+1];
//                break;
//                
//      case 11:  current2 = frame.frame_str[alpha+1];
//                break;
//                
//      case 20:  temp1 = frame.frame_str[alpha+1];
//                break; 
//                   
//      case 21:  temp2 = frame.frame_str[alpha+1];
//                break;
//                
//      case 22:  temp3 = frame.frame_str[alpha+1];
//                break;
//                
//      case 30:  light1 = frame.frame_str[alpha+1];
//                break;
//  
//      case 31:  light2 = frame.frame_str[alpha+1];
//                break;
//  
//      case 40:  fan_speed = frame.frame_str[alpha+1];
//                break;
//                
//      case 50:  led_strip_brightness = frame.frame_str[alpha+1];
//                break;
//                
//      case 60:  sd_card1_detected = (frame.frame_str[alpha+1]==1)? true : false;   //convert byte to boolean
//                break;
//  
//      case 61:  sd_card2_detected = (frame.frame_str[alpha+1]==1)? true : false;
//                break;
//  
//      case 70:  ethernet_connected = (frame.frame_str[alpha+1]==1)? true : false;
//                break; 
//                
//      case 80:  wifi_connected = (frame.frame_str[alpha+1]==1)? true : false;
//                break; 
//                
//      case 90:  screen_brightness = frame.frame_str[alpha+1];
//                break;
//                    
//      case 100: text_size = frame.frame_str[alpha+1];
//                break;
//                   
//      case 110: text_colour_r = frame.frame_str[alpha+1];
//                break;
//  
//                
//      case 120: text_colour_g = frame.frame_str[alpha+1];
//                break;
//  
//      
//      case 130: text_colour_b = frame.frame_str[alpha+1];
//                break;
//                
//      case 140: text_colour_hue = set_hue_colour(frame.frame_str[alpha+1], 1);   //function to set the value of hue based on the msb or lsb received
//                break;
//      
//      case 150: text_colour_hue = set_hue_colour(frame.frame_str[alpha+1], 2);
//                break;
//  
//      case 160: use_hue = (frame.frame_str[alpha+1]==1)? true : false;
//                break;
//  
//      case 170: debug = (frame.frame_str[alpha+1]==1)? true : false;
//                break;
//
//      case 180: screen_mode = frame.frame_str[alpha+1];
//                break;
//                   
//      default:  Sprint("Error: recieved prefix not defined. Prefix :");
//                Sprintln((byte)frame.frame_str[alpha+1]);
//  
//      
//    }
//  }
//  return(0);
//}
//
//int mega_class::set_hue_colour(byte receivedData, int dataType){
//  int temp; //variable to hold the value of the part of text_colour_hue not being overwritten
//  
//  if (dataType==1){    //MSB
//    temp = text_colour_hue%256;   //store the LSB
//    if(receivedData>=128)
//    text_colour_hue = (receivedData-128)*256+temp; 
//    
//    else
//    text_colour_hue = -1*((receivedData*256)+temp);
//  }
//  else //LSB
//    temp = text_colour_hue/256;   // store the MSB, remainder that is lost is the LSB
//    text_colour_hue = temp*256+receivedData; // sum together MSB and LSB
//    
//  
//  return(0);
//}
//
//int mega_class::text_min(){
//  return(-1*text.text_width*frame.disp_str.length());
//}
//
//
//#endif  //if chipset defined
//
//#endif  //if library defined
