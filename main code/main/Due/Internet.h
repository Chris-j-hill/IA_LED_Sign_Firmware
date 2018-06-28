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
    void ethernet_enable(){}
    void ethernet_disable(){} 
    void wifi_enable(){}
    void wifi_disable(){}
    void ping();
    void get_string();
    void get_ip();
    void run_instructions();    // <- run instructions in file on host device
    void connect_to_network();
     
};


#endif //Internet_H
