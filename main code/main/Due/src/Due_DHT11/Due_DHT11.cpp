
#include "Due_DHT11.h"

dht11::dht11(uint8_t pin)
{
	//This pin should be pulled high by a resistor.
	_pin=pin;
	_integral_RH = 0;
	_decimal_RH = 0;;//Not used in DHT11
	_integral_T = 0;
	_decimal_T = 0;//Not used in DHT11.
}

void dht11::init(uint8_t pin){
	//This pin should be pulled high by a resistor.
	_pin=pin;
	_integral_RH = 0;
	_decimal_RH = 0;;//Not used in DHT11
	_integral_T = 0;
	_decimal_T = 0;//Not used in DHT11.
}

void dht11::scan(uint16_t time_out)
{
	int start_time = millis();
	//Where the magic happens
	//Step 1
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delay(18);//at least 18ms.

    //Step 2
    digitalWrite(_pin, HIGH);
    delayMicroseconds(30);
    pinMode(_pin, INPUT);
	
    while(digitalRead(_pin)!=0 && millis()<start_time+time_out);
	if (!(millis()>=start_time+time_out)){
		//Step 3
		while(digitalRead(_pin)==0);

		//Step 4
		while(digitalRead(_pin)!=0);
		
		//Apparently, DHT11 is ready for data transmit.
		//Every Bit communication lasts 80usec. There are 40 bits
		int j=0;
		
		_integral_RH = 0;
		_decimal_RH = 0;
		_integral_T = 0;
		_decimal_T = 0;
		
		int time_keeper=0;//For duration to distinguish "0" from "1"
		int temp_bit = 0;
		//Data transmission.
		for(int i=0;i<40;i++)
		{
		  time_keeper=0;//For duration to distinguish "0" from "1"
		  while(digitalRead(_pin)==0);
		  while(digitalRead(_pin)!=0)
		  { 
			//comparison between "0" and "1" here
			//Apparently, duration of "0" is less than 25 increments of time_keeper.
			//            duration of "1" is more than 25 increments of time keeper.
		   if(time_keeper > 25){temp_bit=1;}
		   else {temp_bit=0;}

		   time_keeper++; 
		  }
			  //Serial.println(time_keeper);

			   if(j<8)//8bit integral RH data 
			   {
				//Apply bit masking
				_integral_RH += (0x00)|(temp_bit<<(7-j)); 
			   }
			   else if(j<16)//8bit decimal RH data
			   {
				_decimal_RH += (0x00)|(temp_bit<<(15-j)); 
			   }
			   else if(j<24)//8bit integral T data
			   {
				_integral_T += (0x00)|(temp_bit<<(23-j));
			   }
			   else if(j<32)//8bit decimal T data
			   {
				_decimal_T += (0x00)|(temp_bit<<(31-j));
			   }
			   else if(j<40)//check sum
			   {
				//Not used.
			   }      
		 j++; 
		}
	}
	else{					//indicate sensor not working
		_integral_RH =0;
		_integral_T =0;
	}
}

uint8_t dht11::get_temperature()
{
	return _integral_T;
}
uint8_t dht11::get_humidity()
{
	return _integral_RH;
}
