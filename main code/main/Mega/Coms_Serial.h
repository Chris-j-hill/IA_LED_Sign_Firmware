

#ifndef COMS_SERIAL_H
#define COMS_SERIAL_H

#include "Coms.h"
#include "Local_Config.h"

//#define TEXT_FRAME      1
//#define POS_FRAME       2
//#define SENSOR_FRAME    3
//#define MENU_FRAME      4


#define APPROX_BYTE_TRANSMIT_TIME 8/((COMS_SPEED<<12/1000000)>>12)//approximate number of microseconds expected per byte, shift by 12 bits retains precision
#define WAIT_FOR_BYTE_PERIOD   APPROX_BYTE_TRANSMIT_TIME<<1 //wait twice this long, rounding error above and maybe start and stop bits

#define Serial_1 Serial1

// array of random values used to test channel reliability at given coms rate
const byte channel_reliability_test_data[16 * 25] PROGMEM = {179, 94, 163, 176, 82, 66, 249, 135, 17, 37, 240, 59, 107, 231, 105, 4, 225, 181, 125, 189, 44, 171, 108, 243, 17,
                                                             223, 241, 166, 88, 212, 176, 9, 8, 17, 5, 65, 153, 79, 33, 8, 229, 11, 36, 65, 18, 186, 134, 87, 21, 227,
                                                             13, 4, 221, 242, 210, 33, 83, 211, 34, 246, 136, 28, 177, 213, 74, 131, 175, 223, 237, 216, 136, 76, 138, 71, 59,
                                                             56, 212, 14, 133, 146, 31, 249, 87, 201, 248, 244, 132, 23, 204, 205, 139, 187, 21, 119, 174, 64, 180, 237, 114, 220,
                                                             117, 160, 209, 244, 146, 48, 93, 216, 23, 31, 68, 214, 102, 234, 88, 155, 111, 118, 65, 34, 234, 97, 76, 150, 182,
                                                             245, 137, 135, 18, 73, 37, 79, 62, 60, 119, 64, 235, 75, 35, 21, 194, 97, 7, 110, 219, 194, 145, 86, 224, 223,
                                                             202, 166, 177, 53, 178, 149, 30, 148, 62, 168, 237, 127, 78, 129, 130, 218, 250, 192, 179, 51, 227, 227, 220, 120, 240,
                                                             115, 186, 54, 198, 203, 18, 234, 240, 26, 74, 17, 71, 27, 103, 93, 98, 102, 179, 102, 155, 17, 215, 87, 111, 35,
                                                             85, 24, 139, 234, 113, 210, 34, 12, 219, 193, 76, 167, 152, 44, 189, 21, 112, 54, 46, 139, 46, 230, 35, 191, 100,
                                                             15, 224, 179, 200, 114, 185, 85, 13, 178, 142, 151, 234, 72, 147, 134, 187, 40, 174, 219, 41, 188, 240, 129, 119, 251,
                                                             189, 3, 244, 75, 119, 237, 229, 5, 187, 109, 52, 130, 39, 155, 205, 84, 83, 142, 149, 1, 178, 208, 219, 220, 165,
                                                             129, 75, 113, 38, 71, 126, 127, 174, 166, 68, 162, 249, 0, 54, 209, 214, 80, 217, 95, 197, 198, 0, 98, 119, 229,
                                                             51, 46, 21, 217, 172, 167, 157, 153, 132, 192, 204, 50, 72, 133, 48, 95, 228, 142, 56, 195, 128, 0, 178, 127, 123,
                                                             109, 237, 14, 200, 231, 227, 149, 29, 83, 229, 128, 28, 141, 253, 31, 212, 63, 230, 56, 107, 108, 22, 160, 124, 3,
                                                             43, 17, 161, 69, 232, 137, 178, 203, 169, 186, 166, 76, 222, 125, 210, 45, 79, 107, 133, 14, 156, 66, 115, 58, 159,
                                                             192, 148, 203, 58, 191, 72, 7, 158, 30, 104, 203, 101, 10, 177, 163, 33, 104, 91, 110, 149, 219, 5, 121, 21, 59
                                                            };


void serial_recieved_ISR();

class Coms_Serial: public Coms {

  private:

    inline bool byte_queued();
    byte error_check_encoded_header(byte *temp_buffer);   //function to run decoding on header if its encoded
    byte error_check_unencoded_header(byte *temp_buffer); //pass pointers to temp buffer becasue memory issues

//    void received_text_frame(byte *temp_buffer);
//    void received_pos_frame(byte *temp_buffer);
//    void received_menu_frame(byte *temp_buffer);
//    void received_sensor_frame(byte *temp_buffer);
//    void received_ping_frame(byte *temp_buffer);
      void receive_frame(byte *temp_buffer);

    inline void ping_good() { //respond indicating good connection
      Serial_1.println("ping_g");
    };
    inline void ping_bad() { //respond indicating bad connection
      Serial_1.println("ping_b");
    };

  public:
    Coms_Serial() {}
    void init_serial();
    void read_buffer();

    void request_frame(byte frame_type, int8_t this_frame = 0, int8_t obj_num = -1){} //request frame
    void attach_interrupt() {};

};
#endif // COMS_SERIAL_H
