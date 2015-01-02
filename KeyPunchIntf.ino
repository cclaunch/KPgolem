#include <MemoryFree.h>
#include <inttypes.h>
#include <ctypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
// 
// Interface to IBM Keypunch allowing punching and reading of cards
// 
// works with 029 A and B models using wire relays and 026 models
//

// global variables
  unsigned char    cmdLine[403];
  unsigned int     punchBuffer[80];
  int              punchLength;
  unsigned int     lastCard[80];
  int              lastLength = 0;
  int              cmdLength;
  unsigned char *  choice  =  cmdLine;
  int              digit;
  unsigned int     asciiTab[128];
  int              cmdMode = 1;  // 1 ASCII    2 BINARY     3 USERTABLE
  boolean          cmdVerify = false;
  int              curCol = 1;
  unsigned int     readIn;
  boolean          escapecheck;
  boolean          flushMode;
  boolean          flowControl;
  boolean          haveReadCable;
  int              cmdCode = 1;  // 1 EBCDIC    2 BCD
  
void loop() {
  
  while (getCommand() == 0);              // extract next command line from serial port

  if (parseCommand() == 0) {         // interpret and validate the command line
    execCommand();                   // execute the requested service
  } else {
    // Parse error so no execution
  }
  cmdLength = 0;
  cmdLine[0] = 0;
  choice = cmdLine;
  
}
