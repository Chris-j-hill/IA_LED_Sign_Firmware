
#ifndef Functions_H
#define Functions_H

class sign_coms;
class due_class;
class mega_class;





//main code for each board type
void  due_setup();    
void  due_loop();

void  mega_setup();
void  mega_loop();


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
int wire_write_frame(int address);
int send_pos_frame(int address);
int pack_xy_coordinates();
int send_disp_string_frame(int address);
int pack_disp_string_frame(int frame_type, int frame_offset);

//mega
void receiveEvent(int howMany);   //interrupt for the mega i2c

#endif
