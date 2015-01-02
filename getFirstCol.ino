//
// routine to issue a dummy punch cycle in order to read the holes in column one of the master card (card in dup station)
//      fires the punch clutch without having done an escapement cycle first
//      waits around through the cycle, picking up the pattern of holes at the relevant time
//

void  getFirstCol(void) {
  
  // trigger the dummy punch cycle (no holes punched and does not advance cards to the next column
  digitalWrite(COL1, PICK);                        // fire punch clutch with no holes selected
  
  // give it enough time to begin moving, then shut off the punch clutch
  delay(FIRSTCOL);                                 // let it start moving
  digitalWrite(COL1, DROP);                        // release the punch clutch
  
  // now wait until the best time for reading the pin values
  delay(SENSECOL);                                 // now wait for the sense pins to engage
  readIn = readKP();                               // get the value of the first column
  
  delay(CYCLETIME);                                // make sure this cycle is wrapped up first
  
  return;
}

