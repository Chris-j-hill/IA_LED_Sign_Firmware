

//include paths for libraries and .h files 
#ifdef _SAM3XA_
#include "Due/Due.h"    
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#include "Mega/Mega.h"    
#else
#error "Neither mega or due selected, compilation failed"
#endif

//all code for both board types executed in setup
void setup() {

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)    //code for megas

  mega_setup();
  mega_loop();     // main loop (-> while (1) )

#else //code for due

  due_setup();
  due_loop();

#endif
}

void loop() {   //no need to execute code here
}

