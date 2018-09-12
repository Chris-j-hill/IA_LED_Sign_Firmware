#ifndef MEGA_H
#define MEGA_H

#include "Mega_Pins.h"                     //list of pins used in each board
#include "Local_config.h"
//#include "function_declarations.h"    //any functions that need prototypes
#include "Mega_Libraries.h"


void mega_setup();
void mega_loop(); 
void configure_address();
 #endif	//MEGA_H
