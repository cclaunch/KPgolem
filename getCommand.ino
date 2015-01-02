//
// retrieves a command line from serial port 1 and stores it in cmdLine
//
// command lines begin with a delimiter character and end with the next 
// unescaped such character. The default delimiter is _
//
// when more than one delimiter arrives sequentially, each pair is converted to a 
// single data character of that value ending when we have a single remaining
// delimiter character, which reverts to its control delineating function
//
// the string cmdLine contains the incoming data, with the beginning delimiter 
// stripped and escaped delimiter pairs converted to single instances, plus an NL
//
// the length of the line is stored in cmdLength as the line is built. 

int getCommand() {
  
  unsigned char inputByte;
      
  // loop while input is available
  while (Serial1.available() > 0) {   
    
    inputByte = Serial1.peek();                  // get a character from serial port 1, but don't pop it yet
    
    // handle XON and XOFF flow control here
    if (inputByte == XOFF) {
      flowControl = false;
    }
    if (inputByte == XON) {
      flowControl = true;
    }
    
    // first we find the start of command line character (_ is default) as a one time action
    if (cmdLength == -1) {
      if (inputByte != DELIM) {
        cmdLine[0] = 0;
        // flush unwanted characgters
      } else {
        cmdLength = 0;
        cmdLine[0] = 0;
        choice = cmdLine;
      }
      Serial1.read();                            // pop it off the queue since we consumed it
      continue;
    }
    
    // check for escaped delimiter which should be stored as an ordinary character
    if (escapecheck) {
      if (inputByte == DELIM) {
        escapecheck = false;                     // done with this pair, go save it
      } else {
        if (flowControl == true) {
          return 0;                              // don't activate the command until flow control is released
        } else {
          Serial1.print(F("Received command: "));  
          Serial1.println((char *)cmdLine);  
          escapecheck = false;                    // real delimiter, not escape sequence
          flushMode = false;
          return cmdLength;                       // actual finish of command line, don't pop it and end the reading of this line
        }
      }  
    } else if (inputByte == DELIM) {          
      escapecheck = true;  
      Serial1.read();          // pop off queue   
      continue;      
    }
    
    // convert invalid characters, where high bit is 1 (from 8 bit stream)
    if (inputByte > 127) {
      inputByte = 0;         // arbitrarily substitute NULL
      Serial1.print(F("Extended ASCII character received (>127), converted to NULL"));
    }
    
    // save the character and bump up the command line length if not flushing
    Serial1.read();                             // pop it off the queue
    if (flushMode == false) {
      cmdLine[cmdLength++] = inputByte;        // store character in slot in array then bump length by 1
      cmdLine[cmdLength] = 0;                  // null delimit the string for use with printing
    }
        
    if ((cmdMode == 2) && (cmdLength > 400)) {
      flushMode = true;
      cmdLength = 0;
      cmdLine[0] = '*';
      cmdLine[1] = 'F';
      cmdLine[2] = 'L';
      cmdLine[3] = 'U';
      cmdLine[4] = 'S';
      cmdLine[5] = 'H';
      cmdLine[6] = 'E';
      cmdLine[7] = 'D';
      cmdLine[8] = '*';
      cmdLine[9] = 0;
      Serial1.println(F("Excessive input line length, more than 400 characters (80 columns), ignoring this command"));
    } else if ((cmdMode != 2) && (cmdLength > 81)) {
      flushMode = true;
      cmdLength = 0;
      cmdLine[0] = '*';
      cmdLine[1] = 'F';
      cmdLine[2] = 'L';
      cmdLine[3] = 'U';
      cmdLine[4] = 'S';
      cmdLine[5] = 'H';
      cmdLine[6] = 'E';
      cmdLine[7] = 'D';
      cmdLine[8] = '*';
      cmdLine[9] = 0;
      Serial1.println(F("Excessive input line length, more than 81 characters (80 columns), ignoring this command"));
    }
  }

  return 0;
}

