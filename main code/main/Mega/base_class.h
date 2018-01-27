//
//class sign_coms {
//
//  private:
//#if defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
//    int hardware_address_pin[2] = {HAREWARE_ADDRESS_1, HAREWARE_ADDRESS_2};     //should be 44,45
//#endif
//    int single_matrix_width = 64;
//    int single_matrix_height = 32;
//    int num_screens = 4;
//    int total_width = single_matrix_width * num_screens;
//
//    int HEADER_LENGTH = 3;
//    int TRAILER_LENGTH = 2;
//    int DATA_IDENTIFIER_BYTE = 2;
//    int FRAME_LENGTH_BYTE = 1;
//
//    //byte frame_type;             // frame type defines what is to be done with info, ie where string should go or other operation
//    //byte frame_length = 150;     // length of current frame
//    int max_frame_size = 150;    // max frame size 150, ie max length of a tweet plus some frame info
//    // byte frame[150];             //frame array
//
//
//
//  public:
//
//    sign_coms();
//
//
//    // emergency stop code
//    void stop();              // stop the code with while(1) loop, maybe implement low power mode???
//
//    int init_serial();            // init the serial at 115200 baud rate
//    int init_serial(int speed);   // init the serial at a custom speed
//    int init_i2c();               // higher level function to initialise i2c
//    int init_i2c(int address);
//    int init_i2c_due();           // initialise i2c for the due
//    int init_i2c_mega(int address);          // initialise 12c for the megas
//    int i2c_scanner();            // scan all addresses and make sure decive responds
//    int i2c_scanner(int address); // test specific address
//
//    int generate_checksum();
//    int error_check();
//
//    int get_frame_code();
//    int get_data();             //extract data from frame
//
//    int toggle_debug();         //toggle the debug function
//
//    int get_address();              //function to get the i2c address based on the hardware pin configuration
//
//    int startup_handshake();      //startup sequence to ensure due boots first and transmission begins when all megas are ready
//
//
//};
//
//
//
////methods
//
//
//sign_coms::sign_coms() {}
//
//
//int sign_coms::init_serial() {      // initialise serial at set baud rate speed of 115200
//
//  int speed = 115200;
//  //if (debug){      //if debug mode, init serial, if debug false dont bether with this
//  int alpha = millis();
//
//  //Serial.begin(speed);
//  while (!Serial) {
//    if (millis() > alpha + 5000) {  //after 5 seconds elapsed, assume serial failed to initialise
//      debug = false;
//      return -1;
//    }
//  }
//
//#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
//  Serial1.begin(COMS_SPEED);    //init comms link , serial1 on megas
//  Serial.println(F("Soft com initialised"));
//#else
//
//
//  //init four soft serial objects on due
//  Serial_1.begin(
//    RX_PIN_1,
//    TX_PIN_1,
//    SOFT_UART_BIT_RATE,
//    soft_uart::data_bit_codes::EIGHT_BITS,
//    soft_uart::parity_codes::NO_PARITY,
//    soft_uart::stop_bit_codes::ONE_STOP_BIT
//  );
//  Serial_2.begin(
//    RX_PIN_2,
//    TX_PIN_2,
//    SOFT_UART_BIT_RATE,
//    soft_uart::data_bit_codes::EIGHT_BITS,
//    soft_uart::parity_codes::NO_PARITY,
//    soft_uart::stop_bit_codes::ONE_STOP_BIT
//  );
//  Serial_3.begin(
//    RX_PIN_3,
//    TX_PIN_3,
//    SOFT_UART_BIT_RATE,
//    soft_uart::data_bit_codes::EIGHT_BITS,
//    soft_uart::parity_codes::NO_PARITY,
//    soft_uart::stop_bit_codes::ONE_STOP_BIT
//  );
//  Serial_4.begin(
//    RX_PIN_4,
//    TX_PIN_4,
//    SOFT_UART_BIT_RATE,
//    soft_uart::data_bit_codes::EIGHT_BITS,
//    soft_uart::parity_codes::NO_PARITY,
//    soft_uart::stop_bit_codes::ONE_STOP_BIT
//  );
//
//#endif
//
//  return 0;
//  // }
//  //   else
//  //   debug = false;
//  //   return 0;
//}
//
//
//int sign_coms::init_serial(int speed) {   // initialise serial at specified speed, must be standardised speed 115200 or below, otherwise error thrown
//  if (speed != 300 && speed != 600 && speed != 1200 && speed != 2400 && speed != 4800 && speed != 14400 && speed != 9600 && speed != 14400 && speed != 19200 && speed != 28800 && speed != 38400 && speed != 57600 && speed != 115200)
//    return (-2);
//  //if (debug){      //if debug mode, init serial, if debug false dont bether with this
//  int alpha = millis();
//
//  Serial.begin(speed);
//  while (!Serial) {
//    if (millis() > alpha + 5000) {  //after 5 seconds elapsed, assume serial failed to initialise
//      debug = false;
//      return -1;
//    }
//  }
//  return 0;
//  // }
//  //   else
//  //   debug = false;
//  //   return 0;
//}
//
//
//void sign_coms::stop() {      // emergency stop code
//  while (1) {}
//}
//
//
//int sign_coms::init_i2c() { //if init does not specify address call with address 5(safe value), if due, value not used
//#if defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)   //if mega find address
//  address = get_address();
//  init_i2c(address);    //initialise i2c
//#else
//  init_i2c_due();   //attach the i2c to wire1
//  //Serial.println(F("Pause for megas to initialise"));
//  //delay(500);
//  //init_i2c(0);      //check all i2c devices activated
//#endif
//}
//
//int sign_coms::init_i2c(int address) {   //initialise i2c by calling specify routines for this chipset
//
//  int alpha = 0;
//#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)          //define interrupts differently for mega and due, this code can run on either
//  alpha = init_i2c_mega(address);
//
//
//#else //due needs to test all i2c addresses to make sure the devices respond
//
//  alpha = i2c_scanner();   //test all addresses
//  if (alpha != 0)
//    Serial.println(F("Failed to detect 4 megas on i2c"));
//  //alpha = init_i2c_due();
//#endif
//  if (alpha < 0) {     //error if less than 0
//    
//      Sprintln("error setting up i2c, stopping program");
//      Sprintln("Error code: " + alpha);
//    
//    while (1) {}    //stop code
//  }
//
//}
//
//
//
//int sign_coms::init_i2c_due() {  //i2c init for the due specifically
//
//  //ensure the device running this is a due
//#if !defined(__AVR_ATmega1280__) && !defined(__AVR_ATmega2560__)  //if not mega, assume only due used otherwise
//
//  Wire1.begin();
//  Serial.println(F("i2c Setup done for due"));
//
//#else
//  Sprintln(F("Error, runinng due i2c init but using mega chipset"));
//  return (-1);
//#endif
//}
//
//
//int sign_coms::init_i2c_mega(int address) {    // i2c init for mega
//
//
//#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)    // if mega
//
//  Wire.begin(address);                // join i2c bus with address #4
//  Wire.onReceive(receiveEvent);       // register event
//  frame.disp_str.reserve(150);
//  Serial.println(F("I2c Setup Done for Mega"));
//  return (0);
//#else
//  Sprintln(F("Error, runinng mega i2c init but using due chipset"));
//  return (-1);
//#endif
//}
//
//
//int sign_coms::i2c_scanner() {           // scan all addresses and make sure decive responds
//
//#if !defined(__AVR_ATmega1280__) && !defined(__AVR_ATmega2560__)
//
//  // based on the i2c scanner methodology send dummy frame to mega
//  // check endTransmission() return value, if non zero there is an error
//  if (test_i2c) {
//    int fail;
//    for (int i = 1; i <= num_screens; i++) {
//      fail = i2c_scanner(i);
//      if (fail != 0) {
//          Sprint(F("Error connecting to i2c device:"));
//          Sprintln(i);
//        
//        return (-1);
//      }
//
//    }
//   
//    Sprintln(F("i2c return success"));
//    return (0);
//  }
//  else
//    return (0);
//
//#endif
//  return (-1);
//}
//
//
//int sign_coms::i2c_scanner(int address) { // test specific address
//
//#if !defined(__AVR_ATmega1280__) && !defined(__AVR_ATmega2560__)
//  Wire1.beginTransmission(address);
//  //for(int i=0; i<strlen(dummy_frame);i++){
//  //Wire1.write(dummy_frame[i]);
//  //}
//  int fail = Wire1.endTransmission();
//  return fail;
//#endif
//}
//
////int sign_coms::toggle_debug() {
////
////  if (debug) {
////    debug = false;
////  }
////  else
////    debug = true;
////}
//
////#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
////int sign_coms::get_address() {             //function to get the i2c address based on the hardware pin configuration
////  pinMode(hardware_address_pin[1], INPUT);
////  pinMode(hardware_address_pin[2], INPUT);
////
////  int alpha = digitalRead(hardware_address_pin[0]);
////  int beta = digitalRead(hardware_address_pin[1]);
////
////
////  if (alpha == LOW && beta == LOW)
////    address = 1;
////
////  if (alpha == LOW && beta == HIGH)
////    address = 2;
////
////  if (alpha == HIGH && beta == LOW)
////    address = 3;
////
////  if (alpha == HIGH && beta == HIGH)
////    address = 4;
////}
////#endif
//
//
//int sign_coms::startup_handshake() {  //code to delay the due in initialisation and enable mega startup
//
//#if !defined(__AVR_ATmega1280__) && !defined(__AVR_ATmega2560__)
//
//  pinMode(due_ready_pin, OUTPUT);           //due handshake
//  pinMode(mega1_ready_pin, INPUT_PULLUP);
//  pinMode(mega2_ready_pin, INPUT_PULLUP);
//  pinMode(mega3_ready_pin, INPUT_PULLUP);
//  pinMode(mega4_ready_pin, INPUT_PULLUP);
//
//  digitalWrite(due_ready_pin, HIGH);
//
//  bool ready = false;
//  int startup_time = millis();
//  while (!ready) {
//
//    bool mega1_ready = !digitalRead(mega1_ready_pin);
//    bool mega2_ready = !digitalRead(mega2_ready_pin);
//    bool mega3_ready = !digitalRead(mega3_ready_pin);
//    bool mega4_ready = !digitalRead(mega4_ready_pin);
//
//    if (!mega_enabled[0]) mega1_ready = true;
//    if (!mega_enabled[1]) mega2_ready = true;
//    if (!mega_enabled[2]) mega3_ready = true;
//    if (!mega_enabled[3]) mega4_ready = true;
//
//    //Serial.println(mega1_ready);
//    if (mega1_ready && mega2_ready && mega3_ready && mega4_ready)
//      ready = true;
//
//    if (millis() > startup_time + 5000) { //provide ample time for mega to start
//      Serial.println(F("Startup time exceeded, mega(s) not responding"));
//      return (-1);
//    }
//  }
//  return (0);
//
//
//#endif
//
//#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
//
//  //mega_startup
//  if (due_enabled) {
//    pinMode(mega_ready_pin, OUTPUT);
//    digitalWrite(mega_ready_pin, LOW);
//    return (0);
//  }
//  else
//    return (-1);
//#endif
//
//
//
//}
//
//
//
//
//

