//
// punchCol will punch one column on the keypunch
//
//    release is 510ms plus 10ms per column remaining on current card
//         punch 1 col, it is 1250ms to release (actual time)
//         punch all 80 col, it is 500 ms to release (actual)
//

void  punchCol(unsigned int data) {
  
  unsigned long    startTime;
  unsigned long    endTime;
  uint8_t          cardRow[13];
  
    startTime = millis();                // timestamp 
  if (bitRead(data, 13) == 1) {          // release required
    endTime = startTime + RELEASETIME;
    digitalWrite(REL, PICK);            // turn on the release magnet
    endTime+= 500;                      // fixed delay in release operation
    endTime+= (80-curCol)*10;           // additional delay for remaining columns
    waitTime(endTime);                  // hang out until card has moved sufficiently
    digitalWrite(REL, DROP);            // stop the release now
    
  } else if (bitRead(data, 12) == 1) {  // space in this column
    endTime = startTime + PUNCHTIME;
    writeKP(0x1000);
    waitTime(endTime);
    writeKP(0x0000);
        
    //  now read the state of the pins in the read station if we need it
    if ((cmdLine[0] == 'R') || ((cmdVerify == true) && (curCol <= lastLength))) {
      waitTime(endTime+READTIME);                // wait until optimum time to sense the pins
      readIn = readKP();                         // go pick up state of the sense pins
      if (readIn == 0) readIn = 0x1000;          // make it a space if no columns had holes  
    }
    endTime+= CYCLETIME;                        // wait for punch cycle to end
    waitTime(endTime);
    
  } else {
    endTime = startTime + PUNCHTIME;
    writeKP(data);
    waitTime(endTime); 
    writeKP(0);
    
    //  now read the state of the pins in the read station if we need it
    if ((cmdLine[0] == 'R') || ((cmdVerify == true) && (curCol <= lastLength))) {
      // wait an extra bit to sense the pins
      waitTime(endTime + READTIME);
      readIn = readKP(); // go get the value of the sense pins
      if (readIn == 0) readIn = 0x1000;          // make it a space if no columns had holes  
    }
    endTime += CYCLETIME;    // wait for punch cycle to end
    waitTime(endTime);
  }
  
  return;
  
}

// routine to wait until a specific time
void  waitTime(unsigned long target) {
  
  unsigned long rightNow;
  unsigned long carlSense;

  // wait until the expiration of the interval
  while ((rightNow = millis()) < target) {
    if ((target - rightNow) < 2) {
      delay(1);
    } else {
      delay (target - rightNow - 1);
    }      
  }
  return;
}
