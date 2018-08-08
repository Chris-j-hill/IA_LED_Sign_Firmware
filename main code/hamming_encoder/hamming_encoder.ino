

const uint32_t hamming_G_matrix_32_26[] = {0x259F1BA,    //matlab generation code >> [h, g, n, k] = hammgen(5); binaryVector2Hex(transpose(g))
                                           0x12CF8DD,
                                           0x2CF8DD4,
                                           0x167C6EA,

                                           0x0B3E375,
                                           0x2000000,
                                           0x1000000,
                                           0x0800000,

                                           0x0400000,
                                           0x0200000,
                                           0x0100000,
                                           0x0080000,

                                           0x0040000,
                                           0x0020000,
                                           0x0010000,
                                           0x0008000,

                                           0x0004000,
                                           0x0002000,
                                           0x0001000,
                                           0x0000800,

                                           0x0000400,
                                           0x0000200,
                                           0x0000100,
                                           0x0000080,

                                           0x0000040,
                                           0x0000020,
                                           0x0000010,
                                           0x0000008,

                                           0x0000004,
                                           0x0000002,
                                           0x0000001
                                          };



byte data[4] = {45, 3, 51 , 71};   //actual data from frame
byte shifts[4] = {17, 13, 5, 3}; //distance in word to shift data

void printzeros(uint32_t var) {
  for (uint32_t test = 0x40000000; test; test >>= 1) {
    Serial.write(var  & test ? '1' : '0');
  }
  //  Serial.println();
}
void printzeros_8(uint32_t var) {
  for (uint32_t test = 0x80; test; test >>= 1) {
    Serial.write(var  & test ? '1' : '0');
  }
  //  Serial.println();
}
uint32_t matlab = 0b1010000010110100110011001101001;
void setup() {
  // put your setup code here, to run once:
  byte to_send[4] = {0};
  Serial.begin(250000);
  uint32_t uncoded = (data[0] << shifts[0]) | (data[1] << shifts[1]) | (data[2] << shifts[2]) | (((data[3] & 0b11111110) >> shifts[3]) | (data[3] & 0b00000001));
  Serial.print ("uncoded = ");
  printzeros(uncoded);
  Serial.println();
  uint32_t encoded = hamming_encode_32_26(uncoded);
  Serial.println();
  Serial.print("matlab  = ");
  printzeros(matlab);

  dword_to_header(to_send, encoded);
  Serial.println();
  Serial.print("byte 1= ");
  printzeros_8(to_send[0]);
  Serial.println();
  Serial.print("byte 2= ");
  printzeros_8(to_send[1]);
  Serial.println();
  Serial.print("byte 3= ");
  printzeros_8(to_send[2]);
  Serial.println();
  Serial.print("byte 4= ");
  printzeros_8(to_send[3]);
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:

}


uint32_t hamming_encode_32_26(uint32_t uncoded) {

  uint32_t encoded = 0;
  uncoded = uncoded& 0x
  for (byte i = 0; i < 31; i++) {
    encoded = (encoded | ((numberOfSetBits(uncoded & hamming_G_matrix_32_26[i]) & 1) << (30 - i)));
  }
  Serial.print ("encoded = ");
  printzeros(encoded);
  return encoded;
}



inline byte numberOfSetBits(uint32_t i)
{
  //  http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetNaive
  i = i - ((i >> 1) & 0x55555555);
  i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
  return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

void dword_to_header(byte *buf, uint32_t encoded) {

  buf[0] = encoded >> 24;
  buf[1] = (encoded >> 16) & 0xFF;
  buf[2] = (encoded >> 8) & 0xFF;
  buf[3] = encoded & 0xFF;
}


