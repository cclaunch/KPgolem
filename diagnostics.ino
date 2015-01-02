//
// Diagnostic routines, called by number
//

//
// Diag 0 - short test of basic connectivity and status
//     just verifies signal states as they sit
//
void  diag0() {
  int  pchtest;
  int  rdrtest;
  
  Serial1.println(F("Starting diagnostic 0 routine"));
  digitalWrite(PCHT1, LOW);
  pchtest = digitalRead(PCHT2);
  digitalWrite(PCHT1, HIGH);
  if ((digitalRead(PCHT2) == LOW) || (pchtest == HIGH)) {
    Serial1.println(F("Punch cable not attached"));
    return;
  } else {
    Serial1.println(F("Punch cable verified"));
  }
  digitalWrite(RDRT1, LOW);
  rdrtest = digitalRead(RDRT2);
  digitalWrite(RDRT1, HIGH);
  if ((digitalRead(RDRT2) == LOW) || (rdrtest == HIGH)) {
    Serial1.println(F("Reader cable not attached"));
    haveReadCable = false;
  } else {
    Serial1.println(F("Reader cable verified"));
    Serial1.print(F("Read station holes detected:"));
    if (digitalRead(RDR12) == HIGH) Serial1.print(F(" 12 "));
    if (digitalRead(RDR11) == HIGH) Serial1.print(F(" 11 "));
    if (digitalRead(RDR0)  == HIGH) Serial1.print(F(" 0 "));
    if (digitalRead(RDR1)  == HIGH) Serial1.print(F(" 1 "));
    if (digitalRead(RDR2)  == HIGH) Serial1.print(F(" 2 "));
    if (digitalRead(RDR3)  == HIGH) Serial1.print(F(" 3 "));
    if (digitalRead(RDR4)  == HIGH) Serial1.print(F(" 4 "));
    if (digitalRead(RDR5)  == HIGH) Serial1.print(F(" 5 "));
    if (digitalRead(RDR6)  == HIGH) Serial1.print(F(" 6 "));
    if (digitalRead(RDR7)  == HIGH) Serial1.print(F(" 7 "));
    if (digitalRead(RDR8)  == HIGH) Serial1.print(F(" 8 "));
    if (digitalRead(RDR9)  == HIGH) Serial1.print(F(" 9 "));
    Serial1.println(F(" "));
    haveReadCable = true;
  }
  if (digitalRead(REG) == HIGH) {
    Serial1.println(F("Card is registered at punch station"));
  } else {
    Serial1.println(F("No Card registered at punch station"));
  }
  Serial1.println(F("Diag 0 completed"));
}

//
// Diag 1 - test of punching, reading and other functions
//   intended for use with keypunch and two blank cards
//
void  diag1() {
  
  unsigned long timeout;
  boolean cmdSave;
  
  Serial1.println(F("Beginning level 1 testing now"));
  
  timeout = millis();
  // now verify registration and punch 12 row values, one per column
  if (digitalRead(REG) == 0) Serial1.println(F("Make sure card is registered at punch station"));
  while (digitalRead(REG) == 0) {
    delay(100);
    if ((millis() - timeout) > 5000) {
      Serial1.println(F("Card not registered in punch station"));
      return;
    }
  }
  cmdSave = cmdVerify;
  cmdVerify = false;
  punchCol(0x0800);      // row 12
  punchCol(0x0400);      // row 11
  punchCol(0x0200);      // row 0
  punchCol(0x0100);      // row 1
  punchCol(0x0080);      // row 2
  punchCol(0x0040);      // row 3
  punchCol(0x0020);      // row 4
  punchCol(0x0010);      // row 5
  punchCol(0x0008);      // row 6
  punchCol(0x0004);      // row 7
  punchCol(0x0002);      // row 8
  punchCol(0x0001);      // row 9
  
  // now hold multipunch down
  digitalWrite(MPC, PICK);                                   // hold down multipunch key
  delay(100);                                                // give it time to actuate
  
  // now type 12 0 2 4 6 8
  punchCol(0x0A00);      // row 12 and 0
  punchCol(0x00A0);      // row 2 and 4
  punchCol(0x000A);      // row 6 and 8
  
  // now release multipunch
  digitalWrite(MPC, DROP);                                   // hold down multipunch key
  delay(100);                                                // give it time to actuate
  
  // now space one
  punchCol(0x1000);      // space
  
  // now hold multipunch down
  digitalWrite(MPC, PICK);                                   // hold down multipunch key
  delay(100);                                                // give it time to actuate
    
  // now type 11 1 3 5 7 9
  punchCol(0x0500);      // row 11 and 1
  punchCol(0x0040);      // row 3
  punchCol(0x0010);      // row 5
  punchCol(0x0005);      // row 7 and 9
  
  // now release multipunch
  digitalWrite(MPC, DROP);                                   // hold down multipunch key
  delay(100);                                                // give it time to actuate
  
  // now activate release
  Serial1.println(F("Done punching card, ready to verify"));
  punchCol(0x2000);
  
  // bail out if no reader cable since we can't verify
  if (haveReadCable == false) {
    Serial1.println(F("No reader cable so automatic verification not possible"));
    return;
  }
  
  // set up lastCard so we can verify the correctness of the output
  cmdVerify = true;
  lastCard[0] = 0x0800;
  lastCard[1] = 0x0400;
  lastCard[2] = 0x0200;
  lastCard[3] = 0x0100;
  lastCard[4] = 0x0080;
  lastCard[5] = 0x0040;
  lastCard[6] = 0x0020;
  lastCard[7] = 0x0010;
  lastCard[8] = 0x0008;
  lastCard[9] = 0x0004;
  lastCard[10] = 0x0002;
  lastCard[11] = 0x0001;
  lastCard[12] = 0x0AAA;
  lastCard[13] = 0x1000;
  lastCard[14] = 0x0555;
  lastCard[15] = 0x1000;
  for (int i = 16; i < 80;i++) {
    lastCard[i] = 0x1000;
  }
  
  timeout = millis();
  if (digitalRead(REG) == 0) Serial1.println(F("Ensure punched card registered in read station and additional card registered in punch station"));
  while (digitalRead(REG) == 0) {
    delay (100);
    if ((millis() - timeout) > 30000) {
      Serial1.println(F("Timeout waiting for next card to be registered in punch station to enable verification"));
      return;
    }
  }
  // now space 12 times while verifying the 12 rows were punched correctly
  Serial1.println(F("Spacing 12 times and verifying the 12 single row punches"));
  punchCol(0x1000);
  punchCol(0x1000);
  punchCol(0x1000);
  punchCol(0x1000);
  punchCol(0x1000);
  punchCol(0x1000);
  punchCol(0x1000);
  punchCol(0x1000);
  punchCol(0x1000);
  punchCol(0x1000);
  punchCol(0x1000);
  punchCol(0x1000);

  // space 1 and verify multipunch 12-0-2-4-6-8
  Serial1.println(F("Spacing to verify the multipunch of even rows"));
  punchCol(0x1000);
  
  // space 1 and verify space
  Serial1.println(F("Spacing to verify the space between multipunches"));
  punchCol(0x1000);
  
  // space 1 and verify multipunch 11-1-3-5-7-9
  Serial1.println(F("Spacing to verify the multipunch of odd rows"));
  punchCol(0x1000);
  
  // space rest of card and verify no punches
  Serial1.println(F("Spacing rest of card to verify no holes punched"));
  for (int i = 0; i < 65; i++) {
    punchCol(0x1000);
  }
  
  // restore the prior state of the verification
  cmdVerify = cmdSave;
  
  Serial1.println(F("Diagnostic level 1 completed"));
  return;
}

