#ifndef Internet_H
#define Internet_H

//#include "Due_Libraries.h"
#include "Config_Local.h"

extern bool ethernet_connected;   
extern bool wifi_connected;


class Internet {

  private:

  public:

    Internet(){}
    
    int init_ethernet();
    int init_wifi();
};


#endif //Internet_H
