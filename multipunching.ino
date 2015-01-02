// 
// countBits routine for multipunching
//    will return the number of holes
//    that are being punched

int    countBits(unsigned int punchChar) {
  
  int    buildCount = 0;
  
  for (int i = 12; i > 0; i--) {                      // go through the low order 12 bits
    if (bitRead(punchChar, i-1) == 1)  buildCount++;  // bump if the selected bit is on
  }
  return buildCount;
}


// 
// pickOne routine for multipunching
//    will return an unsigned long with the next highest bit on
//

unsigned int  pickOne(unsigned int punchChar) {
  
  unsigned int buildChar = 0;
  int          buildCount = 0;
  
  for (int i = 12; i > 0; i--) {
    if (bitRead(punchChar, i-1) == 1) {
      bitSet(buildChar, i-1);
      return buildChar;
    }
  }
  return buildChar;
}

