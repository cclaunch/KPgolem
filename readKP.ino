// 
// routine to read the state of the pin sense lines of the keypunch rapidly and as a single operation
// returning an unsigned int that conforms to our layout (bits 11 to 0 are rows 12, 11, 0, 1 . . . 9 of the card)
//
unsigned int  readKP() {
  
  byte  portB, portE, portG, portH;
  unsigned int temp;
  
  // reset work variable
  temp = 0;
  
  // get the values from the ports
  portB = PINB & B11110000;  // pins 13 12 11 and 10
  portE = PINE & B00111000;  // pins 3 2 and 5
  portG = PING & B00100000;  // pin 4
  portH = PINH & B01111000;  // pins 9 8 7 and 6
  
  // build up the long word with x x x x 12 11 0 1 2 3 4 5 6 7 8 9 values (x is forced to off)
  bitWrite(temp, 11, bitRead(portE, 4)); // pin  2 which is RDR12 
  bitWrite(temp, 10, bitRead(portE, 5)); // pin  3 which is RDR11
  bitWrite(temp,  9, bitRead(portG, 5)); // pin  4 which is RDR0
  bitWrite(temp,  8, bitRead(portE, 3)); // pin  5 which is RDR1
  bitWrite(temp,  7, bitRead(portH, 3)); // pin  6 which is RDR2
  bitWrite(temp,  6, bitRead(portH, 4)); // pin  7 which is RDR3
  bitWrite(temp,  5, bitRead(portH, 5)); // pin  8 which is RDR4
  bitWrite(temp,  4, bitRead(portH, 6)); // pin  9 which is RDR5
  bitWrite(temp,  3, bitRead(portB, 4)); // pin 10 which is RDR6
  bitWrite(temp,  2, bitRead(portB, 5)); // pin 11 which is RDR7
  bitWrite(temp,  1, bitRead(portB, 6)); // pin 12 which is RDR8
  bitWrite(temp,  0, bitRead(portB, 7)); // pin 13 which is RDR9
  
  // return our value
  return temp;
}
