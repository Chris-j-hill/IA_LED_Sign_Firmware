#ifndef LOCAL_CONFIG_H
#define LOCAL_CONFIG_H      

#ifndef CONFIGURATION_H   // if main file not compiled define these 


//  __________  Common Variables  ____________
#define DEBUG       //enable debugging by default 

#ifdef DEBUG
#define Sprintln(a) (Serial.println(a))   // use to print debug errors
#define Sprint(a) (Serial.print(a))

#else

#define Sprintln(a)    // prints nothing if debug not defined
#define Sprint(a)
#endif



#define USE_SERIAL_TO_MEGAS
#define USE_I2C_TO_MEGAS


#define single_matrix_width 64
#define single_matrix_height 32
#define NUM_SCREENS 4    //per side
#define TOTAL_WIDTH single_matrix_width * NUM_SCREENS

// Select if the due is enabled or if all megas enabled, if not all megas enabled, choose which
#define DUE_ENABLED                        
#define ALL_MEGAS_ENABLED

#ifndef ALL_MEGAS_ENABLED
#define MEGA_1_ENABLED true
#define MEGA_2_ENABLED true
#define MEGA_3_ENABLED true
#define MEGA_4_ENABLED true
#endif

#define USING_COLOUR_SET_333
//#define USING_COLOUR_SET_444    // NOT FINISHED IMPLEMENTATION


#define DEFAULT_TEXT_SIZE 2
#define DEFAULT_TEXT_RED_BRIGHTNESS 7
#define DEFAULT_TEXT_GREEN_BRIGHTNESS 7
#define DEFAULT_TEXT_BLUE_BRIGHTNESS 
#endif

// __________  Mega Specific Variables  ____________

#endif // LOCAL_CONFIG_H
