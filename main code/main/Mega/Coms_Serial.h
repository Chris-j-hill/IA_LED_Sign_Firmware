

#ifndef COMS_SERIAL_H
#define COMS_SERIAL_H

#include "Coms.h"
#include "Local_Config.h"


#define APPROX_BYTE_TRANSMIT_TIME 8/((COMS_SPEED<<12/1000000)>>12)//approximate number of microseconds expected per byte, shift by 12 bits retains precision
#define WAIT_FOR_BYTE_PERIOD   APPROX_BYTE_TRANSMIT_TIME<<1 //wait twice this long, rounding error above and maybe start and stop bits

#define Serial_1 Serial1
#define SERIAL1_RX_PIN 19


class Coms_Serial: public Coms {

  private:

    inline bool byte_queued();
    byte error_check_encoded_header(byte *temp_buffer);   //function to run decoding on header if its encoded
    byte error_check_unencoded_header(byte *temp_buffer); //pass pointers to temp buffer becasue memory issues

    void receive_frame(byte *temp_buffer);



    void ping_good();  //respond indicating good connection

    void ping_bad();  //respond indicating bad connection

    uint32_t detRate(byte recpin);



  public:
    Coms_Serial() {}
    void init_serial();
    void read_buffer();

    void request_frame(byte frame_type, int8_t this_frame = 0, int8_t obj_num = -1) {} //request frame
    void request_frame_retransmission(byte frame_type, byte this_frame, byte obj_num);  //request specific data
    void request_frame_retransmission(); //not sure what data we missed

};
#endif // COMS_SERIAL_H
