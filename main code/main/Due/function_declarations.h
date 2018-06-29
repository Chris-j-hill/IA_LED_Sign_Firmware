
#ifndef Functions_H
#define Functions_H

//forward declare all classes
//class Coms;
//class Coms_Serial;
//class Coms_i2c;
//class Card;
//class Menu;
//class Internet;
//class Fans;
//class Encoder;
//class Led_strip;
//class SdFat;
//class Current_Meter;
//class Light_Sensor;


struct Timers{            //structure for all timers used and related variables
  
//Timer1 is led strip timer
// Timer2 is fan speed control timer
//Timer3 is to cycle between modes, eg display different strings peiodically, read instruction set when implented etc

bool led_strip_timer_attached = false;
bool fan_timer_attached = false;
bool pos_timer_attached = false;

};


void send_pos_interrupt();
int set_pos_speed(int x_speed, int y_speed);
int set_pos_update_frequency(int freq);
int attach_timer_interrupts();

int send_pos_frame(int address);
int pack_xy_coordinates();
int send_disp_string_frame(int address);
int pack_disp_string_frame(int frame_type, int frame_offset);


#endif
