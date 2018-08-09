

#ifndef COMS_H
#define COMS_H

#include "Local_Config.h"
#include "Arduino.h"

#define COMS_SPEED 19200         //speed of coms between due and megas when using serial




#define HEADER_LENGTH         4  //length,type,frames num, obj num

#ifdef DO_HEAVY_ERROR_CHECKING
#define TRAILER_LENGTH        3  //column partiy check, checksum and end byte
#else
#define TRAILER_LENGTH        2  // only outer checksum and end byte
#endif

#define FRAME_TYPE_BYTE       1  //location of frame type byte
#define FRAME_LENGTH_BYTE     0  //location of...


#define FRAME_OVERHEAD HEADER_LENGTH+TRAILER_LENGTH        //number of overhead bytes -> frame length, frame type, num frames, frame num, checksum
#define FRAME_DATA_LENGTH MEGA_SERIAL_BUFFER_LENGTH-FRAME_OVERHEAD

//locations in frame header of data
#define FRAME_LENGTH_LOC  0
#define FRAME_TYPE_LOC    1
#define FRAME_NUMBER_LOC  2 //encoding these into same byte
#define NUM_OF_FRAMES_LOC 2
#define OBJ_NUM_LOC       3

#define TEXT_FRAME_TYPE     1
#define POS_FRAME_TYPE      2
#define SENSOR_FRAME_TYPE   3
#define MENU_FRAME_TYPE     4
#define PING_STRING_TYPE    5

#define POS_FRAME_LENGTH      FRAME_OVERHEAD + 7
#define MENU_FRAME_LENGTH     FRAME_OVERHEAD + 3
#define PING_FRAME_LENGTH     FRAME_OVERHEAD +4   //not implemented 3/8/18

#define EXPECTED_NUM_POS_FRAMES           1 //value we expect in num_frames part of frame
#define EXPECTED_NUM_MENU_FRAMES          1
#define EXPECTED_NUM_PING_FRAMES          1
#define EXPECTED_MAX_TEXT_FRAMES          ceil(MAX_TWEET_SIZE / FRAME_DATA_LENGTH) //max number of frames that could be sent for a text string 
#define EXPECTED_MAX_SENSOR_DATA_FRAMES   ceil(166 / FRAME_DATA_LENGTH) //as of 3/8/18 there is 83 items that could be sent, this is 166 bytes of data -> 7 frames

//use these for error checking frame
#define NUM_ALLOWED_FRAME_TYPES 8// there is space for 3 bits of data in frame for frame type byte
#define NUM_ALLOWED_FRAMES      8// 3 bits of data


#define APPLY_FRAME_LENGTH_MASK(a) ((a >> 1) & 0b00111111)  //isolate data
#define APPLY_FRAME_TYPE_MASK(a)   ((a >> 1) & 0b00000111)
#define APPLY_THIS_FRAME_MASK(a)   ((a >> 1) & 0b00000111)  
#define APPLY_FRAME_NUM_MASK(a)    ((a >> 5) & 0b00000111)
#define APPLY_OBJ_NUM_MASK(a)      ((a >> 4) & 0b00001111)

#define APPLY_FRAME_LENGTH_PARITY_MASK(a) (a & 0b00000001)  //isolate parity bit in header byte
#define APPLY_FRAME_TYPE_PARITY_MASK(a)   (a & 0b00000001) 
#define APPLY_THIS_FRAME_PARITY_MASK(a)   (a & 0b00000001)
#define APPLY_FRAME_NUM_PARITY_MASK(a)    ((a >> 4)&0b00000001)
#define APPLY_OBJ_NUM_PARITY_MASK(a)      (a & 0b00000001)
#define APPLY_CHECKSUM_THREE_BIT_MASK(a)  (a & 0b00001110)


#define GET_GLOBAL_POS(a, b) (a<<8 & b)
#define GET_TEXT_DIR(a) a-128
#define GET_ENCODER_POS(a, b) GET_GLOBAL_POS(a, b)



//#define WAIT_TIME_FOR_USB_PORT_CONNECTION 5000

//const byte hamming_H_matrix[] PROGMEM = { 0x4259F1BA, //hamming matrix used to generate syndrome for error checking header
//                                          0x212CF8DD,
//                                          0x12CF8DD4,
//                                          0x0967C6EA,
//                                          0x04B3E375};
//
//
//const byte syndrome[] PROGMEM = {31, 1, 11, 2, 8, 12, 27, 3, 25, 9, 15, 13, 22, 28, 17, 4, 7, 26, 30, 10, 21, 16, 24, 14, 20, 23, 6, 29, 255, 18, 19, 5};// LUT for syndrome, five bit error vector equates to index in this
//                                                                                                                                                         // value at index is the bit that is incorrect
//
//

struct Frame {            //frame_structure

  //byte frame_buffer[FRAME_DATA_LENGTH];  // use to unpack single frame, NB not enough space for dedicated extended buffer for each structure need to decode on the fly before next frame arrives
  byte frame_length = 0;
  byte frame_type = 1;
  byte checksum = 0;
  //  byte header_checksum = 0;    //can calculate in advance for some pos and menu frames to save time
  //  byte checksum_address = 0;   // can also allocate in advance for pos and menu frames
  bool frame_arrived = false;

  byte num_frames = 0;
  byte this_frame = 0;
};


class Coms {

  private:

    inline int calc_pos(byte MSB, byte LSB);
    inline int calc_local_pos(byte MSB, byte LSB);

    //error checking functions, return true for error found
    bool check_byte_parity(byte *buf, byte start_byte, byte end_byte);
    bool check_vertical_checksum(byte *buf, byte frame_length);
    bool check_final_checksum(byte *buf, byte frame_length);

    inline uint16_t sum_header(byte a,byte b,byte c,byte d);
  protected:
    byte parity_of(byte data);
    
  public:
    Coms() {}

    //    int init_software_serial_to_usb_port();            // init the serial at 115200 baud rate
    //    int init_software_serial_to_usb_port(int speed);   // init the serial at a custom speed

    void extract_pos_frame(byte obj_num);
    void extract_menu_frame();
    void extract_sensor_data(byte *temp_buffer);

    bool validate_checksum(byte *temp_buffer);
    void frame_cpy(byte *temp_buffer, byte frame_type);
    byte set_hue_colour(byte receivedData, byte obj_num ,byte data_loc);//set the value of hue
    bool error_check_frame_body(byte *buf, byte frame_type, byte frame_length);  //if frame ok return true
    void unpack_pos_frame(byte *data);
    void unpack_menu_frame(byte *data);
    void unpack_text_frame(byte *data);
    void unpack_ping_frame(byte *data);
    void unpack_sensor_data_frame(byte *data);
    void remove_byte_parity_bit();  //strip data of parity bit
    
    
};


#endif // COMS_H
