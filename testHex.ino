//
// testHex will validate a string contains only valid hex characters
//       which are 0 - 9, a, b, c, d, e, or f
//       returns true if valid, false if nonhex character found
//

boolean  testHex(int x) {
  
  if (strlen((const char *)cmdLine + x) != 4) {  // safety test
    return false;
  }
  for (int i = 0; i < 4; i++) {
    switch (cmdLine[x+i]) {      // check for valid characters
      case '0':
        continue;
      case '1':
        continue;
      case '2':
        continue;
      case '3':
        continue;
      case '4':
        continue;
      case '5':
        continue;
      case '6':
        continue;
      case '7':
        continue;
      case '8':
        continue;
      case '9':
        continue;
      case 'a':
        continue;
      case 'b':
        continue;
      case 'c':
        continue;
      case 'd':
        continue;
      case 'e':
        continue;
      case 'f':
        continue;
      default:
        return false;
    }
  }  
  return true;                  // checked all four and they are good
}
