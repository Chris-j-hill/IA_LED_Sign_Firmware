// value at index is the bit that is incorrect

uint32_t code_rx = 375753728;
uint32_t word_rx = 55534;

//syndrome total is bit error, this is sorted set of bit erorrs, syndrome total is index, bit to flip is value
const byte syndrome_bit_error[] PROGMEM = {1, 2, 19, 3, 6, 20, 12, 4, 30, 7, 28, 21, 9, 13, 24, 5, 11, 31, 18, 8, 23, 29, 27, 22, 26, 10, 17, 14, 15, 25, 16};
const uint32_t hamming_H_matrix[] PROGMEM = { 0x4259F1BA,
                                              0x212CF8DD,
                                              0x12CF8DD4,
                                              0x0967C6EA,
                                              0x04B3E375
                                            };

const byte parity[] PROGMEM =//LUT for parity check, byte value is index, content is 0 if even parity, 1 if odd
{
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0,
  1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1,
  1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1,
  0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0,
  1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1,
  0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0,
  0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1,
  0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1,
  1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1,
  0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0,
  0, 1, 0, 1, 1, 0
};

byte syndrome = 0;
//byte data[4] = {0};


inline uint32_t get_syndrome(uint32_t code_word) {
  syndrome = 0;
  syndrome += (numberOfSetBits(code_word & pgm_read_dword_near(hamming_H_matrix + 0)) & 1);         // multiply row and code_word (&), count num set bits, mod 2 (&1)
  syndrome += (numberOfSetBits(code_word & pgm_read_dword_near(hamming_H_matrix + 1)) & 1) << 1;    // error bit = binary value of syndrome
  syndrome += (numberOfSetBits(code_word & pgm_read_dword_near(hamming_H_matrix + 2)) & 1) << 2;
  syndrome += (numberOfSetBits(code_word & pgm_read_dword_near(hamming_H_matrix + 3)) & 1) << 3;
  syndrome += (numberOfSetBits(code_word & pgm_read_dword_near(hamming_H_matrix + 4)) & 1) << 4;
  if (syndrome != 0) {
    return (code_word ^ (1 << (31 - (pgm_read_byte_near(syndrome_bit_error + syndrome - 1)))));
  }
  return code_word;
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  for (byte i = 0; i < 31; i++) {
    code_rx = code_rx ^ (0x1 << i);           //apply bit error
    uint32_t error_corrected_rx = get_syndrome(code_rx);
    code_rx = code_rx ^ (0x1 << i);           //reverse bit error
    if (code_rx != error_corrected_rx) {      //check for failed correction
      Serial.print(i);
      Serial.println(" corrupted");
    }
  }

  int millis_read_time = micros() - micros();
  int start_time = micros();
  for (int i = 0; i < 4096; i++) {
    get_syndrome(code_rx);
  }
  Serial.println((micros() - start_time - millis_read_time) / 4096);

}


void loop() {
  // put your main code here, to run repeatedly:

}

inline byte numberOfSetBits(uint32_t i)
{
//  http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetNaive
  i = i - ((i >> 1) & 0x55555555);
  i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
  return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

inline void extract_bytes(uint32_t code_word) {

  // code_word bit layout
  //   hhhhhlll
  //   lllptttp
  //   nnnpfffp
  //   oooocccp

  // h = hamming matrix parity bits
  // l = frame length bits
  // p = parity check of preceeding data
  // t = frame type bits
  // n = frame num bits
  // f = this frame bits
  // o = obj_num bits
  // c = checksum bits

  byte data[4] = {0};
  data[0] = ((code_word >> 20) & 0x7F); //shift down to include parity, then confirm parity correct
  data[1] = ((code_word >> 16) & 0xF);
  data[2] = ((code_word >> 8) & 0xFF);
  data[3] = ((code_word >> 8) & 0xFF);

  bool parity_ok = check_header_parity(data);
  if(!parity_ok){
    //attempt recovery
    //request frame retransmit
  }
  else{
    //save bytes
  }
  

}

inline bool check_header_parity(byte data[4]) {

  return (parity[data[0]] && parity[data[1]] && ((parity[data[2]] >> 4) & 0xF) && (parity[data[2]] & 0xF) && parity[data[3]]);

  }

//
//// value at index is the bit that is incorrect
////uint32_t code_rx = 1239182336;
//uint32_t code_rx = 375753728;
////uint32_t code_rx =1626932;
//uint32_t word_rx = 55534;
//
//
//
////const byte syndrome_bit_error[] PROGMEM = {31, 1, 11, 2, 8, 12, 27, 3, 25, 9, 15, 13, 22, 28, 17, 4, 7, 26, 30, 10, 21, 16, 24, 14, 20, 23, 6, 29, 255, 18, 19, 5};// LUT for syndrome, five bit error vector equates to index in this
//
////syndrome total is bit error, this is sorted set of bit erorrs, syndrome total is index, bit to flip is value
//const byte syndrome_bit_error[] PROGMEM = {1, 2, 19, 3, 6, 20, 12, 4, 30, 7, 28, 21, 9, 13, 24, 5, 11, 31, 18, 8, 23, 29, 27, 22, 26, 10, 17, 14, 15, 25, 16};
//const uint32_t hamming_H_matrix[] PROGMEM = { 0x4259F1BA,
//                                              0x212CF8DD,
//                                              0x12CF8DD4,
//                                              0x0967C6EA,
//                                              0x04B3E375
//                                            };
//
////const uint32_t hamming_H_matrix[] PROGMEM = {    0x2EC7CD21,
////                                                 0x5D8F9A42,
////                                                 0x15D8F9A4,
////                                                 0x2BB1F348,
////                                                 0x5763E690
////                                            };
//byte syndrome = 0;
//
//
//
//
//inline uint32_t get_syndrome(uint32_t code_word) {
//  syndrome = 0;
//  syndrome += (numberOfSetBits(code_word & pgm_read_dword_near(hamming_H_matrix + 0)) & 1);         // multiply row and code_word (&), count num set bits, mod 2 (&1)
//  syndrome += (numberOfSetBits(code_word & pgm_read_dword_near(hamming_H_matrix + 1)) & 1) << 1;    // error bit = binary value of syndrome
//  syndrome += (numberOfSetBits(code_word & pgm_read_dword_near(hamming_H_matrix + 2)) & 1) << 2;
//  syndrome += (numberOfSetBits(code_word & pgm_read_dword_near(hamming_H_matrix + 3)) & 1) << 3;
//  syndrome += (numberOfSetBits(code_word & pgm_read_dword_near(hamming_H_matrix + 4)) & 1) << 4;
//  //  Serial.println(code_word,BIN);
//  //  Serial.println(syndrome);
//  if (syndrome != 0) {
//    //    Serial.println(pgm_read_byte_near(syndrome_bit_error + syndrome-1));
//    //    Serial.println(code_word ^ (1 << ((31-pgm_read_byte_near(syndrome_bit_error + syndrome-1)))),BIN);
//    return (code_word ^ (1 << (31 - (pgm_read_byte_near(syndrome_bit_error + syndrome - 1)))));
//  }
//  return code_word;
//}
//
//void setup() {
//  // put your setup code here, to run once:
//
//  Serial.begin(115200);
//  Serial.println();
//  Serial.println();
//  Serial.println();
//  Serial.print("00");
//  Serial.println(code_rx, BIN);
//  Serial.println(pgm_read_dword_near(hamming_H_matrix + 0), BIN);
//  //  Serial.println();
//  //  Serial.println();
//  uint32_t original_rx = code_rx;
//  for (byte i = 0; i < 31; i++) {
//    code_rx = code_rx ^ (0x1 << i);
//    Serial.println(code_rx, BIN);
//    uint32_t error_corrected_rx = get_syndrome(code_rx);
//    if (original_rx != error_corrected_rx) {
//      Serial.print(i);
//      Serial.println(" corrupted");
//      Serial.println(original_rx, HEX);
//      Serial.println(error_corrected_rx, HEX);
//    }
//    code_rx = code_rx ^ (0x1 << i);
//  }
//
//  //Serial.println(syndrome);
//
//}
//
//
//void loop() {
//  // put your main code here, to run repeatedly:
//
//}
//
//byte bitCount (uint32_t value) {
//  int count = 0;
//  while (value > 0) {           // until all bits are zero
//    if ((value & 1) == 1)     // check lower bit
//      count++;
//    value >>= 1;              // shift bits, removing lower bit
//  }
//  return count;
//}
//
//inline byte numberOfSetBits(uint32_t i)
//{
//  // Java: use >>> instead of >>
//  // C or C++: use uint32_t
//  i = i - ((i >> 1) & 0x55555555);
//  i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
//  return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
//}
