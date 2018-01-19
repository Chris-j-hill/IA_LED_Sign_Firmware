
#ifndef Encoder_H
#define Encoder_H

#include "Due_Pins.h"
#include "Config_local.h"



struct Encoder_Struct {   //structure for the encoder

  int pinA = PIN_A;               // Connected to CLK on KY-040
  int pinB = PIN_B;               // Connected to DT on KY-040
  volatile int PosCount = 0;      //number of pulses recieved
  volatile int position = 0;      //position = no. of pulses/2
  const int center = 0;           // center position, use as reference origin
  int pinALast;
  volatile int aVal; 
  volatile bool encoder_moved = false;    // has the encoder moved, if so do functions
  bool clean_loop = false;                // interrupt will arrive mid loop, wait until end of loop check if encoder moved, 
                                          // set clean loop true, do functions on next loop then set both false when finished
};


//data related to the button on the encoder
struct Button_Struct {
  
  int button_pin = BUTTON_PIN;              // pin num
  volatile bool button_pressed = false;     // has the button been pressed recently
  bool clean_loop = false;
  volatile int last_button_pressed = 0;     // when was the button last pressed
  int button_press_interval = 300;          // minimum period for button to be de-pressed to register 2 independant pressed
};




void update_encoder_ISR ();

//interrrupt service routine for button press
void update_button_ISR();

int init_encoder();
int init_button();
int get_text_encoder_position(int byte_number);


class Encoder {

  private:


  public:
    Encoder(){}
    
    int recenter_encoder();             // reset position to 0

    int handle_interrupts();            // handle interrupt interpretation in code. Use structures  interrupt bool value (eg encoder_moved) to identify if an interrupt occured in the last loop.
    // If corresponding structure variable "clean loop" is false, set true for one loop. this is an indicator to other functions
    // to execute their code. the proceedure should go, interrupt happens, interrupt bool set true, loop ends, handle interrupts checks
    // if interrupt occured, if yes set interrupt bool to false and clean loop to true, functions in loop use clean loop run their code,
    // loop ends, handle interrupts sets clean loop false

   void encoder_position_limits(){}        // software limits on encoder range
   void set_encoder_position(byte val){}    // take value input and set the encoder current position to this
};

#endif // Configuration_H
