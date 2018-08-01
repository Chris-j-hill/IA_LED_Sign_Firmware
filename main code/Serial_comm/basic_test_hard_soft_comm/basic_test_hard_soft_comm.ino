#include <DueTimer.h>
#include "soft_uart.h"

void firstHandler(){
  Serial.println("[-  ] First Handler!");
}

using namespace arduino_due;

#define RX_PIN_1 36 // software serial port's reception pin
#define TX_PIN_1 40 // software serial port's transmision pin
#define RX_PIN_2 37 // software serial port's reception pin
#define TX_PIN_2 41 // software serial port's transmision pin
#define RX_PIN_3 38 // software serial port's reception pin
#define TX_PIN_3 42 // software serial port's transmision pin
#define RX_PIN_4 39 // software serial port's reception pin
#define TX_PIN_4 43 // software serial port's transmision pin
#define SOFT_UART_BIT_RATE 115200 // 57600 38400 1200 19200 9600 115200 300
#define RX_BUF_LENGTH 256 // software serial port's reception buffer length
#define TX_BUF_LENGTH 256 // software serial port's transmision buffer length

uint32_t counter=0;

serial_tc5_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);
auto& serial_1=serial_tc5;
serial_tc6_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);
auto& serial_2=serial_tc6;
serial_tc7_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);
auto& serial_3=serial_tc7;
serial_tc8_declaration(RX_BUF_LENGTH,TX_BUF_LENGTH);
auto& serial_4=serial_tc8;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  serial_1.begin(
    RX_PIN_1,
    TX_PIN_1,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
  serial_2.begin(
    RX_PIN_2,
    TX_PIN_2,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
  serial_3.begin(
    RX_PIN_3,
    TX_PIN_3,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
  serial_4.begin(
    RX_PIN_4,
    TX_PIN_4,
    SOFT_UART_BIT_RATE,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );


  Timer3.attachInterrupt(firstHandler).start(100000); // Every 500ms
}

void loop() {
  Serial.println(counter);
  serial_1.println(counter);
  counter++;
  delay(500);
}

