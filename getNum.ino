//
// getNum routine which extracts a decimal number from the remainder of a command line
//    passed index of the starting character of the line
//    returns the positive decimal value or -1 for error
//    skips blanks until a decimal digit is found
//    stops when a space or newline or the end of the command is reached
//    if a non-decimal and non-space character is found, an error occured

int  getNum(int start) {
  int  accumulate = 0;
  int  sawdigit = 0;
  for (int i=start; i<cmdLength; i++) {
    char check = cmdLine[i];
    switch (check) {
      case ' ':
        if (sawdigit == 0) {
          continue;                 // skip over initial spaces
        } else {
          return accumulate;        // return our value when we find terminating space
        }
      case '0':
        sawdigit = 1;
        accumulate = accumulate*10;
        continue;
      case '1':
        sawdigit = 1;
        accumulate = accumulate*10 + 1;
        continue;
      case '2':
        sawdigit = 1;
        accumulate = accumulate*10 + 2;
        continue;
      case '3':
        sawdigit = 1;
        accumulate = accumulate*10 + 3;
        continue;
      case '4':
        sawdigit = 1;
        accumulate = accumulate*10 + 4;
        continue;
      case '5':
        sawdigit = 1;
        accumulate = accumulate*10 + 5;
        continue;
      case '6':
        sawdigit = 1;
        accumulate = accumulate*10 + 6;
        continue;
      case '7':
        sawdigit = 1;
        accumulate = accumulate*10 + 7;
        continue;
      case '8':
        sawdigit = 1;
        accumulate = accumulate*10 + 8;
        continue;
      case '9':
        sawdigit = 1;
        accumulate = accumulate*10 + 9;
        continue;
      case '\n':
        return accumulate;
      case '\r':
        return accumulate;
      default:
        return -1;              // not a valid decimal character, bail out
    }
  }
  if (sawdigit == 1) {
    return accumulate;          // we ran to end of command with digits, return value
  } else {
    return -1;                  // error, ended without finding a number
  }
}
