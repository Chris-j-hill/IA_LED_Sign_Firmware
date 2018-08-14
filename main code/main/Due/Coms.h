
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
#define HARD_COMS_CONFIG SERIAL_8N2

#define SOFT_COMS_SPEED 4800      // software serial speed, nb this may get in the way if transmitting large amounts of data very slowly
#define SOFT_COMS_CONFIG_NUM_BITS     EIGHT_BITS
#define SOFT_COMS_CONFIG_NUM_PARITY   NO_PARITY     //custom parity implemented
#define SOFT_COMS_CONFIG_NUM_STOP     TWO_STOP_BITS  //stop bits to help timing



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
#define FRAME_OVERHEAD (HEADER_LENGTH + TRAILER_LENGTH)        //number of overhead bytes -> frame length, frame type, num frames, frame num, checksum
#define FRAME_DATA_LENGTH (MEGA_SERIAL_BUFFER_LENGTH - FRAME_OVERHEAD)


#define WAIT_TIME_FOR_USB_PORT_CONNECTION 5000

#define MAX_TWEET_SIZE 100
//#define MAX_FRAME_SIZE MAX_TWEET_SIZE+((MAX_TWEET_SIZE % MEGA_SERIAL_BUFFER_LENGTH)*FRAME_OVERHEAD)   // max amount of data to be sent in one go by either the text_frame and limit for sensor_data_frame
// need different approach for bitmaps...

#define TEXT_FRAME_TYPE     1
#define POS_FRAME_TYPE      2
#define SENSOR_FRAME_TYPE   3
#define MENU_FRAME_TYPE     4
#define PING_STRING_TYPE    5
#define FRAME_RETRANSMIT    6 
#define UNKNOWN_RETRANSMIT_TYPE 7 //in case of mega requesting frame and not knowing what frame it was

const char ping_string[] = "ping";
const char expected_ping_rx = 'p';

#define POS_FRAME_LENGTH (FRAME_OVERHEAD + 7)
#define MENU_FRAME_LENGTH (FRAME_OVERHEAD + 3)
#define PING_FRAME_LENGTH (FRAME_OVERHEAD + sizeof(ping_string))

#define PACK_FRAME_NUM_DATA(a, b) (((a<<5) | (b<<1)) & 0b11101110) //ensure parity bits are zero in case not used
#define PACK_OBJ_NUM_DATA(a) (a<<4)


#define GET_FRAME_NUM_DATA(a)     (a & 0b11100000)
#define GET_THIS_FRAME_DATA(a)    (a & 0b00000111)


#define EXTRACT_FRAME_LENGTH(a)   ((a>>1) & 0b00111111)
#define EXTRACT_THIS_FRAME_DATA(a)  ((a>>1)&0b00000111)
#define EXTRACT_OBJ_NUM_DATA(a)  ((a>>4)&0b00001111)

#define generate_checksum_11(a)   generate_checksum(a, 0x1FFF)

#define CHECKSUM_3_BIT_LOC 3
#define CHECKSUM_3_BIT_MASK 0b00001110
#define CLEAR_HEADER_CHECKSUM(a) (a & ~CHECKSUM_3_BIT_MASK)

#define BYTE_PARITY_LOC 7

#define MEGA_RX_FRAME_LENGTH 4
/*
   Nack frame format:
   
   Frame type
   Frame num
   Obj num
   Checksum   <- if were recieving garbage this could be necessary
*/


#define FRAME_HISTORY_MEMORY_DEPTH 8 // how many frames of each type to each mega can we remember, useful to have large depth for menu system

#define EXPECTED_MAX_TEXT_FRAMES          ceil(MAX_TWEET_SIZE / FRAME_DATA_LENGTH) //max number of frames that could be sent for a text string 


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

//
//struct Frame_History{
//
//   uint32_t transmission_time[FRAME_HISTORY_MEMORY_DEPTH] = {0}; // log time it was transmitted, if a long time ago, try match to another frame maybe? or re transmitt a bunch? or abandon
//   uint8_t frame_header[FRAME_HISTORY_MEMORY_DEPTH][4] = {{0}};    // type, frame, num obj num etc
//   byte jump_table[FRAME_HISTORY_MEMORY_DEPTH] = {0};            // value indicating which function to call to configure retransmit
//   byte num_retransmissions[FRAME_HISTORY_MEMORY_DEPTH] ={0};    // number fo retransmissions of any specific frame
//   byte history_index = 0;                                       // index were currently at in arrays
//
//   byte frame_content[FRAME_HISTORY_MEMORY_DEPTH][MEGA_SERIAL_BUFFER_LENGTH]= {{0}}; //the content of the frame rather than recalculating
//
//   int transmission_time_limit = 0; // limits for allowable if it doesnt respond within a certain time
//   byte num_retransmission_limit = 4;
//   byte num_frames_to_retransmit = 10;  //if cant identify nack, send a few most recent frames. should be only relavent frames, not pos, not previous menu frames etc
//};


struct Frame_History {

  byte history_index = 0;                                                            // index were currently at in arrays
  byte frame_content[FRAME_HISTORY_MEMORY_DEPTH][MEGA_SERIAL_BUFFER_LENGTH] = {{0}}; //the content of the frame rather than recalculating
  uint32_t num_populated_buffers =0;  //counter for number fo frames sent, dont try re transmit more frames than have ever sent
};

class Coms {

  private:

    inline void pack_xy_coordinates(byte obj_num) ;                                          //function to pack the 4 bytes to send the x and y positions of the text cursor

    void set_buffer_parity_bits(byte *buf, byte bit_loc, int buf_length, int start_from = 0); // set parity of last bit for all bytes excpet last two(ie the checksums, which is dependant on the value of the bytes)
    void set_verical_parity_byte(byte *buf, int checksum_loc, int start_byte = 0);

    inline byte parity_of(byte value);
    void set_checksum_11(uint16_t checksum, byte frame_type);

  protected:

    void pack_disp_string_frame(uint16_t frame_num, byte obj_num);        //function to pack a frame of text to display
    void build_pos_frame(byte obj_num);                                               //function to send the xy coordinates along with a number of other related variables
    byte get_text_colour_hue(byte byte_number, byte obj_num);                            //function to return the MSB or LSB of the current hue value
    void build_menu_data_frame(byte menu_number);    //function to build the frame to send menu info
    bool error_sanity_check(byte frame_num, byte obj_num);  //if obj in use and string could occupy this frame number
    void set_frame_parity_and_checksum(byte frame_type, byte frame_length);    //pack frame with parity bits
    uint16_t generate_checksum(byte frame_type, uint16_t modulo_mask = 0xFF);// generate checksum, default is 8 bit checksum
    void hamming_encoder(byte frame_type) {}


    Frame_History text_frame_history[NUM_SCREENS];
    Frame_History menu_frame_history[NUM_SCREENS];
    Frame_History sensor_data_frame_history[NUM_SCREENS];
    Frame_History pos_frame_history[NUM_SCREENS];


    void append_frame_history(byte *buf, byte address, byte frame_type); //add this to frame history

    byte find_in_frame_history(byte address, byte frame_type, byte frame_num, byte obj_num);    //got a nack, find last instance of the frame being sent to that obj, confirm reasonable request

    bool request_error_sanity_check(byte frame_type, byte frame_num, byte obj_num);


    
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

