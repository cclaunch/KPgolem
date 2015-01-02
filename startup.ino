#include <avr/io.h>
#include <avr/pgmspace.h>
//
// setup routine for interface, run at startup
//

void setup() {
  
  char   pchtest;
  char   rdrtest;
  String hello = String("Starting Keypunch Interface V");
  hello = hello + VERSION;
  Serial.begin(9600);
  Serial.println(hello);
  Serial1.begin(9600, SERIAL_8N1);
  Serial1.print(char(XON));
  Serial1.println(hello);
    
  digitalWrite(PUN12, DROP);
  digitalWrite(PUN11, DROP);
  digitalWrite(PUN0, DROP);
  digitalWrite(PUN1, DROP);
  digitalWrite(PUN2, DROP);
  digitalWrite(PUN3, DROP);
  digitalWrite(PUN4, DROP);
  digitalWrite(PUN5, DROP);
  digitalWrite(PUN6, DROP);
  digitalWrite(PUN7, DROP);
  digitalWrite(PUN8, DROP);
  digitalWrite(PUN9, DROP);
  
  digitalWrite(COL1, DROP);
  digitalWrite(MPC, DROP);
  digitalWrite(REL, DROP);
  digitalWrite(SPC, DROP);
  
  pinMode(PUN12, OUTPUT);
  pinMode(PUN11, OUTPUT);
  pinMode(PUN0,  OUTPUT);
  pinMode(PUN1,  OUTPUT);
  pinMode(PUN2,  OUTPUT);
  pinMode(PUN3,  OUTPUT);
  pinMode(PUN4,  OUTPUT);
  pinMode(PUN5,  OUTPUT);
  pinMode(PUN6,  OUTPUT);
  pinMode(PUN7,  OUTPUT);
  pinMode(PUN8,  OUTPUT);
  pinMode(PUN9,  OUTPUT);
  
  pinMode(COL1,  OUTPUT);
  pinMode(MPC,  OUTPUT);
  pinMode(REL,  OUTPUT);
  pinMode(SPC,  OUTPUT);
  pinMode(REG,  INPUT);

  pinMode(RDRT1, OUTPUT);
  pinMode(RDRT2, INPUT);

  digitalWrite(PCHT1, LOW);
  pinMode(PCHT1, OUTPUT);
  pinMode(PCHT2, INPUT);
  
  pchtest = digitalRead(PCHT2);
  digitalWrite(PCHT1, HIGH);
 if ((digitalRead(PCHT2) == LOW) || (pchtest == HIGH)) {
   Serial1.println(F("No punch cable connection, aborting"));
   Serial1.end();
   Serial.end();
   exit;
 } 
  
  digitalWrite(RDRT1, LOW);
  rdrtest = digitalRead(RDRT2);
  digitalWrite(RDRT1, HIGH);
  if ((digitalRead(RDRT2) == LOW) || (rdrtest == HIGH)) {
    Serial1.println(F("Reader cable not attached, output only today"));
    haveReadCable = false;
  } else {
    pinMode(RDR12, INPUT);
    pinMode(RDR11, INPUT);
    pinMode(RDR0,  INPUT);
    pinMode(RDR1,  INPUT);
    pinMode(RDR2,  INPUT);
    pinMode(RDR3,  INPUT);
    pinMode(RDR4,  INPUT);
    pinMode(RDR5,  INPUT);
    pinMode(RDR6,  INPUT);
    pinMode(RDR7,  INPUT);
    pinMode(RDR8,  INPUT);
    pinMode(RDR9,  INPUT);
    haveReadCable = true;
    Serial1.println(F("Reader cable detected and activated"));
  }
  
  // load the std ASCII table now (default is to copy EBCDIC)
  cmdCode = 1;
  loadASCII();
      
  cmdLength = -1;       // will flush initial input stream up to the first _ character
  escapecheck = false;
  flushMode = false;
  flowControl = false;
  
  Serial1.println(F("READY"));
}
