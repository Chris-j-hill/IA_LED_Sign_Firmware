//arduinodue_dht11.h

#define arduinoduedht11_h

#include "Arduino.h"

#define TIMEOUT 100

class dht11
{
	public:
		dht11(){};
		dht11(uint8_t pin);
		void init(uint8_t pin);
		void scan(uint16_t time_out = TIMEOUT);
		uint8_t get_temperature();
		uint8_t get_humidity();

	private:
		uint8_t _pin;
		uint8_t _integral_RH;
		uint8_t _decimal_RH;//Not used in DHT11
		uint8_t _integral_T;
		uint8_t _decimal_T;//Not used in DHT11
};