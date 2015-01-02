// 
// this is the standard ASCII table as setup in original keypunch interface, designed for use with 1401 system and BCD characters
//
void  loadASCII() {
    boolean  choice1;
    choice1 = (cmdCode == 1);
    for (int i = 0; i < 128; i++) {
      if (choice1) {
        asciiTab[i] = pgm_read_word_near(ebcdicTab + i);
      } else {
        asciiTab[i] = pgm_read_word_near(bcdTab + i);
      }
    }    
    return;
}

//
// this routine clears the table before loading a user table
//
void  clearASCII() {
  for (int i = 0; i < 128; i++) {
    asciiTab[i] = 0x8000;  // invalid entry marker
  }
  return;
}
