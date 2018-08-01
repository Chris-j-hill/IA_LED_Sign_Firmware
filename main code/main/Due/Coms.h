
/*
   Parent class used to genreate the frames, parse them, do error checking and any other functionallity
   Actually transmitting the frames is done in the daughter class of the protocol used

*/

#ifndef Coms_H
#define Coms_H

#include "Config_Local.h"

//https://github.com/ivanseidel/DueTimer
#include "DueTimer.h"       // wrapper library to handle timers 0-4 

//https://github.com/antodom/soft_uart
#include "soft_uart.h"

// variables for initialising soft serial for comms
using namespace arduino_due;
#define COMS_SPEED 9600         //speed of coms between due and megas when using serial (available baud rates  1200 9600 19200 38400 57600 115200)
#define RX_BUF_LENGTH 256 // software serial port's reception buffer length 
#define TX_BUF_LENGTH 256 // software serial port's transmision buffer length


#define HEADER_LENGTH 4  //length,type,num frames, frame no
#define TRAILER_LENGTH 2 //checksum and end byte
#define FRAME_TYPE_BYTE  1
#define FRAME_LENGTH_BYTE  0

#define ENDBYTE_CHARACTER 255 // can scan through serial until this is reached on mega end if error detected 

#define FRAME_DATA_LENGTH MEGA_SERIAL_BUFFER_LENGTH - HEADER_LENGTH - TRAILER_LENGTH
#define FRAME_OVERHEAD HEADER_LENGTH+TRAILER_LENGTH        //number of overhead bytes -> frame length, frame type, num frames, frame num, checksum

#define WAIT_TIME_FOR_USB_PORT_CONNECTION 5000

#define MEGA_SERIAL_BUFFER_LENGTH 32
#define MAX_TWEET_SIZE 100
//#define MAX_FRAME_SIZE MAX_TWEET_SIZE+((MAX_TWEET_SIZE % MEGA_SERIAL_BUFFER_LENGTH)*FRAME_OVERHEAD)   // max amount of data to be sent in one go by either the text_frame and limit for sensor_data_frame
// need different approach for bitmaps...

#define TEXT_FRAME_TYPE     1
#define POS_FRAME_TYPE      2
#define SENSOR_FRAME_TYPE   3
#define MENU_FRAME_TYPE     4

#define MEGA_RX_FRAME_LENGTH 3
/*
   Nack frame format

   Frame num
   Obj num
   Checksum   <- if were recieving garbage this could be necessary
*/

struct Frame {            //frame details for the due, seperate one for the mega below

  byte frame_buffer[MEGA_SERIAL_BUFFER_LENGTH]; // use to pack single frame
  //byte extended_frame_buffer[MAX_FRAME_SIZE];   // large buffer to store sequence of frames to transmit later, eg text_frame generated and in queue
  bool send_extended_buffer = false;
  byte frame_length = 0;
  byte frame_type = 1;
  byte checksum = 0;
  byte num_frames = 0;
  byte this_frame = 0;
  byte header_checksum = 0;   //can calculate in advance for some pos and menu frames to save time
  uint16_t checksum_address = 0;   // can also allocate in advance for pos and menu frames
  bool frame_queued = false;    //queue frame this loop to send at the beginning of next

};


class Coms {

  private:
    
  public:

    Coms() {}
    
    
    void pack_disp_string_frame(uint16_t frame_num, byte obj_num);        //function to pack a frame of text to display
    void build_pos_frame(byte obj_num);                                               //function to send the xy coordinates along with a number of other related variables
    void pack_xy_coordinates(byte obj_num) ;                                          //function to pack the 4 bytes to send the x and y positions of the text cursor
    
    byte get_text_colour_hue(byte byte_number, byte obj_num);                            //function to return the MSB or LSB of the current hue value to send over i2c

    void calc_delay();

    void build_menu_data_frame(byte menu_number);    //function to build the frame to send menu info
    int init_frames();  //set constant elements of frames
    byte generate_checksum(byte frame_type);
   
    bool error_sanity_check(byte frame_num, byte obj_num);  //if obj in use and string could occupy this frame number
    
    
    //  todo
    void echo_menu(); //is this needed

   
};




#endif  //Coms_H

