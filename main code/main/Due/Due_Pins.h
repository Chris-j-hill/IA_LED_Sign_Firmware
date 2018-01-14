

#ifndef Due_Pins_H
#define Due_Pins_H      //define to stop double declarations
#include "Arduino.h"

    //      due pins


        //also uses SDA1 and SCL1 and spi pins
        #define PIN_A       5
        #define PIN_B       6
        #define BUTTON_PIN  7

        #define LED_STRIP   8
        #define FAN_PIN     9

        #define CURRENT_METER_1 22    // change to analog pin...
        #define CURRENT_METER_2 23

        #define LDR1        24
        #define LDR2        25
        

        #define TEMP1       26
        #define TEMP2       27
        #define TEMP3       28


        #define SD1_ENABLE  4
        #define SD2_ENABLE  44
  
        #define NACK_PIN1   16      //pins for the due
        #define NACK_PIN2   17
        #define NACK_PIN3   18
        #define NACK_PIN4   19
        
        #define NOTIFIER    13      //led pin, use to notify of data stuff happening

        #define due_ready_pin        45   //startup handshake pins
        #define mega1_ready_pin      46
        #define mega2_ready_pin      47
        #define mega3_ready_pin      48
        #define mega4_ready_pin      49

        #define RX_PIN_1 36 // software serial port's reception pin
        #define TX_PIN_1 40 // software serial port's transmision pin
        #define RX_PIN_2 37 // software serial port's reception pin
        #define TX_PIN_2 41 // software serial port's transmision pin
        #define RX_PIN_3 38 // software serial port's reception pin
        #define TX_PIN_3 42 // software serial port's transmision pin
        #define RX_PIN_4 39 // software serial port's reception pin
        #define TX_PIN_4 43 // software serial port's transmision pin


#endif // Due_Pins_H
