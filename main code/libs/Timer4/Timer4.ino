
//tibrary from here: https://forum.arduino.cc/index.php?topic=220775.0

#include "TimerFour.h"


uint32_t period = 1000*1000;
byte pin =13;
void setup() {

Timer4.initialize();
Timer4.attachInterrupt(isr4);
Timer4.start();
pinMode(pin, OUTPUT);
}

void loop() {

}

void isr4 (){
  bool state = digitalRead(pin);
  digitalWrite(pin, !state);
}

