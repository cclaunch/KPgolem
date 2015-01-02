//
// routine to punch a card image
//

void doPunch() {
  
  int      loopLength;
  boolean  gotRelease;
  unsigned int punchChar;
  unsigned int multiChar;
  int      numBits;
  int      multiPasses;
  
  // figure out length of line
  loopLength = punchLength;
    
  // if verify mode, figure length of last card
  if ((cmdVerify == true) && (lastLength > punchLength)) {
    loopLength = lastLength;
  }
  
  // set up curCol to 1
  curCol = 1;
  gotRelease = false;
  
// CVC  while (digitalRead(REG) == LOW) {
// CVC    Serial1.println(F("Register a card at the punch station"));
// CVC    delay(1000);
// CVC  }
  
  // If we are in verify mode and we had punched the prior card, preload read value with dummy cycle  
  if ((cmdVerify == true) && lastLength > 0) {
     getFirstCol();                                   // will be in variable readIn
  }
    
  // loop for work length
  for (int i=0; i < loopLength; i++) {
        
    // error msg for any invalid character
    if (punchBuffer[i] == 0x8000) {
      Serial1.print(F("Invalid character in column "));
      Serial1.print(curCol);
      Serial1.println(F(" during punch"));
      punchBuffer[i] = 0x1000;   // convert this to a space
    }
    
    // if past line, do space for remaining columns
    if (i >= punchLength) {
      punchChar = 0x1000;        // space character
    } else {
      punchChar = punchBuffer[i];
    }
    
    // watch latch to see if release done
    if ((gotRelease == true) && (i < punchLength)) {
      Serial1.println(F("Other characters follow release (newline) on same command"));
    }
    if ((punchChar == 0x2000)) {
      gotRelease = true;
      if (i <= punchLength) punchChar = 0x1000;                     // make space if still verifying last card
    }
    
    // check for multipunch and if so, hold button then loop in groups of 3 holes until done, then release button
    numBits = countBits(punchChar);
    multiPasses = 0;
    if (((cmdMode == 2) && (numBits > 1)) || (numBits > 3)) {      // multihole in binary or more than three holes with user table
      multiPasses = numBits;                                       // how many times to use multipunch
    }
    for (int j = multiPasses; j > 0; j--) {                        // too many holes, punch them with multipunch
      if (j == multiPasses) {
        digitalWrite(MPC, PICK);                                   // key pole 1 activate multipunch relay
        delay(100);                                                // give it time to actuate
      }
      multiChar = pickOne(punchChar);                              // get next hole to punch
      punchCol(multiChar);                                         // do these holes under multipunch
      punchChar = punchChar ^ multiChar;                           // flip off those holes we just punched
    }

    // now punch character or hole that is remaining if we did a multipunch
    punchCol(punchChar);
    
    // if we were doing multipunch, release it and wait
    if (multiPasses > 0) {
      digitalWrite(MPC, DROP);
      delay (100);
    }
    
    // increment curCol
    curCol++;
    
    // if verify, and not beyond length of last card, check readIn against last card column, flag error
    if ((cmdVerify == true) && (lastLength > 0) && (i <= lastLength)) {
      if (lastCard[i] != readIn) {
        Serial1.print(F("WARN Verify error column "));
        Serial1.println(curCol-1);
        Serial1.print("lastcard value for column "); // CVC
        Serial1.print(i); // CVC
        Serial1.print("was "); // CVC
        Serial1.print(lastCard[i], HEX); // CVC
        Serial1.print(" but what we read was "); // CVC
        Serial1.println(readIn, HEX); // CVC
      }
    }
 }                                                                // end loop through columns to punch and verify
  
  // if rel latch not set and line < 80, issue release
  if ((curCol <81) && (gotRelease == false)) {
    punchChar = 0x2000;          // release
    punchCol(punchChar);
 }
  
  // if verify mode, save card image in lastCard for use in next punch cycle
  
  if (cmdVerify == true) {
    for (int i = 0;i < punchLength;i++) {
      lastCard[i] = punchBuffer[i];
    }
    lastLength = punchLength;
    Serial1.println(F("Last card verified during punch of this card"));
  } 

// CVC  Serial1.println(F("OK Punch operation complete"));
  Serial.println(F("OK Punch operation complete")); // CVC
  return;
}
