
/*
 * Parent class used to genreate the frames, parse them, do error checking and any other functionallity
 * Actually transmitting the frames is done in the daughter class of the protocol used
 * 
 */

#ifndef Sign_coms_H
#define Sign_coms_H

#include "Config_Local.h"
//#include "Coms_I2C.h"
//#include "Coms_Serial.h"
//https://github.com/ivanseidel/DueTimer
#include "DueTimer.h"       // wrapper library to handle timers 0-4 

//https://github.com/antodom/soft_uart
#include "soft_uart.h"

// variables for initialising soft serial for comms
using namespace arduino_due;
#define COMS_SPEED 38400         //speed of coms between due and megas when using serial
#define SOFT_UART_BIT_RATE COMS_SPEED // 57600 38400 1200 19200 9600 115200 300
#define RX_BUF_LENGTH 256 // software serial port's reception buffer length 
#define TX_BUF_LENGTH 256 // software serial port's transmision buffer length

serial_tc5_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);
auto& Serial_1=serial_tc5;
serial_tc6_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);
auto& Serial_2=serial_tc6;
serial_tc7_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);
auto& Serial_3=serial_tc7;
serial_tc8_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);
auto& Serial_4=serial_tc8;

#define HEADER_LENGTH 4  //length,type,num frames, frame no
#define TRAILER_LENGTH 1 //just checksum
#define DATA_IDENTIFIER_BYTE  2   //frame type byte
#define FRAME_LENGTH_BYTE  1
#define FRAME_DATA_LENGTH MEGA_SERIAL_BUFFER_LENGTH-HEADER_LENGTH-TRAILER_LENGTH
#define FRAME_OVERHEAD HEADER_LENGTH+TRAILER_LENGTH        //number of overhead bytes -> frame length, frame type, num frames, frame num, checksum

#define MEGA_SERIAL_BUFFER_LENGTH 32

#define WAIT_TIME_FOR_USB_PORT_CONNECTION 5000


#ifndef ALL_MEGAS_ENABLED
bool mega_enabled[4] = {MEGA_1_ENABLED, MEGA_2_ENABLED, MEGA_3_ENABLED, MEGA_4_ENABLED};  
#else
bool mega_enabled[4] = {true, true, true, true};  // ignore communication if other board is false
#endif




// list of valid sensor prefix's for sending non string data to the megas.
// append this as required and add to switch statements in due and mega code
const byte to_mega_prefix_array[] = {10, 11, 20, 21, 22, 30, 31, 40, 50, 60, 61, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180};  

byte time_since_last_sent_text_frame = 0;
bool send_text_now = false;
volatile bool send_pos_now = false;   //variable set in interrupt to trigger send pos function in main loop. (serial doesnt work in interrutps)


struct Frame{             //frame details for the due, seperate one for the mega below

byte frame_buffer[MEGA_SERIAL_BUFFER_LENGTH];
byte frame_length = 0;
byte frame_type = 1;
byte checksum = 0;
byte num_frames = 0;
byte this_frame = 0;
};
Frame frame;


#ifdef ENABLE_ERROR_CHECKING
struct Nack_details{
  byte mega1_failed_frame_type = 0;   // the type of frame last sent, to recalculate
  byte mega2_failed_frame_type = 0;
  byte mega3_failed_frame_type = 0;
  byte mega4_failed_frame_type = 0;
  
  byte mega1_failed_frame_number = 0;   // the frame number that failed 
  byte mega2_failed_frame_number = 0;
  byte mega3_failed_frame_number = 0;
  byte mega4_failed_frame_number = 0;
};
Nack_details nack;
#endif // ENABLE_ERROR_CHECKING


//pos frame variables
byte x_pos_LSB = 0;   
byte x_pos_MSB = 0;
byte y_pos_LSB = 0;
byte y_pos_MSB = 0;
byte x_pos_dir = 129;   //direction and speed of the cursor, static = 128
byte y_pos_dir = 127;
byte comms_delay = 0;
byte pos_update_freq = 5;
byte pos_frame_length = 13;   //length of frame to transmit to update pos



int attach_timer_pos_update();

int set_pos_update_frequency(int freq);
 
int set_pos_speed(int x_speed, int y_speed){            //function to set the speed (pixels per second) the cursor postion is moving at
x_pos_dir = x_speed+128;                                //shift up to allow negatives to be sent as bytes, make sure to shift down on recieve end
y_pos_dir = y_speed+128;
}

void send_pos_interrupt(){     // interrupt to send pos data to all megas
    send_pos_now = true;
}


class Coms {

  private:
    

  public:

    Coms(){}

    int init_software_serial_to_megas();            // init the software serial at 115200 baud rate
    int init_software_serial_to_megas(int speed);   // init the serial at a custom speed
    int init_software_serial_to_usb_port();            // init the serial at 115200 baud rate
    int init_software_serial_to_usb_port(int speed);   // init the serial at a custom speed
    int startup_handshake();      //startup sequence to ensure due boots first and transmission begins when all megas are ready
    virtual void write_frame(int address) = 0;
//     void write_frame(int address)
    int send_disp_string_frame(int address);                             //complete function to send strings over i2c to display on specified screen
    int pack_disp_string_frame(int frame_type, int frame_offset);        //function to pack a frame of text to display
    int send_pos_frame(int address);                                     //function to send the xy coordinates along with a number of other related variables
    int pack_xy_coordinates() ;                                          //function to pack the 4 bytes to send the x and y positions of the text cursor
    int send_all_calibration_data(int address);                          //function to send all data calibration
    int send_specific_calibration_data(byte sensor_prefix, int address, bool more_bytes, int offset);  //function to send specific value
    int send_all_pos_on_interrupt();     // function to send pos data to all megas periodically based on timer interrupt
    int send_all_pos_now();
    int send_all_text();    // send the text frame to all megas
    void print_frame();      //print the currently stored frame to the serial monitor
   
    int get_text_colour_hue(int byte_number);                            //function to retrun the MSB or LSB of the current hue value to send over i2c


//  todo
    void echo_menu();
    int generate_checksum();
    int error_check();
    int get_serial();                     //function to interpret serial data recieved without a user prompt

    int get_frame_code();
    int get_data();             //extract data from frame


};




#endif  //Sign_coms_H

