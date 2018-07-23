
#ifndef Encoder_H
#define Encoder_H

#include "Due_Pins.h"
#include "Config_local.h"



struct Encoder_Struct {   //structure for the encoder

  byte pinA = PIN_A;               // Connected to CLK on KY-040
  byte pinB = PIN_B;               // Connected to DT on KY-040
  volatile int PosCount = 0;      //number of pulses recieved
  volatile int position = 0;      //position = no. of pulses/2
  const int center = 0;           // center position, use as reference origin
  int pinALast;
  volatile int aVal;
  volatile bool encoder_moved_ISR = false;    // has the encoder moved, if so do functions
  bool encoder_moved = false;                // interrupt will arrive mid loop, wait until end of loop check if encoder moved,
  // set clean loop true, do functions on next loop then set both false when finished
  bool enabled = false;
  bool is_attached = false;

  byte sensitivity = 1;   // scaler for how many turns require one position movement to be read
};


//data related to the button on the encoder
struct Button_Struct {

  int button_pin = BUTTON_PIN;              // pin num
  volatile bool button_pressed_ISR = false;     // has the button been pressed recently
  bool button_pressed = false;
  volatile int last_button_pressed = 0;     // when was the button last pressed
  int button_press_interval = 300;          // minimum period for button to be de-pressed to register 2 independant pressed
  bool enabled = false;
  bool is_attached = false;
};


void update_encoder_ISR ();

//interrrupt service routine for button press
void update_button_ISR();

void init_encoder_ISR();
void init_button_ISR();

class Encoder {

  private:

    void enable_encoder();
    void disable_encoder();
    void enable_button();
    void disable_button();

  public:
    Encoder() {}
    void init_encoder();
    void init_button();



    void recenter_encoder();             // reset position to encoder_parameters.center

    void handle_interupts();            // handle interrupt interpretation in code. Use structures  interrupt bool value (eg encoder_moved) to identify if an interrupt occured in the last loop.
    // If corresponding structure variable "clean loop" is false, set true for one loop. this is an indicator to other functions
    // to execute their code. the proceedure should go, interrupt happens, interrupt bool set true, loop ends, handle interrupts checks
    // if interrupt occured, if yes set interrupt bool to false and clean loop to true, functions in loop use clean loop run their code,
    // loop ends, handle interrupts sets clean loop false

    void encoder_position_limits();        // software limits on encoder range
    void set_encoder_position(int val);      // take value input and set the encoder current position to this
    byte get_text_encoder_position(byte byte_number);  //upper or lower byte of position
    inline void below_zero_limit();
    inline void above_upper_limit(int limit);
};

#endif // Configuration_H
