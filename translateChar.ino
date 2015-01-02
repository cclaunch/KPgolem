//
// will translate a character using the current mode (ascii, binary or user table)
//

unsigned int  transChar(int pointer) {
  
      unsigned int punchOut;
      int      tableIndex;
      unsigned int buildvalue;
      unsigned char showme;

      switch (cmdMode) {
      case 1:                              // ascii table
        tableIndex = cmdLine[pointer];
        punchOut = asciiTab[tableIndex];
        return punchOut;
        
      case 2:                              // binary
        buildvalue = 0;
        for (int i = 0; i < 4; i++) {
          switch (cmdLine[pointer + i]) {
            case '0':
              buildvalue = (buildvalue << 4);
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
              Serial1.print(F("Improper hex digit "));
              showme = cmdLine[pointer + i];
              Serial1.print(showme);
              Serial1.println(F(" in input for binary mode PUNCH command"));
              return 0x8000;
          }
        }
        punchOut = buildvalue;
        return punchOut;
        
      case 3:                              // user table
        tableIndex = cmdLine[pointer];
        punchOut = asciiTab[tableIndex];
        return punchOut;
        
      default:
        Serial1.println(F("Internal error, mode invalid"));
        return 0x8000;
    }

  
}
