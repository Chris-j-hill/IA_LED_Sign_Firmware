
#ifndef Functions_H
#define Functions_H
//
//class sign_coms;
//class due_class;
//class mega_class;

class Coms;
class Coms_Serial;
class Coms_i2c;
class Card;
class Menu;
class Internet;
class Fans;
class Encoder;
class Led_strip;
class SdFat;
class Current_Meter;
class Light_Sensor;


struct Timers{            //structer for all timers used and related variables
  
//Timer1 is led strip timer
// Timer2 is fan speed control timer
//Timer3 is to cycle between modes, eg display different strings peiodically, read instruction set when implented etc

bool led_strip_timer_attached = false;
bool fan_timer_attached = false;
bool pos_timer_attached = false;

};





//main code for each board type
void  due_setup();    
void  due_loop();

//interrupts
void update_encoder_ISR();    //interrupt for when encoder position changes
int init_encoder();      //set up functions for the encoder
void reset_encoder();     //reset position to zero

void update_button_ISR();
int init_button();
void reset_button();


void fade_led_strip();
int led_strip_set_freq();
void fade_fans();
int fans_set_freq();      //interrupt to set the frequency the fans are adjusted
void send_pos_interrupt();
int set_pos_speed(int x_speed, int y_speed);
int set_pos_update_frequency(int freq);
int attach_timer_interrupts();

int send_pos_frame(int address);
int pack_xy_coordinates();
int send_disp_string_frame(int address);
int pack_disp_string_frame(int frame_type, int frame_offset);


#endif
