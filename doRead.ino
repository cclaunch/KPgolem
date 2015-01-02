//
// doRead routine to read cards 
//

void  doRead() {
  
  unsigned int found;
  char     nextChar;
  
  while (digitalRead(REG) == LOW) {
    Serial1.println(F("Register card to be read at dup station and another card at the punch station"));
    delay(1000);
  }
  
  // go pick up the data from column 1 of the card in the read (duplicate or master) station
  getFirstCol();                                   // will be in variable readIn
  
  // begin outputing the card
  Serial1.print(F("OK Read card:"));
  
  // Now space through the 80 columns to pick up the rest of the data  
  for (int i = 0; i < 80; i++) {
    found = readIn;                                // pick up value from last pass since reading leads punching by one column
    punchCol(0x1000);  // punch a space
    // reading is done during the punch cycle in punchCol and saved in readIn
    
    // translate input back with table or binary method
    switch (cmdMode) {
      case 1:      // ascii
        if (found == 0) found = 0x1000;            // no holes is a space
        nextChar = ' ';
        for (int l = 0; l < 128; l++) {            // loop thru ASCII table
          if (asciiTab[l] == found) {              // did we find the hole pattern in the table?
            nextChar = char(l);                    // position is the ASCII character
            break;                                 // go with first match found
          }
        }
        Serial1.print(nextChar);
        break;
        
      case 2:      // binary
        if (found == 0) {
          Serial1.print(F("1000 "));   
        } else {
          Serial1.print(F("0"));
          nextChar = getHex((found & 0x0F00) >> 8);
          Serial1.print(nextChar);
          found &= 0x00FF;
          nextChar = getHex((found & 0x00F0) >> 4);
          Serial1.print(nextChar);
          found &= 0x000F;
          nextChar = getHex(found & 0x000F);
          Serial1.print(nextChar);
          Serial1.print(F(" "));
        }
        break;
        
      case 3:      // user table is really same ASCII table, but reloaded by user
        if (found == 0) found = 0x1000;            // no holes is a space
        nextChar = ' ';
        for (int l = 0; l < 128; l++) {            // loop thru ASCII table
          if (asciiTab[l] == found) {              // did we find the hole pattern in the table?
            nextChar = char(l);                    // position is the ASCII character
            Serial1.print(nextChar);               // and output it
            break;                                 // go with first match found
          }
        }
        break;
        
      default:
        Serial1.println(F("Internal error, mode is invalid during read"));
        return;
    }
  }
  
  // release the card
  punchCol(0x2000);
  Serial1.println(F(" "));
  
  return;
}
