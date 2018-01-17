#ifndef Internet_H
#define Internet_H

#include "Due_Libraries.h"


#ifdef ENABLE_ETHERNET
bool enable_ethernet = true;
#else
bool enable_ethernet = false;
#endif

#ifdef ENABLE_WIFI
bool enable_wifi = true;
#else
bool enable_wifi = false;
#endif

bool ethernet_enabled = false;
bool wifi_enabled = false;

bool ethernet_connected = true;   
bool wifi_connected = true;


class Internet {

  private:

  public:

    Internet(){}
    
    int init_ethernet();
    int init_wifi();
};


#endif //Internet_H
