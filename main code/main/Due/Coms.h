
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
#define COMS_SPEED 19200         //speed of coms between due and megas when using hardware serial (available baud rates  1200 9600 19200 38400 57600 115200)
#define HARD_COMS_CONFIG SERIAL_8N1

#define SOFT_COMS_SPEED 4800      // software serial speed, nb this may get in the way if transmitting large amounts of data very slowly
#define RX_BUF_LENGTH 256 // software serial port's reception buffer length 
#define TX_BUF_LENGTH 256 // software serial port's transmision buffer length


#define HEADER_LENGTH 4  //length,type,num frames, obj num

#ifdef DO_HEAVY_ERROR_CHECKING
#define TRAILER_LENGTH 3 //2 checksums and end byte
#else
#define TRAILER_LENGTH 2  // one checksum and endbyte
#endif
#define FRAME_TYPE_BYTE  1  //location of ...
#define FRAME_LENGTH_BYTE  0  //location of ...

#define ENDBYTE_CHARACTER 255 // can scan through serial until this is reached on mega end if error detected 

#define MEGA_SERIAL_BUFFER_LENGTH 64 // must match or be less than SERIAL_RX_BUFFER_SIZE in hardware->arduino->avr->cores->arduino->HardwareSerial.h
#define FRAME_OVERHEAD HEADER_LENGTH+TRAILER_LENGTH        //number of overhead bytes -> frame length, frame type, num frames, frame num, checksum
#define FRAME_DATA_LENGTH MEGA_SERIAL_BUFFER_LENGTH - FRAME_OVERHEAD


#define WAIT_TIME_FOR_USB_PORT_CONNECTION 5000

#define MAX_TWEET_SIZE 100
//#define MAX_FRAME_SIZE MAX_TWEET_SIZE+((MAX_TWEET_SIZE % MEGA_SERIAL_BUFFER_LENGTH)*FRAME_OVERHEAD)   // max amount of data to be sent in one go by either the text_frame and limit for sensor_data_frame
// need different approach for bitmaps...

#define TEXT_FRAME_TYPE     1
#define POS_FRAME_TYPE      2
#define SENSOR_FRAME_TYPE   3
#define MENU_FRAME_TYPE     4
#define PING_STRING_TYPE    5

const char ping_string[] = "ping";
const char expected_ping_rx = 'p';

#define POS_FRAME_LENGTH FRAME_OVERHEAD + 7
#define MENU_FRAME_LENGTH FRAME_OVERHEAD + 3
#define PING_FRAME_LENGTH FRAME_OVERHEAD + sizeof(ping_string)

#define PACK_FRAME_NUM_DATA(a, b) (((a<<5) | (b<<1)) & 0b11101110) //ensure parity bits are zero in case not used
#define PACK_OBJ_NUM_DATA(a) (a<<4)


#define GET_FRAME_NUM_DATA(a)     (a & 0b11100000)
#define GET_THIS_FRAME_DATA(a)    (a & 0b00000111)

#define generate_checksum_11(a)   generate_checksum(a, 0x1FFF)

#define CHECKSUM_3_BIT_LOC 3
#define CHECKSUM_3_BIT_MASK 0b00001110



#define MEGA_RX_FRAME_LENGTH 4
/*
   Nack frame format
   Frame type
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

    inline void pack_xy_coordinates(byte obj_num) ;                                          //function to pack the 4 bytes to send the x and y positions of the text cursor

    void set_buffer_parity_bits(byte *buf, byte bit_loc, int buf_length, int start_from = 0); // set parity of last bit for all bytes excpet last two(ie the checksums, which is dependant on the value of the bytes)
    void set_verical_parity_byte(byte *buf, int checksum_loc, int start_byte = 0);
    
    inline byte parity_of(byte value);
    void set_checksum_11(uint16_t checksum,byte frame_type);

  protected:

    void pack_disp_string_frame(uint16_t frame_num, byte obj_num);        //function to pack a frame of text to display
    void build_pos_frame(byte obj_num);                                               //function to send the xy coordinates along with a number of other related variables
    byte get_text_colour_hue(byte byte_number, byte obj_num);                            //function to return the MSB or LSB of the current hue value
    void build_menu_data_frame(byte menu_number);    //function to build the frame to send menu info
    bool error_sanity_check(byte frame_num, byte obj_num);  //if obj in use and string could occupy this frame number
    void set_frame_parity_and_checksum(byte frame_type, byte frame_length);    //pack frame with parity bits
    uint16_t generate_checksum(byte frame_type, uint16_t modulo_mask = 0xFF);// generate checksum, default is 8 bit checksum
    void hamming_encoder(byte frame_type){}

  public:

    //should be very little needs to be public in this class, mostly called through coms_serial

    Coms() {}
     
    void calc_delay();
    void init_frames();  //set constant elements of frames
    void set_header_parity(byte frame_type);

    //  todo
    void echo_menu(); //is this needed


};


#endif  //Coms_H

