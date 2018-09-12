  
// sandboxed code for the mega 
    
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#include "Mega.h" 
#else
#error "Select Mega Board, or run main.ino"   
#endif

//all code for both board types executed in setup
void setup() {

  mega_setup();
  mega_loop();     // main loop (-> while (1) )

}

void loop() {   //no need to execute code here
}

