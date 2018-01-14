
#ifndef Fans_H
#define Fans_H

//class for both fan cantrol and temperature monitoring  

class fans{

  private:

    int poll_temperature_sensor (int pin);       // read temp sensor registers, used to update stored value to most recent
    byte read_temp_data_from_register (int pin);        // convert incoming bits from temp sensor to a byte

  public:
        fans(){}

          //initialisations of individual items, set pins, test if working if applicable, etc
        int init_fans();
        int init_temp_sensors();            
        int writeFanSpeed(int newValue);
    
        int get_temperature (int pin);       // read temperature based on specified pin, return value in degrees C


//TODO

    int set_fan(int newValue);
    int is_fan_on();                  //useful for menu display
    int return_fan_speed();
    int get_humidity();       //because we can
    int set_temp_sensor_enable(int sensor);   //code to enable sensors, might be useful to toggle them, maybe autotoggle if not working?
    int set_temp_sensor_disable(int sensor);


};


#endif // Fans_H
