

#ifndef COMS_SERIAL_H
#define COMS_SERIAL_H

#include "Coms.h"
#include "Local_Config.h"

#define TEXT_FRAME      1
#define POS_FRAME       2
#define SENSOR_FRAME    3
#define MENU_FRAME      4

//locations in frame header of data 
#define FRAME_LENGTH_LOC  0
#define FRAME_TYPE_LOC    1
#define FRAME_NUMBER_LOC  2
#define NUM_OF_FRAMES_LOC 3


void serial_recieved_ISR();

class Coms_Serial: public Coms{

private:

public:
Coms_Serial(){}

void read_buffer();
void received_text_frame();
void received_pos_frame();
void received_menu_frame();
void received_sensor_frame();

void attach_interrupt(){};

};
#endif // COMS_SERIAL_H
