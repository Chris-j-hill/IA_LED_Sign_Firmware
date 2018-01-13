
//includes for all 3rd party libraries used

#ifndef Mega_Libraries_H
#define Mega_Libraries_H

#include <Wire.h>
#ifdef USE_CUSTOM_RGB_MATRIX_LIBRARY
#include "libs/RGBmatrixPanel.h"
#else
#include "RGBmatrixPanel.h"
#endif


#endif	// Mega_Libraries_H
