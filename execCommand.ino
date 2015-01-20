#include <avr/io.h>
#include <avr/pgmspace.h>
//
// Executes the verbs after they have been parsed and validated
//

void  execCommand() {
  
      unsigned int buildvalue;
      char showme[2];
  
  // stop user from sending more input until we process long duration execution actions
  Serial1.print(char(XOFF));
  
  if (cmdLine[0] == 'V') {
      // verification state already set, so just exit with confirmation message
      Serial1.print(F("OK Verify mode is "));
      if (cmdVerify == true) {
        Serial1.println(F("ON"));
      } else {
        Serial1.println(F("OFF"));
      }
      
  } else if (cmdLine[0] == 'I') {
      Serial1.print(F("Idle received"));
      
  } else if (cmdLine[0] == 'M') {
      // verification state already set, so just exit with confirmation message
      switch (cmdMode) {
        case 1:
          Serial1.println(F("OK Mode is ASCII"));
          loadASCII();
          break;
        case 2:
          Serial1.println(F("OK Mode is BINARY"));
          break;
        case 3:
          Serial1.println(F("OK Mode is USERTAB"));
          clearASCII();
          break;
        default:
          Serial1.println(F("Mode is invalid choice, reset to ASCII"));
          cmdMode = 1;
          loadASCII();
          break;
      }
      
  } else if (cmdLine[0] == 'C') {
      // code table already set, so just exit with confirmation message
      switch (cmdCode) {
        case 1:
          Serial1.println(F("OK ASCII input to encode EBCDIC"));
          break;
        case 2:
          Serial1.println(F("OK ASCII input to encode BCD"));
          break;
        default:
          Serial1.println(F("Code is invalid choice, reset to EBCDIC"));
          cmdCode = 1;
          break;
      }
      
  } else if (cmdLine[0] == 'L') {
    
    // load usertable entry 'digit' with value of 'choice'
    buildvalue = 0;
    Serial1.print(F("OK Placing value "));
    for (int i = 0; i < 4; i++) {
      switch (choice[i]) {
        case '0':
          buildvalue = buildvalue << 4;
          break;
        case '1':
          buildvalue = (buildvalue << 4) + 1;
          break;
        case '2':
          buildvalue = (buildvalue << 4) + 2;
          break;
        case '3':
          buildvalue = (buildvalue << 4) + 3;
          break;
        case '4':
          buildvalue = (buildvalue << 4) + 4;
          break;
        case '5':
          buildvalue = (buildvalue << 4) + 5;
          break;
        case '6':
          buildvalue = (buildvalue << 4) + 6;
          break;
        case '7':
          buildvalue = (buildvalue << 4) + 7;
          break;
        case '8':
          buildvalue = (buildvalue << 4) + 8;
          break;
        case '9':
          buildvalue = (buildvalue << 4) + 9;
          break;
        case 'a':
          buildvalue = (buildvalue << 4) + 10;
          break;
        case 'b':
          buildvalue = (buildvalue << 4) + 11;
          break;
        case 'c':
          buildvalue = (buildvalue << 4) + 12;
          break;
        case 'd':
          buildvalue = (buildvalue << 4) + 13;
          break;
        case 'e':
          buildvalue = (buildvalue << 4) + 14;
          break;
        case 'f':
          buildvalue = (buildvalue << 4) + 15;
          break;
        default:
          Serial1.print(F("Improper hex input digit "));
          Serial1.print(choice[i], HEX);
          Serial1.println(F(" for LOAD command"));
          return;
      }
      if ((bitRead(buildvalue, 15) == 1) && ((buildvalue & 0x7FFF) != 0)) {
        cmdLine[0] = ' ';
        Serial1.print(F("Invalid bit cannot be combined with other bits in "));
        Serial1.println(buildvalue, HEX);
        return;
      } else if ((bitRead(buildvalue, 12) == 1) && ((buildvalue & 0x6FFF) != 0)) {
        cmdLine[0] = ' ';
        Serial1.print(F("Space cannot be combined with other bits in "));
        Serial1.println(buildvalue, HEX);
        return;
      } else if ((bitRead(buildvalue, 13) == 1) && ((buildvalue & 0x5FFF) != 0)) {
        cmdLine[0] = ' ';
        Serial1.print(F("Space cannot be combined with other bits in "));
        Serial1.println(buildvalue, HEX);
        return;
      } else if (bitRead(buildvalue, 14) == 1) {
        cmdLine[0] = ' ';
        Serial1.print(F("Bit 14 must be 0 in "));        
        Serial1.println(buildvalue, HEX);
      } 
    }
    Serial1.print(buildvalue, HEX);
    Serial1.print(F(" in table entry for decimal entry "));
    Serial1.print(digit);
    Serial1.print(F(" ("));
    showme[0] = digit;
    showme[1] = 0;
    Serial1.print(showme);
    Serial1.println(F(")"));
    asciiTab[digit] = buildvalue;
    
  } else if (cmdLine[0] == 'D') {
    switch (digit) {
      case 0:
        diag0();
        Serial1.println(F("OK Diagnostic"));
        break;
        
     case 1:
        diag1();
        Serial1.println(F("OK Diagnostic"));
        break;
        
     default:
       Serial1.println(F("System error, invalid diagnostic level number"));
       break;
    }
    
  } else if (cmdLine[0] == 'R') {
    
    // do a read
    doRead();
    
  } else if (cmdLine[0] == 'P') {
    
    // do a punch and possibly verify
    Serial1.print(F("Punching Card of ")); // CVC
    Serial1.print(punchLength); // CVC
    Serial1.println(F(" columns")); // CVC
    doPunch();
    
  } else {
      Serial1.print(F("Invalid verb found in execute area "));
      Serial1.println(cmdLine[0]);
  }
  
  // open the spigot now that we are done with command processing
  Serial1.print(char(XON));
  
  return;
}
