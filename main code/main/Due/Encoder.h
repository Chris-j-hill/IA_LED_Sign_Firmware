
#ifndef Encoder_H
#define Encoder_H

class encoder {

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
