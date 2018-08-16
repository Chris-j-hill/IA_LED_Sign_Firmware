#This is an easy to use library for DHT11 using the Arduino due 

To INSTALL

1. Simply clone this repository in your Arduino/libraries file path.

2. Then Sketch/Include Libraries/Manage Libraries, wait until the Update reaches 100%.
   Close Library Manager.

3. Sketch/Include Libraries   

To USE

1. #include <arduinoduedht11.h>
2. dht11 dht11(2);//DHT11 object. PIN to attach to DHT11, here PIN2.
3. There are 3 functions : dht11.scan(); , dht11.get_humidity(), dht11.get_temperature().

NOTE : dht11.get_humidity() is in percentage and dht11.get_temperature() is in Celcius.

Pin out of DHT11 (might be different depending on model)
![alt tag](http://learning.grobotronics.com/wp-content/uploads/2013/07/DHT11_Pins.jpg)

By Vincent Gosselin, 2017.



