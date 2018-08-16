//test_arduinoduedht11.ino

#include <Due_DHT11.h>

dht11 dht11;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Hello");
  dht11.init(2)
  
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  dht11.scan(100);	//timout of 100ms
  delay(1000);
  Serial.print("Humidity is : ");Serial.println(dht11.get_humidity(),DEC);
  Serial.print("Temperature is : ");Serial.println(dht11.get_temperature(),DEC);
}