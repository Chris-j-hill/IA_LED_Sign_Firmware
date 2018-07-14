

#ifndef COMS_H
#define COMS_H

#include "Local_Config.h"
#include "Arduino.h"

#define COMS_SPEED 38400         //speed of coms between due and megas when using serial


#define HEADER_LENGTH 4  //length,type,num frames, frame no
#define TRAILER_LENGTH 1 //just checksum
#define DATA_IDENTIFIER_BYTE  2   //frame type byte
#define FRAME_LENGTH_BYTE  1
#define FRAME_DATA_LENGTH MEGA_SERIAL_BUFFER_LENGTH-HEADER_LENGTH-TRAILER_LENGTH
#define FRAME_OVERHEAD HEADER_LENGTH+TRAILER_LENGTH        //number of overhead bytes -> frame length, frame type, num frames, frame num, checksum

#define WAIT_TIME_FOR_USB_PORT_CONNECTION 5000

#define MEGA_SERIAL_BUFFER_LENGTH 32
//#define MAX_TWEET_SIZE 140
#define MAX_FRAME_SIZE MAX_TWEET_SIZE+((MAX_TWEET_SIZE % MEGA_SERIAL_BUFFER_LENGTH)*FRAME_OVERHEAD)   // max amount of data to be sent in one go by either the text_frame and limit for sensor_data_frame
                                                                                                      // need different approach for bitmaps...




struct Frame {            //frame_structure

  byte frame_buffer[FRAME_DATA_LENGTH]; // use to pack single frame // use to unpack single frame, NB not enough space for dedicated extended buffer for each structure 
  byte frame_length = 0;
  byte frame_type = 1;
  byte checksum = 0;
  byte header_checksum = 0;   //can calculate in advance for some pos and menu frames to save time
  int checksum_address = 0;   // can also allocate in advance for pos and menu frames
  bool frame_arrived = false;

};

// \/ is this needed
struct Text_Frame {            //seperate structure without array to save bytes

  byte frame_length = 0;
  byte frame_type = 1;
  byte checksum = 0;

};



class Coms{

private:

  inline int calc_pos(byte MSB, byte LSB);
  inline int calc_local_pos(byte MSB, byte LSB);

public:
Coms(){}

//    int init_software_serial_to_usb_port();            // init the serial at 115200 baud rate
//    int init_software_serial_to_usb_port(int speed);   // init the serial at a custom speed
     
    void extract_pos_frame(byte obj_num);      
    void extract_menu_frame();
    int extract_sensor_data(byte *address); 

     int set_hue_colour(byte receivedData, int dataType){}
};

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
//
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

#endif // COMS_H
