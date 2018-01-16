
#ifndef Encoder_H
#define Encoder_H


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
Encoder_Struct encoder;     //create encoder struct

//data related to the button on the encoder
struct Button_Struct {
  
  int button_pin = BUTTON_PIN;              // pin num
  volatile bool button_pressed = false;     // has the button been pressed recently
  bool clean_loop = false;
  volatile int last_button_pressed = 0;     // when was the button last pressed
  int button_press_interval = 300;          // minimum period for button to be de-pressed to register 2 independant pressed
};
Button_Struct button;       //create button struct

int init_encoder() {
  if (enable_encoder && !encoder_enabled) {

    pinMode(encoder.pinA, INPUT);
    pinMode(encoder.pinB, INPUT);

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    if (encoder.pinA != 2 && encoder.pinA != 3 && encoder.pinA != 18 && encoder.pinA != 19 && encoder.pinA != 20 && encoder.pinA != 21) {
      Sprintln("error, encoder pin not an interrupt");
      return (-1);
    }
    else
      attachInterrupt(digitalPinToInterrupt(encoder.pinA), update_encoder_ISR, CHANGE);
#else
    attachInterrupt(encoder.pinA, update_encoder_ISR, CHANGE);
#endif

    return (0);
  }

  else {
    Sprintln(F("Conflict with enabling encoder: make sure only enabled once and 'enable_encoder' is true"));
    return (-1);
  }

}

int init_button() {

  pinMode(button.button_pin, INPUT);


#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  if (button.button_pin != 2 && button.button_pin != 3 && button.button_pin != 18 && button.button_pin != 19  && button.button_pin != 20 && button.button_pin != 21) {
   
      Sprintln("error, button pin not an interrupt");
      return(-1);
   
    
    }
  else
    attachInterrupt(digitalPinToInterrupt(button), update_button_ISR, FALLING);
#else
  attachInterrupt(button.button_pin, update_button_ISR, CHANGE);
#endif


Serial.println(F("Button Initialised"));
return(0);
}

void update_encoder_ISR () {
  encoder.aVal = digitalRead(encoder.pinA);
  if (encoder.aVal != encoder.pinALast) { // Means the knob is rotating
    // if the knob is rotating, we need to determine direction
    // We do that by reading pin B.
    if (digitalRead(encoder.pinB) != encoder.aVal) {  // Means pin A Changed first - We're Rotating Clockwise
      encoder.PosCount ++;

    } else {// Otherwise B changed first and we're moving CCW

      encoder.PosCount--;

    }
    encoder.position = encoder.PosCount / 2;

  }

  encoder.pinALast = encoder.aVal;
  encoder.encoder_moved = true;
}

//interrrupt service routine for button press
void update_button_ISR() {

  //button pressed and been a while since last press
  if (digitalRead(button.button_pin) == false && millis() - button.last_button_pressed >= button.button_press_interval) {
    //if (debug)
    Serial.println("Button Pressed");
    button.last_button_pressed = millis();
    button.button_pressed = true;
  }
}


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

   
};

#endif // Configuration_H
