//
//   responsible for parsing the commands and validating choices
//
//   each command has a verb and then data or choices
//
//   verbs
//    P                - data steam of up to 80 columns follows, char or HHHH is four
//                            hex digits per binary char with one space between each 
//                            column, NL encountered means short card, release early
//    R                - ignores remainder, will read a card and return it to the caller
//                            in mode, e.g. ASCII char, userTable char or HHHH plus space
//    MODE             - choices are ASCII, BINARY or USERTABLE
//    CODE             - choices for ASCII are BCD or EBCDIC, initial mode is EBCDIC
//    VERIFY           - choices are ON or OFF
//    LOAD             - will install a new value in the user table for one character
//                       _LOAD ddd hhhh       
//                             ddd is decimal value of character
//                             hhhh are hex digits to install
//    DIAG             - number of an implemented diagnostic test
//                            0 - basic test of connectivity
//                            1 - basic plus punch a card to test all operations
//                              (note test 1 requires reader cable to verify results)
//
//      if FULLVERBS is set to 0, only the first letter is required for each verb
//

int  parseCommand() {
  
  char   test[2];
  int    separator;
  char   showme[5];
  int    parmlength;
  int    remaining;
  int    stepsize;
  
    
  // ignore case for the first char here, so set up string that is just first character
  test[0] = cmdLine[0];                   // pick up first character of command line
  test[1] = 0;                            // turn it into a terminated string of one char
  strupr(test);                           // make the new string upper case for the purpose of our switch command

  switch (test[0]) { 
    case 'P':
      cmdLine[0] = 'P';
      Serial.print(F("processing P punch command for data length "));
      if (cmdMode == 2) {
        Serial.println(cmdLength/5);
        stepsize = 4;
      } else {
        Serial.println(cmdLength-1);
        stepsize = 1;
      }
      
      punchLength = 0;
            
      // pick up remainder of line in a loop
      for (int i = 0; i < (cmdLength - 1); i += stepsize) {
        remaining = cmdLength - i - 1;         
        
        if ((cmdMode == 2) && (remaining < 4 )) {                // we don't have room for four hex digits
          for (int k = 0; k < remaining; k++) {
            char checkme = cmdLine[i+k+1]; 
            switch (checkme) {
              case ' ':                                            // okay to have trailing blanks
                continue;
                
              case 13:                                            // okay to have trailing CR       
                continue;
                
              case 10:                                            // okay to have trailing NL   
                continue;
                
              default:
                Serial1.println(F("Invalid characters at end of binary punch line"));
                cmdLine[0] = ' ';                                // make verb invalid (blank)
                k = remaining;
                break;
            }
          }
          if (cmdLine[0] == ' ') {
            break;
          }
        } else {
          // translate according to mode and store in buffer (1 or 4 
          punchBuffer[punchLength++] = transChar(i+1);
          if (punchBuffer[punchLength - 1] == 0x8000) {
            Serial1.print(F("Invalid character specified at column "));
            Serial1.println(punchLength); 
          }
          
          // error checking for required blank space in binary mode when more columns of input may follow
          if ((cmdMode == 2) && (remaining >= 9)) { 
            if ((cmdLine[i+5] != ' '))  {
              Serial1.println(F("Must have a blank between sets of four hex digits for binary mode"));
              cmdLine[0] = ' ';                                // make verb invalid (blank)     
              punchLength = 0;            
              break;
            } else {
              i++;                                            // bump extra position to cover the space
            }
          }
          
        }
      
      }
      break;
      
    case 'R':
      if (haveReadCable == false) {
        Serial1.println(F("No read cable, read command not valid"));
        cmdLine[0] = ' ';                                // make verb invalid (blank)
      } else {
        cmdLine[0] = 'R';
        Serial.println(F("processing R read command"));
      }
      break;
      
    case 'D':
      // check this is diag
      if ((FULLVERBS == 0) && (cmdLength >= 3) && (cmdLine[1] == ' ')) {
        cmdLine[0] = 'D';
        digit = getNum(2);
      } else if ((FULLVERBS == 1) && (memcmp_P(cmdLine, Cdiag, 4) == 0) and (cmdLine[4] == ' ')) {
        cmdLine[0] = 'D';
        digit = getNum(5);
      } else {
        Serial1.println(F("Malformed DIAG command"));
        cmdLine[0] = ' ';                                // make verb invalid (blank)
        break;
      }
      switch (digit) {
        case -1:
          Serial1.println(F("DIAG command must contain decimal number"));
          cmdLine[0] = ' ';                                // make verb invalid (blank)
          break;
        case 0:                  // quick validation
          break;
        case 1:                  // punch test patterns and verify through reader contacts
          break;
        default:
          Serial1.print(F("Invalid diagnostic number "));
          Serial1.print(digit);
          Serial1.println(F(" entered"));
          cmdLine[0] = ' ';                                // make verb invalid (blank)
          break;
        }
      break;
           
    case 'M':
      if ((FULLVERBS == 0) && (cmdLength >= 3)) {
        cmdLine[0] = 'M';
        int x = 0;
        while (isblank(cmdLine[++x]));              // skip forward over blanks (space or tab)
        if (cmdLength <= x) {
          Serial1.println(F("M must have nonblank operand"));
          cmdLine[0] = ' ';                                // make verb invalid (blank)
        } else if (cmdLine[x] == 'A') {
          cmdMode = 1;
        } else if (cmdLine[x] == 'B') {
          cmdMode = 2;
        } else if (cmdLine[x] == 'U') {
          cmdMode = 3;
        } else {
          Serial1.println(F("M must be A[scii], B[inary] or U[sertab]"));
          cmdLine[0] = ' ';                                // make verb invalid (blank)
        }
      } else if ((FULLVERBS == 1) && (cmdLength >= 9) && (memcmp_P(cmdLine, Cmode, 4) == 0) && (cmdLine[4] == ' ')) {
        cmdLine[0] = 'M';
        int x = 3;
        while (isblank(cmdLine[++x]));              // skip forward over blanks (space or tab)
        if (cmdLength <= x) {
          Serial1.println(F("MODE must have nonblank operand"));
          cmdLine[0] = ' ';                                // make verb invalid (blank)
        } else if (memcmp_P(((const char *)cmdLine + x), CASCII, 5) == 0) {
          cmdMode = 1;
        } else if (memcmp_P((const char *)(cmdLine + x), CBINARY, 6) == 0) {
          cmdMode = 2;
        } else if (memcmp_P((const char *)(cmdLine + x), CUSERTAB, 7) == 0) {
          cmdMode = 3;
        } else {
          Serial1.println(F("MODE must be ASCII, BINARY or USERTAB"));
          cmdLine[0] = ' ';                                // make verb invalid (blank)
        }
      } else {
        Serial1.println(F("Malformed MODE command"));
        cmdLine[0] = ' ';                                // make verb invalid (blank)
      }
      break; 
      
    case 'V':
      if ((FULLVERBS == 1) && (cmdLength >= 9) && (memcmp_P(cmdLine, Cverify, 6) == 0) && (cmdLine[6] == ' ')){
          cmdLine[0] = 'V';
          int x = 5;
          while (isblank(cmdLine[++x]));              // skip forward over blanks (space or tab)
          if (cmdLength <= x) {
            Serial1.println(F("VERIFY must have nonblank operand"));
            cmdLine[0] = ' ';                                // make verb invalid (blank)
          } else if (memcmp_P(((const char *)cmdLine + x), CON, 2) == 0) {
            if (cmdLength > x+3) {
              Serial1.println(F("Extra characters after VERIFY ON"));
              cmdLine[0] = ' ';                                // make verb invalid (blank)
              break;              
            }
            if (cmdLength > x+4) {
              Serial1.println(F("Extra characters after VERIFY OFF"));
              cmdLine[0] = ' ';                                // make verb invalid (blank)
              break;              
            }
            if (cmdVerify == false) lastLength = 0;    // empty
            if (haveReadCable == false) {
              Serial1.println(F("No read cable, so verify cannot be used"));
              cmdLine[0] = ' ';                                // make verb invalid (blank)
              break;
            } 
            cmdVerify = true;
          } else if (memcmp_P(((const char *)cmdLine + x), COFF, 3) == 0) {
            if (cmdVerify == true) lastLength = 0;    // empty 
            cmdVerify = false;
          } else {
            Serial1.println(F("VERIFY can only be ON or OFF"));
            cmdLine[0] = ' ';                                // make verb invalid (blank)
          } 
        } else if ((FULLVERBS == 0) && (cmdLength >= 4)) {
          cmdLine[0] = 'V';
          int x = 0;
          while (isblank(cmdLine[++x]));              // skip forward over blanks (space or tab)
          if (cmdLength <= x) {
            Serial1.println(F("V must have nonblank operand"));
            cmdLine[0] = ' ';                                // make verb invalid (blank)
          } else if (memcmp_P(((const char *)cmdLine + x), CON, 2) == 0) {
            if (cmdVerify == false) lastLength = 0;    // empty
            if (haveReadCable == false) {
              Serial1.println(F("No read cable, so verify cannot be used"));
              cmdLine[0] = ' ';                                // make verb invalid (blank)
              break;
            } 
            cmdVerify = true;
            lastLength = 0;                                  // ensure we don't try to verify the first card punched after this command
          } else if (memcmp_P(((const char *)cmdLine + x), COFF, 3) == 0) {
            if (cmdVerify == true) lastLength = 0;    // empty 
            cmdVerify = false;
            lastLength = 0;                                  // ensure we don't try to verify any cards punched after this command
          } else {
            Serial1.println(F("V can only be ON or OFF"));
            cmdLine[0] = ' ';                                // make verb invalid (blank)
          } 
      } else {
        Serial1.println(F("Malformed VERIFY command"));
        cmdLine[0] = ' ';                                // make verb invalid (blank)
      }
      break;
      
    case 'C':
      if ((FULLVERBS == 1) && (cmdLength >= 8) && (memcmp_P(cmdLine, Ccode, 4) == 0) && 
        (cmdLine[4] == ' ')){
          cmdLine[0] = 'C';
          int x = 4;
          while (isblank(cmdLine[++x]));              // skip white space before parameter
          if (cmdLength <= x) {
            Serial1.println(F("CODE must have nonblank operand"));
            cmdLine[0] = ' ';                                // make verb invalid (blank)
          } else if (memcmp_P(((const char *)cmdLine + x), CBCD, 3) == 0) {
            cmdCode = 2;
            loadASCII();
          } else if (memcmp_P(((const char *)cmdLine + x), CEBCDIC, 6) == 0) {
            cmdCode = 1;
            loadASCII();
          } else {
            Serial1.println(F("CODE can only be BCD or EBCDIC"));
            cmdLine[0] = ' ';                                // make verb invalid (blank)
          } 
        } else if ((FULLVERBS == 0) && (cmdLength >= 3)) {
          cmdLine[0] = 'C';
          int x = 0;
          while (isblank(cmdLine[++x]));              // skip white space before parameter
          if (cmdLength <= x) {
            Serial1.println(F("C must have nonblank operand"));
            cmdLine[0] = ' ';                                // make verb invalid (blank)
          } else if (cmdLine[x] == 'B') {
             cmdCode = 2;
             loadASCII();
          } else if (cmdLine[x] == 'E') {
            cmdCode = 1;
            loadASCII();
          } else {
            Serial1.println(F("C can only be B[cd] or E[bcdic]"));
            cmdLine[0] = ' ';                                // make verb invalid (blank)
          }
      } else {
        Serial1.println(F("Malformed CODE command"));
        cmdLine[0] = ' ';                                // make verb invalid (blank)
      }
      break;
      
    case 'L':
      if ((FULLVERBS == 1) && (cmdLength >= 11) && (memcmp_P(cmdLine, Cload, 4) == 0) && (cmdLine[4] == ' ')) {
          cmdLine[0] = 'L';
          digit = getNum(5);
          if ((digit < 0) || (digit > 127)) {
            Serial1.print(F("LOAD with invalid table entry number "));
            Serial1.print(digit);
            Serial1.println(F(" selected"));
            cmdLine[0] = ' ';                                // make verb invalid (blank)
            break;
          } 
          if ((digit == XON) || (digit == XOFF)) {
            Serial1.println(F("Cannot use flow control characters XON(x11) or XOFF(x13)"));
            cmdLine[0] = ' ';                                // make verb invalid (blank)
            break;
          }
          int x = 3;
          while (isblank(cmdLine[++x]));               // skip initial white space
          if ((x+5) > cmdLength) {                        // not enought left to hold entry
            Serial1.println(F("LOAD too short, need dd hhhh"));
            cmdLine[0] = ' ';
            break;
          }
          while (isdigit(cmdLine[++x]));               // skip the numeric value
          if ((x+4) >= cmdLength) {
            Serial1.print(F("LOAD too short, need hhhh after dd"));
            cmdLine[0] = ' ';
            break;
          }
          while (isblank(cmdLine[++x]));               // skip white space between paramters
          if ((x+3) >= cmdLength) {
            Serial1.println(F("LOAD too short, need four hex digits"));
            cmdLine[0] = ' ';
            break;
          }
          if (testHex(x)) {
              choice = cmdLine + x;                  // save our ptr
              break;
          } else {
              Serial1.print(F("Load command for entry "));
              Serial1.print(digit);
              Serial1.print(F(" needs four hex digits as operand, got "));
              showme[0] = cmdLine[x];
              showme[1] = cmdLine[x+1];
              showme[2] = cmdLine[x+2];
              showme[3] = cmdLine[x+3];
              showme[4] = 0;
              Serial1.println(showme);
              cmdLine[0] = ' ';                                // make verb invalid (blank)
              break;
          }
        } else if ((FULLVERBS == 0) && (cmdLength >= 8)) {
          cmdLine[0] = 'L';
          digit = getNum(2);
          if ((digit < 0) || (digit > 127)) {
            Serial1.print(F("L with invalid table entry number "));
            Serial1.print(digit);
            Serial1.println(F(" selected"));
            cmdLine[0] = ' ';                                // make verb invalid (blank)
            break;
          } 
          if ((digit == XON) || (digit == XOFF)) {
            Serial1.println(F("Cannot use flow control characters XON(x11) or XOFF(x13)"));
            cmdLine[0] = ' ';                                // make verb invalid (blank)
            break;
          }
          int x = 0;
          while (isblank(cmdLine[++x]));               // skip initial white space
          if ((x+5) > cmdLength) {                        // not enought left to hold entry
            Serial1.println(F("LOAD too short, need dd hhhh"));
            cmdLine[0] = ' ';
            break;
          }
          while (isdigit(cmdLine[++x]));               // skip the numeric value
          if ((x+4) >= cmdLength) {
            Serial1.println(F("LOAD too short, need hhhh after dd"));
            cmdLine[0] = ' ';
            break;
          }
          while (isblank(cmdLine[++x]));               // skip white space between paramters
          if ((x+3) >= cmdLength) {
            Serial1.println(F("LOAD too short, need four hex digits"));
            cmdLine[0] = ' ';
            break;
          }
          if (testHex(x)) {
            choice = cmdLine + x;                  // save our pointer
            break;
          } else {
            Serial1.print(F("L command for entry "));
            Serial1.print(digit);
            Serial1.print(F(" needs four hex digits as operand, got "));
            showme[0] = cmdLine[x];
            showme[1] = cmdLine[x+1];
            showme[2] = cmdLine[x+2];
            showme[3] = cmdLine[x+3];
            showme[4] = 0;
            Serial1.println(showme);
            cmdLine[0] = ' ';                                // make verb invalid (blank)
          }
      } else {
        Serial1.println(F("Malformed LOAD command"));
        cmdLine[0] = ' ';                                // make verb invalid (blank)
      }
      break;
      
    default:
      Serial1.print(F("Invalid command "));
      Serial1.println(cmdLine[0]);
      cmdLine[0] = ' ';                                // make verb invalid (blank)
      break;
  }
  if (cmdLine[0] == ' ') {
    return 1;
  } else {
    return 0;
  }
  
}
