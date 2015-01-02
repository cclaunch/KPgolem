//
// writeKP routine will do a fast and virtually simultaneous write to all
// punch solenoids, changing the output ports of the ATMEL processor 
// directly rather than using digitalWrite to each sequentially
//
// PICK is done by setting the pin to 0, DROP by returning it to 1
// covers the 12 punch pins plus space and release pins
// 

void  writeKP(unsigned int doMe) {
  
  byte  portA, portB, portL;
  
  // set up with zero bit for each relay to be picked
  portA =  ((doMe & 0X1F00) >> 8);                // get the high byte, low five bits, which match port A exactly
  portB =  ((doMe & 0X00C0) >> 6);                // get pun 2 and 3 bits in position for port B
  portB += ((doMe & 0X0030) >> 2);                // add in pun 4 and 5 bits (port B is xxxx4523)
  portL =  ((doMe & 0X000C) >> 2);                // get pun 6 and 7 in position for port L
  portL += ((doMe & 0X0003) << 2);                // add in pun 8 and 9 bits (port L is xxxx8967)

  // 
  // set up as a mask which will flip off the bits we have set when applied to the real ports
  portA = portA ^ B11111111;                      // result is 0 if portA was 1, otherwise it is 1
  portB = portB ^ B11111111;                      // same for port B   ( B00010000 becomes B11101111)
  portL = portL ^ B11111111;                      // and for port L    ( B00000110 becomes B11111001)
  
  // now pick the relays as requested (set them to 0)
  PORTA &= portA;                                 // every zero in the mask turns that port to 0
  PORTB &= portB;                                 // do same for port B
  PORTL &= portL;                                 // which makes each 1 in input be a 0 on port
  
  // mask off bits which are not our relays on the ports
  portA &= B00011111;                             // don't touch bits 7 6 and 5 of this port
  portB &= B00001111;                             // don't touch bits 7 6 5 and 4 of this port
  portL &= B00001111;                             // don't touch bits 7 6 5 and 4 of this port
  
  // and drop relays as requested by input (set them to 1)
  PORTA |= portA;                                 // turn on relays which were zero in the input
  PORTB |= portB;                                 // relays in this port too
  PORTL |= portL;                                 // now all 1 inputs are 0 in port and 0 inputs are 1 in port
  
  return;
}
