
// sandboxed version of the due code, should not interfere with and mega code, 
// should run independantly of main sign config
// this file should only call main due functions in due.h file. 
// all other functionality should be handled by those files 
 
#ifdef _SAM3XA_

//include paths for libraries and .h files
#include "Due.h"    



//all code for both board types executed in setup
void setup() {


  due_setup();
  due_loop();
}

void loop() 
{}   //no need to execute code here



#else
#error"This is the due code only, make sure the board selected is a due, or else run main.ino"
#endif
