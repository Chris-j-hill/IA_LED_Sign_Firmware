
#ifndef Functions_H
#define Functions_H


struct Timers{            //structure for all timers used and related variables
  
//Timer1 is led strip timer
// Timer2 is fan speed control timer
//Timer3 is to cycle between modes, eg display different strings peiodically, read instruction set when implented etc

bool led_strip_timer_attached = false;
bool fan_timer_attached = false;
bool pos_timer_attached = false;

};


int send_pos_frame(int address);
int pack_xy_coordinates();
int send_disp_string_frame(int address);
//int pack_disp_string_frame(byte frame_type, uint16_t frame_offset);


#endif
