
// sandboxed code for the mega 
    
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#include "Mega.h"    
#endif

//    uint8_t         *matrixbuff[2];
//  int buffsize  = 32 * 64 * 3; // x3 = 3 bytes holds 4 planes "packed"
//int allocsize = buffsize * 2;
//uint8_t buf[32 * 64 * 3*2];
//  if(NULL == (matrixbuff[0] = (uint8_t *)malloc(allocsize))) return;
//  memset(matrixbuff[0], 0, allocsize);

//all code for both board types executed in setup
void setup() {

  mega_setup();
  mega_loop();     // main loop (-> while (1) )

}

void loop() {   //no need to execute code here
}

